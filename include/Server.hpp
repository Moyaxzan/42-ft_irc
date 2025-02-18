#ifndef SERVER_HPP

#define SERVER_HPP
#define SERV_IP "127.0.0.1"

#include <netinet/in.h>
#include <exception>
#include <sys/types.h>
#include <sys/select.h>
#include "../include/argparsing.hpp"

class Server {
	private:
		fd_set all_sockets_;
		int serv_socket_;
		int	fd_max_;	// useful ?
		sockaddr_in socket_infos_;

		void newClient_(void);
		void readClient(int fd);

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
