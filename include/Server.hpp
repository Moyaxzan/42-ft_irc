#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <exception>
#include <sys/types.h>
#include <sys/select.h>
#include <map>
#include <set>
#include "Client.hpp"
#include "../include/argparsing.hpp"

#define SERV_IP "127.0.0.1"
#define SERV_NAME ":localhost "

#define CAPRESP "CAP * LS : \r\nCAP END"
#define ERR_ALREADYREGISTRED "462 * :You may not reregister"
#define ERR_NEEDMOREPARAMS "461 "
//#define NEEDMOREPARAMS " :Not enough parameters"
// #define ERR_NEEDMOREPARAMS "461 * :Not enough parameters"

#define CORRECTPASS "NOTICE AUTH :Password accepted"
#define ERR_WRONGPASS "NOTICE AUTH :Invalid password"
#define ERR_NOTREGISTEREDPASS  "451 * :Please authenticate first with PASS command"
#define ERR_NOTREGISTEREDNICK  "451 * :Please authenticate first with NICK command"
#define ERR_NONICKNAMEGIVEN "431 * :No nickname given"
#define ERR_NICKNAMEINUSE "433 * "
#define NICKINUSE ":Nickname is already in use"
#define ERR_ERRONEUSNICKNAME "432 * "
#define ERRONEUS ":Erroneous nickname"
#define NICKSET "NOTICE AUTH :Nickname set to "

#define USERSET "NOTICE AUTH :Username set to "
#define ERR_ERRONEUSUSERNAME "461 "
#define ERRONEUSUSERNAME "USER :Invalid username"


class Server {
	private:
		fd_set					all_sockets_;
		int						serv_socket_;
		int						fd_max_;
		sockaddr_in				socket_infos_;
		std::string				password_;
		std::string				creatTime_;
		std::set<std::string>	nicknames_;
		//std::set<std::string>	usernames_;
		std::map<int, Client>	clients_;

				//client management
		void	newClient_(void);
		void	readClient(int fd);
		void	disconnectClient(int fd);

        //auth
		void	ignoreCAP(int fd);
		bool	handleCommand(int fd, std::string msg);
		bool	checkPassword(int fd, std::string line);
		bool	isValidNickname(int fd, std::string nickname);
		bool	handleNick(int fd, std::string line);
		bool	allUserElements(int fd, std::string line, std::string &username);
		bool	isValidUsername(int fd, std::string line, std::string &username);
		bool	handleUser(int fd, std::string line);

				//welcome message
		void	setCreatTime_(void);
		void	sendWelcomeMessage_(int fd);

	public:
		Server(void);
		Server(t_args& args);
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server(void);

		void runServer(void);

};

class SocketError : public std::exception {
	public:
		const char *what(void) const throw();
};

class BindError : public std::exception {
	public:
		const char *what(void) const throw();
};

class ListenError : public std::exception {
	public:
		const char *what(void) const throw();
};

class AcceptError : public std::exception {
	public:
		const char *what(void) const throw();
};

class SelectError : public std::exception {
	public:
		const char *what(void) const throw();
};

#endif
