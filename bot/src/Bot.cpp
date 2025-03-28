#include "../include/Bot.hpp"
#include "../include/global.hpp"

Bot::Bot(char *port, char *pwd)
{
    std::ifstream infile;
    std::string line;
    DIR *profanities_dir;
    dirent *curr;
    
    profanities_dir = opendir("./bot/profanities");
    if (profanities_dir)
    {
        while ((curr = readdir(profanities_dir)))
        addProfanityDict(std::string(curr->d_name));
        closedir(profanities_dir);
    }
    else
        std::cout << "No profanities file found!\n";
    this->initServerConnection_(port, pwd);
}

Bot::Bot(Bot const &other)
{
    *this = other;
}

Bot::~Bot(void)
{
    close(this->socket_);
}

Bot &Bot::operator=(Bot const &other)
{
    this->socket_ = other.socket_;
    this->profanities_ = other.profanities_;
    return *this;
}

void Bot::initServerConnection_(char *port, char *pwd)
{
    sockaddr_in server_infos;
    server_infos.sin_family = AF_INET;
    server_infos.sin_port = htons(atoi(port));
    server_infos.sin_addr.s_addr = inet_addr(SERV_IP);

    this->socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket_ == -1)
        throw SocketBotError();

    if (connect(this->socket_, (sockaddr *)&server_infos, sizeof(server_infos)) == -1)
        throw ConnectionError();

    this->sendMsg("CAP LS 302", 0, true);
    std::string res = this->recvMsg();
    this->sendMsg("PASS " + std::string(pwd), 0, true);
    res = this->recvMsg();
    if (res.find("Invalid password") != res.npos)
        throw WrongPassword();
    this->sendMsg("NICK " + std::string("sheriff"), 0, true);
    this->sendMsg("USER sheriff localhost localhost: le_sheriff", 0, true);
}

std::vector<std::vector<std::string> > &Bot::getDicts(void)
{
    return this->profanities_;
}

void Bot::sendMsg(std::string const &to_send, int time, bool server)
{
    std::string final_str;
    if (!server)    // server == true -> send server msg
        final_str = "PRIVMSG " + channel + " :" + to_send;
    else
        final_str = to_send;
    int str_len = final_str.length();
    const char *str = final_str.c_str();
    int bytes_sent = 0;
    int send_res;

    while (bytes_sent < str_len)
    {
        sleep(time);
        send_res = send(this->socket_, str + bytes_sent, str_len - bytes_sent, 0);
        if (send_res == -1)
        {
            if (errno == EINTR)
                continue;
            else
                throw SendError();
        }
        bytes_sent += send_res;
    }
}

std::string Bot::recvMsg(void)
{
    char buffer[512];
    int err;
    std::string full_msg;

    memset(buffer, '\0', 512);
    while (full_msg.find("\r\n") == full_msg.npos)
    {
        err = recv(this->getClientSocket(), buffer, 511, 0);
        if (err == 0 && full_msg.find("Invalid password"))
            throw WrongPassword();
        if (err == 0)
            throw ConnectionError();
        full_msg += std::string(buffer);
        memset(buffer, '\0', 512);
    }
    return (full_msg);
}

void Bot::addProfanityDict(std::string filename)
{
    if (filename.empty() || filename.compare(".") == 0 || filename.compare("..") == 0)
        return;

    std::ifstream infile;
    std::string line;
    std::vector<std::string> dict;

    infile.open(("./bot/profanities/" + filename).c_str(), std::ios::in);
    if (!infile.is_open())
    {
        this->fileError();
        return;
    }
    while (std::getline(infile, line, '\n'))
        dict.push_back(line);
    this->profanities_.push_back(dict);
}

void Bot::checkAddBadPerson(std::string username)
{
    std::vector<BadPerson>::iterator it;

    for (it = bad_people.begin(); it != bad_people.end(); it++)
    {
        if ((*it).getName() == username)
        {
            if ((*it).getStrike() == 1)
            {
                (*it).incStrike();
                this->sendMsg("Last damn chance, " RED + username + RESET + ". " + "Step wrong again, you’re gone.\n", 0, false);
                return ;
            }
            else if ((*it).getStrike() == 2)
            {
                this->sendMsg(BOLD "That's it " RED BLINK + username + RESET BOLD ". I'm kicking you out, feller. And don’t you dare crawl back." + RESET + "\n", 0, false);
                username.erase(0, 1);
                username.erase(username.size() - 1);
                std::cout << "KICK " + channel + " " + username << "\n";
                this->sendMsg("KICK " + channel + " " + username + " :so rude", 0, true);
                this->bad_people.erase(it);
                return ;
            }
        }
    }
    BadPerson first_warning(username);
    this->bad_people.push_back(username);
    this->sendMsg("Watch your mouth, cowboy. One more word outta line, and you’re dust. Understood, " ORANGE + username + RESET +"?\n", 0, false);
}

