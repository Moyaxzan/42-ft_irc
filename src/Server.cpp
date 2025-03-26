#include "../include/Server.hpp"
#include "../include/debug.hpp"
#include "../include/Command.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctime>
#include <sstream>
#include <vector>
#include <list>

// *************************************** CONSTRUCTORS/DESTRUCTORS **************************************************************//

Server::Server(void) {
}

Server::Server(t_args& args) {
	socklen_t size;
	FD_ZERO(&this->all_sockets_);

	//structure for bind
    this->socket_infos_.sin_family = AF_INET; // to use IPV4
    this->socket_infos_.sin_port = htons(args.port);
    this->socket_infos_.sin_addr.s_addr = inet_addr(SERV_IP);
    size = sizeof(this->socket_infos_);

    DEBUG_LOG("Starting server!");
	this->serv_socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->serv_socket_ == -1) {
		throw(SocketError());
	}

	// get creation time and date 
	setCreatTime_();
	DEBUG_LOG("startup time: " + this->creatTime_);

	//bind fd and ip::port
	int	opt = 1;
	setsockopt(this->serv_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int bind_res = bind(this->serv_socket_, (const sockaddr *)&this->socket_infos_, size);
	if (bind_res == -1)
		throw(BindError());

	//sets socket as passive listener waiting for incoming connections 
    if (listen(this->serv_socket_, 20) == -1) // pourquoi 20 ?
		throw(ListenError());

	FD_SET(this->serv_socket_, &this->all_sockets_);
	this->fd_max_ = this->serv_socket_;
	this->password_=args.password;
}

Server::Server(const Server& other) {
	*this = other;
}

Server::~Server(void) {
// close stuff
  for (int i = 0; i <= this->fd_max_; i++) {
		if (FD_ISSET(i, &this->all_sockets_)) {
			close(i);
		}
	}
	for (std::map<int, Client *>::iterator it = this->clients_.begin(); it != this->clients_.end(); it++) {
		delete it->second;
	}
	for (std::vector<Channel *>::iterator it = this->channels_.begin(); it != this->channels_.end(); it++) {
		delete *it;
	}
}

// *************************************** OVERLOAD OPERATORS **************************************************************//

Server& Server::operator=(const Server& other) {
	this->all_sockets_ = other.all_sockets_;
	this->serv_socket_ = other.serv_socket_;
	this->socket_infos_ = other.socket_infos_;
	this->fd_max_ = other.fd_max_;
	this->password_ = other.password_;
	return (*this);
}

// *************************************** GETTERS/SETTERS **************************************************************//

const std::string&	Server::getPassword(void) const {
	return (this->password_);
}

const std::set<std::string>&	Server::getNicknames(void) const {
	return (this->nicknames_);
}

const std::vector<Channel *>	Server::getChannels(void) const {
	return (this->channels_);
}

Channel	*Server::getChannelByName(const std::string &name) {
	std::vector<Channel *>::iterator it;
	for (it = this->channels_.begin(); it != this->channels_.end(); it++) {
		if ((*it)->getName() == name) {
			return (*it);
		}
	}
	return (NULL);
}

Client	*Server::getClientByNick(const std::string &name) {
	std::map<int, Client *>::iterator it;
	for (it = this->clients_.begin(); it != this->clients_.end(); it++) {
		if (it->second->getNick() == name) {
			return (it->second);
		}
	}
	return (NULL);
}

Channel *Server::getChannelById(unsigned int channel_id)
{
	std::vector<Channel *>::iterator it;
	for (it = this->channels_.begin(); it != this->channels_.end(); it++) {
		if ((*it)->getId() == channel_id) {
			return *it;
		}
	}
	return NULL;
}

const std::map<std::string, int>&	Server::getNickFd(void) const {
	return (this->nickFd_);
}

const std::map<int, Client *>&		Server::getClients(void) const {
	return (this->clients_);
}

void	Server::addNickname(std::string nickname, int fd) {
	this->nicknames_.insert(nickname);
	this->nickFd_.insert(std::make_pair(nickname, fd));
}

bool	Server::addChannel(std::string channelName, Client *creator, std::string passwd) {
	int	id = 0;
	std::vector<Channel *>::iterator it;

	if (this->getChannelByName(channelName) != NULL) {
		return (false);
	}
	for (it = this->channels_.begin(); it != this->channels_.end(); it++) {
		id++;
	}
	Channel *newChan = new Channel(id, channelName);
	if (!newChan) {
		return (false);
	}
	//add creator to the members & operators
	newChan->addOperator(creator);
	newChan->addMember(creator);
	newChan->setPassword(passwd);
	this->channels_.push_back(newChan);
	std::vector<Client *> ops = newChan->getOperators();
	return (true);
}

