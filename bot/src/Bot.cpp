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

Bot::Bot(Bot const & other)
{
    *this = other;
}

Bot::~Bot(void)
{
    std::cout << "Bot destructor called\n";
}

Bot &Bot::operator=(Bot const & other)
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
}

std::vector<std::vector<std::string> >&Bot::getDicts(void)
{
    return this->profanities_;
}

void Bot::sendMsg(std::string const & to_send)
{
    int bytes_sent = 0;
    int str_len = to_send.length();
    const char *str = to_send.c_str();
    int send_res;

    while (bytes_sent < str_len)
    {
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
            std::cout << "problem\n";
        full_msg += std::string(buffer);
    }
    return (full_msg);
}

void Bot::addProfanityDict(std::string filename)
{
    if (filename.empty() || filename.compare(".") == 0 || filename.compare("..") == 0)
        return ;
    
    std::ifstream infile;
    std::string line;
    std::vector<std::string> dict;

    infile.open(("./profanities/" + filename).c_str(), std::ios::in);
    if (!infile.is_open())
    {
        this->fileError();
        return ;
    }
    while (std::getline(infile, line, '\n'))
        dict.push_back(line);
    this->profanities_.push_back(dict);
}

bool Bot::isStrPbmatic(std::string str)
{
    std::vector<std::string>::iterator it;
    std::vector<std::vector<std::string> >::iterator it2;
    std::vector<std::vector<std::string> >&all_dicts = this->getDicts();

    for (it2 = all_dicts.begin(); it2 != all_dicts.end(); it2++)
    {
        for (it = it2->begin(); it != it2->end(); it++)
        {
            if (str.find(*it) != str.npos)
            {
                std::cout << "Profanity found" << "\n";
                return true;
            }
        }
    }
    return false;
}

int Bot::getClientSocket(void)
{
    return this->socket_;
}

void Bot::launchRoulette(void)
{
    Gun gun;
    std::vector<int> players;
    std::vector<int>::iterator it;
    std::string msg;
    // std::string req;

    //requete au serveur pour pick 6 joueurs random (max) dans le channel
    // req = "1 3";
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
    this->sendMsg(intro);
    std::random_shuffle(players.begin(), players.end());
    intro.clear();
    intro = encapsulate(players[0]) + " will begin ! Would you rather PULL the trigger or ROLL the bullets ?\n";
    this->sendMsg(intro);
    while (!(msg = this->recvMsg()).empty())
    {
        if (msg.find("PULL") != msg.npos)
        {
            if (!gun.checkBullet())
                this->sendMsg("Survived\n");
            else
            {
                this->sendMsg("Dead!\n");
                break ;
            }
        }
        else if (msg.find("ROLL") != msg.npos)
        {
            gun.shuffleBullets();
            this->sendMsg("Bullets have been rolled !\n");
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