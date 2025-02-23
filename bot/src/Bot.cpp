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
        std::cout << "No profanities file found!" << "\n";
    this->initServerConnection_(port);
}

void Bot::initServerConnection_(char *port)
{
    sockaddr_in server_infos;
    server_infos.sin_family = AF_INET;
    server_infos.sin_port = htons(atoi(port));
    server_infos.sin_addr.s_addr = inet_addr(SERV_IP);

    this->socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket_ == -1)
        return ;

    if (connect(this->socket_, (sockaddr *)&server_infos, sizeof(server_infos)) == -1)
        throw ConnectionError();
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

std::vector<std::vector<std::string> >&Bot::getDicts(void)
{
    return this->profanities_;
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
    char buffer[1024];
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
        {
            intro += "and " + player_nb + " ";
        }
        else
            intro += player_nb + ", ";
    }
    intro += "have been selected !\n";
    send(this->socket_, intro.c_str(), intro.size(), 0);
    std::random_shuffle(players.begin(), players.end());
    intro.clear();
    intro = encapsulate(players[0]) + " will begin ! Would you rather PULL the trigger or ROLL the bullets ?\n";
    send(this->socket_, intro.c_str(), intro.size(), 0);
    while (recv(this->socket_, buffer, 1023, 0) != 0 || recv(this->socket_, buffer, 1023, 0) != -1)
    {
        std::string buff = buffer;
        if (buff.find("PULL") != buff.npos)
        {
            if (!gun.checkBullet())
                std::cout << "Survived\n";
            else
                std::cout << "Dead!\n";
        }
        else if (buff.find("ROLL") != buff.npos)
        {
            gun.shuffleBullets();
        }
        memset(buffer, '\0', 1023);
    }
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