void	Server::setCreatTime_(void) {
	time_t now = time(NULL);  // Get current time
    struct tm *timeinfo = localtime(&now);  // Convert to local time

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);  // Format time

	this->creatTime_ = buffer;
}

// *************************************** MEMBER FUNCTIONS **************************************************************//

void Server::newClient_(void) {
	int	accept_fd = accept(this->serv_socket_, NULL, NULL);
	if (accept_fd == -1)
		throw(AcceptError());
	FD_SET(accept_fd, &this->all_sockets_);
	if (accept_fd > this->fd_max_)
		this->fd_max_ = accept_fd;
	this->clients_.insert(std::make_pair(accept_fd, new Client(accept_fd)));
	std::cout << "New client detected with fd: " << accept_fd << std::endl;
}

/*
std::vector<std::string>	splitLines(std::string msg) {
	std::vector<std::string>	lines;
	std::stringstream			ss(msg);
	std::string					line;

	while (std::getline(ss, line, '\n')) {
		if (!line.empty() && line[line.size() - 1] == '\r') // To erase '\r' if it is present
			line.erase(line.size() - 1);
		lines.push_back(line);
	}
	return (lines);
}
*/

bool	Server::handleCommand(int fd, std::string cmd) {
	DEBUG_LOG("Into handleCommand function");
	DEBUG_LOG("Command = " + cmd);
	
	if (cmd.find("CAP LS") == 0) {
		Command::cap(this->clients_[fd], cmd);
	} else if (cmd.find("PASS ") == 0) {
		if (!Command::pass(this->clients_[fd], this, cmd))
			return (false); // blocking command
	} else if (cmd.find("NICK ") == 0) {
		if (!Command::nick(this->clients_[fd], this, cmd))
			return (false);
	} else if (cmd.find("USER ") == 0) {
		if (!Command::user(this->clients_[fd], cmd))
			return (false);
		if (!this->clients_[fd]->isWelcomeSent() && this->clients_[fd]->isAuth())
			this->sendWelcomeMessage_(fd);
	} else if (cmd.find("PING ") == 0) {
		return (Command::ping(this->clients_[fd], cmd));
	} else if (cmd.find("MODE ") == 0) {
		return (Command::mode(this, this->clients_[fd], cmd));
	} else if (cmd.find("PRIVMSG ") == 0) {
		return (Command::privMsg(this->clients_[fd], this, cmd));
	} else if (cmd.find("JOIN ") == 0) {
		return (Command::join(this->clients_[fd], this, cmd));
	} else if (cmd.find("TOPIC ") == 0) {
		return (Command::topic(this->clients_[fd], this, cmd));
    } else if (cmd.find("INVITE ") == 0) {
		return (Command::invite(this->clients_[fd], this, cmd));
	} else if (cmd.find("KICK ") == 0) {
		return (Command::kick(this->clients_[fd], this, cmd));
	}else if (cmd.find("MODE ") == 0) {
		return (Command::mode(this, this->clients_[fd], cmd));
	}
	return (true);
}

// Promeut un nouvel operateur en cas de deconnexion client si celui-ci etait dernier operateur client
// == check tous les channels dont fait partie le client et check s'il etait dernier op sur le channel

void static promoteNewOperator(Channel *channel, Client *lastOP)
{
	std::list<Client *> members = channel->getMembers();
	std::list<Client *>::iterator it;

	for (it = members.begin(); it != members.end(); it++)
	{
		if ((*it)->getId() != lastOP->getId())
		{
			channel->addOperator(*it);
			channel->broadcast(NULL, NOTICE_OPER((*it)->getNick(), channel->getName()));
			return ;
		}
	}
}

void Server::checkChannelsPromoteOP(Client *client)
{
	std::list<unsigned int> chans_id = client->getJoinedChannels();
	std::list<unsigned int>::iterator it;
	std::list<Channel *> joined_chans;
	std::list<Channel *>::iterator it2;
	
	for (it = chans_id.begin(); it != chans_id.end(); it++)
		joined_chans.push_back(this->getChannelById(*it));
	for (it2 = joined_chans.begin(); it2 != joined_chans.end(); it2++)
	{
		if ((*it2)->isOperator(client) && (*it2)->getOperators().size() == 1 && (*it2)->getMembers().size() > 1)
		{
			promoteNewOperator(*it2, client);
			return ;
		}
	}
}

