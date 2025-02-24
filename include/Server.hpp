#ifndef SERVER_HPP

#define SERVER_HPP
#define SERV_IP "127.0.0.1"

#define SERV_NAME "localhost"

#include <netinet/in.h>
#include <exception>
#include <sys/types.h>
#include <sys/select.h>
#include <map>
#include <set>
#include <sstream>
#include "Client.hpp"
#include "../include/argparsing.hpp"

class Server {
	private:
		fd_set					all_sockets_;
		int						serv_socket_;
		int						fd_max_;
		sockaddr_in				socket_infos_;
		std::string				password_;
		std::map<int, int>		authClients_;
		std::set<std::string>	nicknames_;
		//std::set<std::string>	usernames_;
		std::map<int, Client>	clients_;
/*
authClients_ : différentes valeurs d'état :
0 = non authentifié (pas de PASS ou mot de passe incorrect)
1 = mot de passe accepté, mais pas encore de NICK
2 = NICK reçu, mais USER manquant
3 = totalement authentifié
Tant que authClients_[fd] < 3, le client ne peut pas interagir avec le serveur
*/

		void newClient_(void);
		void readClient(int fd);
		void disconnectClient(int fd);
		void ignoreCAP(int fd);
		void authenticate(int fd, std::string msg);
		void checkPassword(int fd, std::string line);
		void handleNick(int fd, std::string line);

	public:
		Server(void);
		Server(t_args& args);
		Server(Server& other);
		Server& operator=(Server& other);
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
