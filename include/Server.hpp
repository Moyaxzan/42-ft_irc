#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <exception>
#include <sys/types.h>
#include <sys/select.h>
#include <signal.h>
#include <cerrno>
#include <map>
#include <set>
#include "Client.hpp"
#include "argparsing.hpp"
#include "Channel.hpp"

#define SERV_IP "127.0.0.1"

// extern volatile __sig_atomic_t g_stopSig;
extern volatile sig_atomic_t g_stopSig;

class Server {
	private:
		fd_set						all_sockets_;
        fd_set                      write_fds;
		int							serv_socket_;
		int							fd_max_;
		sockaddr_in					socket_infos_;
		std::string					password_;
		std::string					creatTime_;
		std::set<std::string>		nicknames_;
		//std::set<std::string>		usernames_;
		std::map<std::string, int>	nickFd_;
		std::map<int, Client *>		clients_;
		std::vector<Channel *>		channels_;

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
		const std::string&					getPassword(void) const;
		const std::set<std::string>&		getNicknames(void) const;
		const std::map<std::string, int>&	getNickFd(void) const;
		std::map<int, Client *>		        getClients(void);
		const std::vector<Channel *>		getChannels(void) const;
		Channel*							getChannelByName(const std::string &name);
		Client*								getClientByNick(const std::string &name);
        Client*                             getClientById(int client_id);
		Channel*							getChannelById(unsigned int id);
        fd_set                              *getWriteFds(void);
		//setters
		void							    addNickname(std::string nickname, int fd);
		bool							    addChannel(std::string channelName, Client *creator, std::string passwd);
		//member functions
		void							    runServer(void);
		void							    checkChannelsPromoteOP(Client *client);
		void							    disconnectClient(int fd);
		void							    log(const std::string& level, const std::string& category, const std::string message);
		void								deleteChan(Channel* channel);
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