// Verifier avec structure sever qu'on supprime bien tout
// Distinguer dans cette fonction une déconnexion voulue d'une erreur pour transférer un éventuel
// message aux autres clients en cas de déconnexion volontaire avec un int pr le type de déconnexion
// et une string pr le message à transférer
void	Server::disconnectClient(int fd) {
	DEBUG_LOG("Into disconnectClient");
	Client					*client = this->clients_[fd];
	std::list<unsigned int>	chans = client->getJoinedChannels();

	checkChannelsPromoteOP(client);
	for (std::list<unsigned int>::iterator it = chans.begin(); it != chans.end(); it++) {
		Channel* channel = this->channels_[*it];
		;
		if (!channel->disconnectClient(client, "")) {
			delete channel;
			this->channels_.erase(this->channels_.begin() + *it);
		}
	}
	std::string	nickname = client->getNick();
	FD_CLR(fd, &this->all_sockets_);
	close (fd);
	this->nicknames_.erase(nickname);	//useful ??
	delete client;						//free client
	this->clients_.erase(fd);
	//change fd_max_ if it is equal to fd and look for the new higher fd
	// supprimer le client de tous les canaux où il était présent et lui enlevers son dstatut d'opérateur
	// signaler aux autres clients présents dans les meme canaux la déconnexion du client actuel (avec broadcastmessage ?)
	if (nickname.empty())
		std::cout << "Client " << fd << " disconnected" << std::endl;
	else
		std::cout << "Client " << nickname << " disconnected" << std::endl;
}

// Gérer autres commandes
void Server::readClient(int fd) {
	char		buffer[1024] = {'\0'};
	int			recv_res = recv(fd, buffer, 1023, 0);
 
	DEBUG_LOG("Into readClient");
 	// cas d'une fermeture propre du client mais on doit aussi gérer QUIT ect (cf réponse chat gpt)
	if (recv_res <= 0) {
		DEBUG_LOG("Into previous recv_res == 0");
		disconnectClient(fd);
		return ;
	}
	//std::vector<std::string> lines = splitLines(msg);
	std::vector<std::string> lines = split(msg, '\n');
	for (std::vector<std::string>::iterator line = lines.begin(); line != lines.end(); line++) {
		if (!handleCommand(fd, *line))
			break ;
	}

}

// Imposer des noms de channels commençant par "#"
// see https://www.codequoi.com/programmation-reseau-via-socket-en-c/#c%C3%B4t%C3%A9-serveur--accepter-des-connexions-client-via-socket
// for guideline
void Server::runServer(void)
{
	fd_set readfds;
	while (!g_stopSig) {
		readfds = this->all_sockets_;
		if (select(this->fd_max_ + 1, &readfds, NULL, NULL, NULL) == -1 && !g_stopSig) {
			throw(SelectError());
		}
		for (int i = 0; !g_stopSig && i <= this->fd_max_; i++) {
			if (FD_ISSET(i, &readfds)) {
				if (i == this->serv_socket_) {
					this->newClient_();
				} else {
					this->readClient(i);
				}
			}
		}
	}
	std::cout << std::endl << "Exit" << std::endl;
	return ;
}

void	Server::sendWelcomeMessage_(int fd) {
	Client*		client = this->clients_[fd];
    std::string	nick = client->getNick();

    client->sendMessage(std::string(SERV_NAME) + " 001 " + nick + " :🤠 Howdy, partner! Welcome to the Wild West of IRC, where only the fastest typers survive!");
    client->sendMessage(std::string(SERV_NAME) + " 002 " + nick + " :Your host is " + SERV_NAME + ", runnin’ on version 1.0, straight outta the frontier.");
    client->sendMessage(std::string(SERV_NAME) + " 003 " + nick + " :This here server was founded on a bright morning in the Wild West, back in " + this->creatTime_ + ".");
    client->sendMessage(std::string(SERV_NAME) + " 004 " + nick + " " + SERV_NAME + " 1.0 Sheriff Deputy");
	client->sendMessage(std::string(SERV_NAME) + " 005 " + nick + CACTUS);
     
	client->setWelcomeSent(true);
    // Message of the Day (MOTD) ?
    // client->sendMessage(std::string(":") + SERV_NAME + " 375 " + nick + " :- Welcome to DustySaloon, the roughest and toughest IRC town in the West!");
    // client->sendMessage(std::string(":") + SERV_NAME + " 372 " + nick + " :- Grab your hat, watch out for bandits, and don’t go startin’ duels unless you’re quick on the draw!");
    // client->sendMessage(std::string(":") + SERV_NAME + " 372 " + nick + " :- Type /help if you need guidance from the Sheriff.");
    // client->sendMessage(std::string(":") + SERV_NAME + " 376 " + nick + " :- Saddle up and enjoy yer stay, partner! 🤠🌵🔥");
}

