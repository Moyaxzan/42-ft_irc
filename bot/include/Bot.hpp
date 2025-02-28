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
#include "../include/BadPerson.hpp"
#include "../include/Gun.hpp"

class BadPerson;

typedef struct s_msg
{
    std::string original;
    std::string username;
    std::vector<std::string> content;
} t_msg;

class Bot /* : public Client */
{
    private:
        int socket_;
        std::vector<std::vector<std::string> >profanities_; 
        std::vector<BadPerson> bad_people;
        
        void addProfanityDict(std::string filename);
        void initServerConnection_(char *port);
        void checkAddBadPerson(std::string username);
        void launchRoulette(std::string const & username);
        void printBear(void);
        void fileError(void);
        bool checkBadContent(std::vector<std::string> & content, std::string const & bad_word);
        
    public:
        Bot(char *port);
        Bot(Bot const & other);
        ~Bot(void);
        Bot & operator=(Bot const & other);
        
        std::vector<std::vector<std::string> >&getDicts(void);
        int  getClientSocket(void);
        
        t_msg parseMsg(std::string msg);
        void sendMsg(std::string const & to_send, int time);
        std::string recvMsg(void);
        
        void monitor(t_msg & msg);
        void checkRoulette(t_msg & msg);
};

std::string encapsulate(int i);
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