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
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../include/Gun.hpp"

class Bot /* : public Client */
{
    private:
        void fileError(void);
        void addProfanityDict(std::string filename);
        int socket_;
        std::vector<std::vector<std::string> >profanities_;

        void initServerConnection_(char *port);

    public:
        Bot(char *port);
        Bot(Bot const & other);
        ~Bot(void);
        Bot & operator=(Bot const & other);

        std::vector<std::vector<std::string> >&getDicts(void);

        int  getClientSocket(void);
        bool isStrPbmatic(std::string str);
        void launchRoulette(void);
};

std::string encapsulate(int i);

class ConnectionError : public std::exception
{
    public:
        const char *what(void) const throw();
};



#endif