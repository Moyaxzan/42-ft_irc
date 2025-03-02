#include "../include/Bot.hpp"

Bot::Bot(char *port, char *pwd)
{
    std::ifstream infile;
    std::string line;
    DIR *profanities_dir;
    dirent *curr;

    profanities_dir = opendir("./profanities");
    if (profanities_dir)
    {
        while ((curr = readdir(profanities_dir)))           //Going through each file in "./profanities" and parsing them into this->profanities_
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

    this->sendMsg("CAP LS 302", 0);
    std::string res = this->recvMsg();
    if (res.find("CAP END") != res.npos)        // Proper client authentification
    {
        this->sendMsg("PASS " + std::string(pwd), 0);
        res = this->recvMsg();
        if (res.find("Invalid password") != res.npos)
            throw WrongPassword();
        this->sendMsg("NICK " + std::string("sheriff"), 0);
        this->sendMsg("USER sheriff localhost localhost: le_sheriff", 0);
    }
}

std::vector<std::vector<std::string> > &Bot::getDicts(void)
{
    return this->profanities_;
}

void Bot::sendMsg(std::string const &to_send, int time)
{
    int bytes_sent = 0;
    int str_len = to_send.length();
    const char *str = to_send.c_str();
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
        std::cerr << "There was a problem opening a profanities file : " << std::strerror(errno) << "\n";
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
                this->sendMsg("BOT: This is your last warning " + username + "! Next time you're getting some time off.\n", 0);
                return ;
            }
            else if ((*it).getStrike() == 2)
            {
                this->sendMsg("BOT: That's it " + username + " ! You are out! OUT !\n", 0);
                this->sendMsg("BOT: KICK MESSAGE SERVEUR " + username + "\n", 0);
                this->bad_people.erase(it);
                return ;
            }
        }
    }
    BadPerson first_warning(username);
    this->bad_people.push_back(username);
    this->sendMsg("BOT: First warning " + username + ". This is my channel and we're being polite around here.\n", 0);
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
    it = split_msg.begin() + 3; // iterator to the "real" begin of the user msg
    it->erase(0, 1);
    split_msg.begin()->erase(0, 1);
    final_msg.author = *split_msg.begin();
    while (it != split_msg.end())
    {
        std::string tmp = *it;
        if (!tmp.empty())
            final_msg.split_content.push_back(tmp);
        if (it + 1 != split_msg.end())
            final_msg.content += tmp + " ";
        else
            final_msg.content += tmp;
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
    std::vector<std::string>::iterator it = msg.split_content.begin();
    while (it != msg.split_content.end())
    {
        std::cout << "content = " << msg.content << "\n";
        if (msg.content == "START ROULETTE")
        {
            sendIntroRoulette(msg.author);
            launchRoulette();
            return ;
        }
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
        for (it = it2->begin(); it != it2->end(); it++) // Searching through all dicts to find an occurence
        {
            if (msg.original.find(*it) != std::string::npos && checkBadContent(msg.split_content, *it)) 
            {
                this->checkAddBadPerson(msg.author);
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
    sendMsg(username + " just threw down the gauntlet for a game of Russian Roulette! Saddle up, folks!\n", 0);
    sendMsg("The rules are simple, partner. We got ourselves a six-shooter, but only one chamber's got lead in it.\n", 2);
    sendMsg("Each turn, someone’s gotta cock the hammer and PULL the trigger, pointin’ it right at their own head.\n", 2);
    sendMsg("Feelin’ lucky? You can ROLL and give the cylinder a spin to shuffle things up—if you got the nerve.\n", 2);
    sendMsg("But one way or another, you’ll have to pull that trigger!\n", 2);
    sendMsg("What could go wrong ? Good luck fellers!\n", 2);
}

void Bot::rouletteLoop(std::vector<std::string> & players, Gun & gun)
{
    std::string msg;
    t_msg parsed_msg;
    
    while (1)
    {
        this->sendMsg("It's " + players[0] + "'s turn! Would you rather ROLL the barrel or PULL the trigger?\n", 2);
        while (!(msg = this->recvMsg()).empty())
        {
            parsed_msg = parseMsg(msg);
            this->monitor(parsed_msg);
            std::cout << parsed_msg.author << "\n";
            std::cout << "'" << parsed_msg.content << "'" << "\n";
            if (parsed_msg.author == players[0] && parsed_msg.content == "ROLL")
            {
                this->sendMsg("Rolling barrel...\n", 0);
                gun.shuffleBullets();
                this->sendMsg("Done !\n", 2);
                break ;
            }
            else if (parsed_msg.author == players[0] && parsed_msg.content == "PULL")
            {
                this->sendMsg("Cocking the hammer.. Ready ?\n", 0);
                for (int i = 3; i > 0; i--)
                    this->sendMsg("..\n", 2);
                if (gun.checkBullet())
                {
                    this->sendMsg("BANG!  " + parsed_msg.author + "died!\n", 3);
                    this->sendMsg("See you in another life.\n", 2);
                    this->sendMsg("KICK PLAYER MESSAGE SERVEUR\n", 0);
                    return ;
                }
                this->sendMsg(parsed_msg.author + " survived !\n", 0);
                std::rotate(players.begin(), players.begin() + 1, players.end());
                break ;
            }
        }
    }
}

void Bot::launchRoulette(void)
{
    Gun gun;
    std::vector<std::string> players;
    std::vector<std::string>::iterator it;
    std::string new_msg;

    // requete au serveur pour pick 6 joueurs random (max) dans le channel
    //  req = "1 3";
    players.push_back("jonas");
    players.push_back("maurice");
    // players.push_back("michel");
    // players.push_back("tarzon");
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
    this->sendMsg(intro, 2);
    std::srand(std::time(0));
    std::random_shuffle(players.begin(), players.end());
    intro.clear();
    intro = encapsulate(players[0]) + " will begin !\n";
    this->sendMsg(intro, 2);
    this->rouletteLoop(players, gun);
}

std::string encapsulate(std::string name)
{
    std::stringstream nb;

    nb << "[" << name << "]";
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
        this->sendMsg(line, 1);
    bear.close();
}
