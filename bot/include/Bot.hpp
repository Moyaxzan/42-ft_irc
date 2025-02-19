#ifndef BOT_HPP
#define BOT_HPP
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <dirent.h>
#include <unistd.h>
#include <sys/socket.h>
#include "../../include/Server.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>

class Bot // : public Client ?
{
    private:
        void fileError(void);
        void addProfanityDict(std::string filename);
        std::vector<std::vector<std::string> >profanities_;

    public:
        Bot(void);
        Bot(Bot const & other);
        ~Bot(void);
        Bot & operator=(Bot const & other);

        std::vector<std::vector<std::string> >&getDicts(void);

        bool isStrPbmatic(std::string str);
};

#endif