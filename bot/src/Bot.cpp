#include "../include/Bot.hpp"

Bot::Bot(void)
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

void Bot::launchRoulette(void)
{
    Gun gun;
    std::vector<int> players;
    std::vector<int>::iterator it;
    // std::string req;

    //requete au serveur pour pick 6 joueurs random (max) dans le channel
    // req = "1 3";
    players.push_back(1);
    players.push_back(2);
    std::cout << "Players ";
    for (it = players.begin(); it != players.end(); it++)
    {
        std::stringstream player_nb;
        player_nb << "[" << *it << "]";
        if (it == players.end() - 1)
            std::cout << "and " << player_nb.str() << " ";
        else
            std::cout << player_nb.str() << ", ";
    }
    std::cout << "have been selected !\n";
}

void Bot::fileError(void)
{
    std::cerr << "There was a problem opening a profanities file : " << std::strerror(errno) << "\n";
}
