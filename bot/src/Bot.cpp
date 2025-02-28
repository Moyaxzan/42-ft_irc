#include "../include/Bot.hpp"

Bot::Bot(char *port)
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
    this->initServerConnection_(port);
}

Bot::Bot(Bot const &other)
{
    *this = other;
}

Bot::~Bot(void)
{
}

Bot &Bot::operator=(Bot const &other)
{
    this->socket_ = other.socket_;
    this->profanities_ = other.profanities_;
    return *this;
}

void Bot::initServerConnection_(char *port)
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

    // this->sendMsg("CAP LS 302");
    // std::string res = this->recvMsg();
    // if (res.find("CAP END") != res.npos)  // GOOD AUTH TO ADD BACK 
    // {
    //     this->sendMsg("PASS mdp\nNICK sheriff\nUSER le_sheriff");
    // }
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
        if (err == 0)
            throw ConnectionError();
        if (err == -1)
            std::cout << "problem\n";       // need to throw something ?
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
    it = split_msg.begin() + 3;
    it->erase(0, 1);
    split_msg.begin()->erase(0, 1);
    final_msg.username = *split_msg.begin();
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
            launchRoulette(msg.username);
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

void Bot::launchRoulette(std::string const & username)
{
    Gun gun;
    std::vector<int> players;
    std::vector<int>::iterator it;
    std::string msg;
    // std::string req;

    printBear();
    sendMsg(username + " just started a Russian Chat Roulette! LET'S GO!\n", 0);
    sendMsg("It's a quite simple game. There's a 6 bullets gun loaded with only one bullet.\n", 2);
    sendMsg("Each turn, each person picked will have to PULL the trigger and try shooting the gun to his/her face.\n", 2);
    sendMsg("Alternatively, you can also ROLL the gun barrel and randomize the bullets inside.\n", 2);
    sendMsg("But you will have to shoot the gun, eventually!\n", 2);
    sendMsg("What could go wrong ? Good luck !\n", 2);
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
    this->sendMsg(intro, 2);
    std::srand(std::time(0));
    std::random_shuffle(players.begin(), players.end());
    intro.clear();
    intro = encapsulate(players[0]) + " will begin ! Would you rather PULL the trigger or ROLL the bullets ?\n";
    this->sendMsg(intro, 2);
    while (!(msg = this->recvMsg()).empty())
    {

        if (msg.find("PULL") != msg.npos)
        {
            if (!gun.checkBullet())
                this->sendMsg("Survived\n", 0);
            else
            {
                this->sendMsg("Dead!\n", 0);
                break;
            }
        }
        else if (msg.find("ROLL") != msg.npos)
        {
            gun.shuffleBullets();
            this->sendMsg("Bullets have been rolled !\n", 0);
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
        this->sendMsg(line, 1);
    bear.close();
}

// std::vector<std::string> split(std::string str, std::string delim)
// {
//     std::vector<std::string> tokens;
//     std::string token;

//     str.resize(str.length() - 3);
//     while (str.find(delim) != str.npos)
//     {
//         // std::cout << "begin loop str = " << str << "\n";
//         token = str.substr(0, str.find(delim));
//         if (*token.begin() == ':')
//             token.erase(0, 1);
//         if (*token.end() == '\n')
//         {
//             // std::cout << "one token backslash" << std::endl;
//             token.resize(str.length() - 1);
//         }
//         // std::cout << "pushing token = '" << token << "'" << "\n";
//         tokens.push_back(token);
//         str.erase(0, str.find(delim) + delim.length());
//         // std::cout << "str after erase = '" << str << "'" << "\n";
//     }
//     if (*str.begin() == ':')
//         str.erase(0, 1);
//     // std::cout << "token = '" << str << "'" << "\n";
//     tokens.push_back(str);
//     // std::vector<std::string>::iterator it = tokens.begin();
//     return tokens;
// }