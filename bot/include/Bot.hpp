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

extern std::string channel;

typedef struct s_msg
{
    std::string original;
    std::string username;
    std::vector<std::string> content;
} t_msg;

class Bot
{
	private:
		int 									socket_;
		std::vector<BadPerson>					badPeople_;
		std::vector<std::vector<std::string> >	profanities_; 
		
		void                                    addProfanityDict(std::string filename);
		void                                    fileError(void);
		void                                    initServerConnection_(char *port, char *pwd);
		void                                    checkAddBadPerson(std::string username);
		bool                                    checkBadContent(std::vector<std::string> & content, std::string const & bad_word);
		bool                                    checkIfOper(void);
		
		std::vector<std::string>                getPlayersVec(void);
		void                                    launchRoulette(t_msg const & msg);
		void                                    sendIntroRoulette(std::string username);
		void                                    rouletteLoop(std::vector<std::string> & players, Gun & gun);

	public:
		Bot(char *port, char *pwd);
		Bot(Bot const & other);
		~Bot(void);
		Bot & operator=(Bot const & other);
		
		int										getClientSocket(void);
		std::vector<std::vector<std::string> >	&getDicts(void);
		
		int			                            handlePart(void);
		t_msg 		                            parseMsg(std::string msg);
		void		                            sendMsg(std::string const & to_send, int time, bool server);
		std::string	                            recvMsg(void);
		
		void	                                monitor(t_msg & msg);
		void	                                checkRoulette(t_msg & msg);
};

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

class WrongPassword : public std::exception
{
    public:
        const char *what(void) const throw();
};

#define PART_MSG(channel) ("PART " + channel)

#endif