t_msg Bot::parseMsg(std::string recv_msg)
{
    std::vector<std::string> split_msg;
    std::vector<std::string>::iterator it;
    t_msg final_msg;

    final_msg.original = recv_msg;
    split_msg = split(recv_msg, " ");
    if (split_msg.size() < 4) // should not be necessary once msgs are correctly formatted
        return (final_msg); // is empty
    it = split_msg.end() - 1;
    if (it->find('\n') != it->npos && it->find("\r") != it->npos)
        it->resize(it->length() - 2);
    it = split_msg.begin() + 3;
    it->erase(0, 1);
    split_msg.begin()->erase(0, 1);
    final_msg.username = *split_msg.begin();
    final_msg.username = "<" + final_msg.username.substr(0, final_msg.username.find("!")) + ">";
    while (it != split_msg.end())
    {
        std::string tmp = *it;
        if (!tmp.empty())
            final_msg.content.push_back(std::string(*it));
        it++;
    }
    return final_msg;
}

bool Bot::checkBadContent(std::vector<std::string> & content, std::string const & bad_word)
{
    std::vector<std::string>::iterator it = content.begin();

    while (it != content.end())
    {
        if (*it == bad_word)
            return true;
        it++;
    }
    return false;
}

void Bot::checkRoulette(t_msg & msg)
{
    std::vector<std::string>::iterator it = msg.content.begin();
    while (it != msg.content.end())
    {
        if (*it == "START" && it + 1 != msg.content.end() && *(it + 1) == "ROULETTE")
            launchRoulette(msg);
        it++;
    }
}

void Bot::monitor(t_msg & msg)
{
    std::vector<std::string>::iterator it;
    std::vector<std::vector<std::string> >::iterator it2;
    std::vector<std::vector<std::string> > &all_dicts = this->getDicts();

    for (it2 = all_dicts.begin(); it2 != all_dicts.end(); it2++)
    {
        for (it = it2->begin(); it != it2->end(); it++)
        {
            if (msg.original.find(*it) != std::string::npos && checkBadContent(msg.content, *it))
            {
                this->checkAddBadPerson(msg.username);
                return ;
            }
        }
    }
}

int Bot::getClientSocket(void)
{
    return this->socket_;
}

void Bot::sendIntroRoulette(std::string username)
{
    sendMsg(GREEN + username + RESET + " just threw down the gauntlet for a game of Russian Roulette! Saddle up, folks!\n", 0, false);
    sendMsg("The rules are simple, partner. We got ourselves a six-shooter, but only one chamber's got lead in it.\n", 2, false);
    sendMsg("Each turn, someone’s gotta cock the hammer and PULL the trigger, pointin’ it right at their own head.\n", 2, false);
    sendMsg("Feelin’ lucky? You can ROLL and give the cylinder a spin to shuffle things up—if you got the nerve.\n", 2, false);
    sendMsg("But one way or another, you’ll have to pull that trigger!\n", 2, false);
    sendMsg("What could go wrong ? Good luck fellers!\n", 2, false);
}

static bool msg_cmp(std::vector<std::string> msg, std::string to_cmp)
{
    return (msg.size() == 1 && msg[0] == to_cmp);
}

