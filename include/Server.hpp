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

extern volatile sig_atomic_t g_stopSig;

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
		std::map<int, Client *>	clients_;

		//client management
		void	newClient_(void);
		void	readClient(int fd);
        //auth
		void	ignoreCAP(int fd);
		bool	handleCommand(int fd, std::string msg);
		//welcome message
		void	setCreatTime_(void);
		void	sendWelcomeMessage_(int fd);

	public:
		Server(void);
		Server(t_args& args);
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server(void);

		//getters
		const std::string&				getPassword(void) const;
		const std::set<std::string>&	getNicknames(void) const;
		//setters
		void							addNickname(std::string nickname);
		//member functions

		void							runServer(void);
		void							disconnectClient(int fd);

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
