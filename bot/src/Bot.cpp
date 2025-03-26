#include "../include/Bot.hpp"
#include "../include/global.hpp"

Bot::Bot(char *port, char *pwd)
{
    std::ifstream infile;
    std::string line;
    DIR *profanities_dir;
    dirent *curr;
    
    profanities_dir = opendir("./profanities");
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
    this->sendMsg("PASS " + std::string(pwd), 1, true);
    res = this->recvMsg();
    if (res.find("Invalid password") != res.npos)
        throw WrongPassword();
    this->sendMsg("NICK " + std::string("sheriff"), 1, true);
    this->sendMsg("USER sheriff localhost localhost: le_sheriff", 1, true);
}

std::vector<std::vector<std::string> > &Bot::getDicts(void)
{
    return this->profanities_;
}

void Bot::sendMsg(std::string const &to_send, int time, bool server)
{
    std::string final_str;
    if (!server)  // wont be necessary after invite
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
        err = recv(this->getClientSocket(), buffer, 512, 0);
        full_msg += std::string(buffer);
        if (err == 0 && full_msg.find("Invalid password"))
            throw WrongPassword();
        if (err == 0)
            throw ConnectionError();
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

    infile.open(("./profanities/" + filename).c_str(), std::ios::in);
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
                this->sendMsg("Last damn chance, " RED + username + RESET + ". " + UNDERLINE + "Step wrong again, you’re gone.\n", 0, false);
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
        if (*it == "START" && it + 1 != msg.content.end() && *(it + 1) == "RCR")
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
    printBear();
    sendMsg(username + " just threw down the gauntlet for a game of Russian Roulette! Saddle up, folks!\n", 0, false);
    sendMsg("The rules are simple, partner. We got ourselves a six-shooter, but only one chamber's got lead in it.\n", 2, false);
    sendMsg("Each turn, someone’s gotta cock the hammer and PULL the trigger, pointin’ it right at their own head.\n", 2, false);
    sendMsg("Feelin’ lucky? You can ROLL and give the cylinder a spin to shuffle things up—if you got the nerve.\n", 2, false);
    sendMsg("But one way or another, you’ll have to pull that trigger!\n", 2, false);
    sendMsg("Ain’t nothin’ to it... just a bit of good ol’ fashioned frontier luck.\n", 2, false);
    sendMsg("What could go wrong ? Good luck fellers!\n", 2, false);
}

void Bot::launchRoulette(t_msg const & msg)
{
    Gun gun;
    std::vector<int> players;
    std::vector<int>::iterator it;
    std::string new_msg;
    // std::string req;

    (void) msg;

    // requete au serveur pour pick 6 joueurs random (max) dans le channel
    //  req = "1 3";
    players.push_back(1);
    players.push_back(2);
    players.push_back(3);
    players.push_back(4);
    std::string intro = "Players ";
    for (it = players.begin(); it != players.end(); it++)
    {
        std::string player_nb = encapsulate(*it);
        if (it == players.end())
            intro += "and " + player_nb + " ";
        else
            intro += player_nb + ", ";
    }
    intro += "have been selected !\n";
    this->sendMsg(intro, 2, false);
    std::srand(std::time(0));
    std::random_shuffle(players.begin(), players.end());
    intro.clear();
    intro = encapsulate(players[0]) + " will begin ! Would you rather PULL the trigger or ROLL the bullets ?\n";
    this->sendMsg(intro, 2, false);
    while (!(new_msg = this->recvMsg()).empty())
    {
        
        if (new_msg.find("PULL") != new_msg.npos)
        {
            if (!gun.checkBullet())
                this->sendMsg("Survived\n", 0, false);
            else
            {
                this->sendMsg("Dead!\n", 0, false);
                break;
            }
        }
        else if (new_msg.find("ROLL") != new_msg.npos)
        {
            gun.shuffleBullets();
            this->sendMsg("Bullets have been rolled !\n", 0, false);
        }
    }
    std::cout << "Looks like someone died !\n";
}

void Bot::fileError(void)
{
    std::cerr << "There was a problem opening a profanities file : " << std::strerror(errno) << "\n";
}

std::string encapsulate(int i)
{
    std::stringstream nb;

    nb << "[" << i << "]";
    return nb.str();
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