void Bot::rouletteLoop(std::vector<std::string> & players, Gun & gun)
{
    std::string msg;
    t_msg parsed_msg;
    
    while (1)
    {
        this->sendMsg("It's " GREEN + players[0] + RESET + "'s turn! Would you rather ROLL the barrel or PULL the trigger?\n", 2, false);
        while (!(msg = this->recvMsg()).empty())
        {
            parsed_msg = parseMsg(msg);
            this->monitor(parsed_msg);
            if (parsed_msg.username == players[0] && msg_cmp(parsed_msg.content, "ROLL"))
            {
                this->sendMsg("Rolling barrel...\n", 0, false);
                gun.shuffleBullets();
                this->sendMsg("Done !\n", 2, false);
                break ;
            }
            else if (parsed_msg.username == players[0] && msg_cmp(parsed_msg.content, "PULL"))
            {
                this->sendMsg("Cocking the hammer.. Ready ?\n", 0, false);
                for (int i = 2; i > 0; i--)
                    this->sendMsg("..\n", 2, false);
                if (gun.checkBullet())
                {
                    this->sendMsg(RED BLINK "BANG! " RESET GREEN + parsed_msg.username + RESET + " died!\n", 3, false);
                    this->sendMsg("See you in another life, loser.\n", 2, false);
                    parsed_msg.username = parsed_msg.username.erase(0, 1);
                    parsed_msg.username = parsed_msg.username.erase(parsed_msg.username.length() - 1, 1);
                    this->sendMsg("KICK " + channel + " " + parsed_msg.username, 0, true);
                    return ;
                }
                this->sendMsg(GREEN + parsed_msg.username + RESET + " survived !\n", 3, false);
                std::rotate(players.begin(), players.begin() + 1, players.end());
                break ;
            }
        }
    }
}

bool Bot::checkIfOper(void)
{
    std::vector<std::string>::iterator it;
    t_msg req;

    sendMsg("NAMES " + channel, 0, true);
    req = parseMsg(recvMsg());
    for (it = req.content.begin(); it != req.content.end(); it++)
    {
        if ((*it).find("@sheriff") != std::string::npos)
            return true;
    }
    return false;
}

std::vector<std::string> Bot::getPlayersVec(void)
{
    std::vector<std::string> players;
    std::vector<std::string>::iterator it;
    t_msg msg;

    sendMsg("NAMES " + channel, 0, true);
    msg = parseMsg(recvMsg());
    for (it = msg.content.begin(); it != msg.content.end(); it++)
    {
        if ((*it)[0] == '#' || (*it).find("sheriff") != std::string::npos)
            continue;
        if ((*it)[0] == ':')
            (*it).erase(0, 1);
        if ((*it)[0] == '@')
            (*it).erase(0, 1);
        (*it).insert(0, "<");
        (*it).insert((*it).length(), ">");
        players.push_back(*it);
    }
    std::srand(std::time(0));
    std::random_shuffle(players.begin(), players.end());
    while (players.size() > 6)
        players.pop_back();
    for (it = players.begin(); it != players.end(); it++)
        std::cout << *it << "\n";
    return players;
}

void Bot::launchRoulette(t_msg const & msg)
{
    Gun gun;
    std::vector<std::string> players;
    std::vector<std::string>::iterator it;
    std::string new_msg;

    if (!checkIfOper())
    {
        this->sendMsg("I'm afraid I don't have operator rights fellers.. and therefore we can't play.\n", 0, false);
        return ;
    }
    players = this->getPlayersVec(); // requete au serveur pour pick 6 joueurs random (max) dans le channel
    sendIntroRoulette(msg.username);
    std::string intro = "Players " GREEN;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (it == players.end() - 1)
            intro += RESET "and " GREEN + (*it) + RESET + " ";
        else
            intro += (*it) + RESET ", " + GREEN;
    }
    intro += "have been selected !\n";
    this->sendMsg(intro, 2, false);
    intro = GREEN + players[0] + RESET + " will begin !\n";
    this->sendMsg(intro, 2, false);
    rouletteLoop(players, gun);
}

void Bot::fileError(void)
{
    std::cerr << "There was a problem opening a profanities file : " << std::strerror(errno) << "\n";
}

std::vector<std::string> split(std::string str, std::string delim)
{
    std::vector<std::string> tokens;
    std::string token;

    while (str.find(delim) != str.npos)
    {
        token = str.substr(0, str.find(delim));
        tokens.push_back(token);
        str.erase(0, str.find(delim) + delim.length());
    }
    tokens.push_back(str);
    return tokens;
}

void Bot::printBear(void)
{
    std::ifstream bear;
    std::string line;

    bear.open("./src/bear");
    if (!bear.is_open())
        return ;
    while (std::getline(bear, line, '\0'))
        this->sendMsg(line, 1, false);
    bear.close();
}
