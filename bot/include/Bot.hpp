#ifndef BOT_HPP
#define BOT_HPP
#include <iostream>
#include <cctype>
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
        std::vector<std::vector<std::string> >profanities_;
        int socket_;

        void initServerConnection_(char *port);

    public:
        Bot(char *port);
        Bot(Bot const & other);
        ~Bot(void);
        Bot & operator=(Bot const & other);

        std::vector<std::vector<std::string> >&getDicts(void);
        int  getClientSocket(void);

        void sendMsg(std::string const & to_send);
        std::string recvMsg(void);
        bool isStrPbmatic(std::string str);
        void launchRoulette(void);
};

std::string encapsulate(int i);
std::string trim(std::string str, std::string delim);
std::vector<std::string> split(std::string str, std::string delim);

class ConnectionError : public std::exception
{
    public:
        const char *what(void) const throw();
};

class SocketBotError : public std::exception
{
    public:
        const char *what(void) const throw();
};

class SendError : public std::exception
{
    public:
        const char *what(void) const throw();
};

class FileOpenError : public std::exception
{
    public:
        const char *what(void) const throw();
};

#endif