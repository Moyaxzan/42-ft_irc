#include "../include/Server.hpp"
#include "../include/debug.hpp"
#include "../include/Command.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctime>
#include <sstream>
#include <vector>
#include <list>
#include <stdlib.h>

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

	this->serv_socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->serv_socket_ == -1) {
		throw(SocketError());
	}

	// get creation time and date 
	setCreatTime_();
	this->log("INFO", "STARTUP", "server has started !");

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

Channel *Server::getChannelByName(const std::string &name) {
	std::vector<Channel *>::iterator it;
	for (it = this->channels_.begin(); it != this->channels_.end(); it++) {
		if ((*it)->getName() == name) {
			return *it;
		}
	}
	return (NULL);
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
	this->log("INFO", "AUTH", "new connection on " SERV_IP);
}

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

bool	Server::handleCommand(int fd, std::string cmd) {
	DEBUG_LOG("Handling command: " + cmd);
	if (cmd.find("CAP LS") == 0) {
		Command::cap(this, this->clients_[fd], cmd);
	} else if (cmd.find("PASS ") == 0) {
		if (!Command::pass(this->clients_[fd], this, cmd))
			return (false); // blocking command
	} else if (cmd.find("NICK ") == 0) {
		if (!Command::nick(this->clients_[fd], this, cmd))
			return (false);
	} else if (cmd.find("USER ") == 0) {
		if (!Command::user(this, this->clients_[fd], cmd))
			return (false);
		if (!this->clients_[fd]->isWelcomeSent() && this->clients_[fd]->isAuth()) {
			this->sendWelcomeMessage_(fd);
			this->log("INFO", "AUTH", this->clients_[fd]->getNick() + " successfully authentified");
		}
	} else if (cmd.find("PING ") == 0) {
		return (Command::ping(this, this->clients_[fd], cmd));
	} else if (cmd.find("MODE ") == 0) {
		return (Command::mode(this, this->clients_[fd], cmd));
	} else if (cmd.find("PRIVMSG ") == 0) {
		return (Command::privMsg(this->clients_[fd], this, cmd));
	} else if (cmd.find("JOIN ") == 0) {
		return (Command::join(this->clients_[fd], this, cmd));
	} else if (cmd.find("TOPIC ") == 0) {
		return (Command::topic(this->clients_[fd], this, cmd));
	}
    /* else if (cmd.find("INVITE ") == 0) {
		return (Command::invite(this->clients_[fd], this, cmd));
	} else if (cmd.find("KICK ") == 0) {
		return (Command::kick(this->clients_[fd], this, cmd));
    } else if (cmd.find("PART ") == 0) {
		return (Command::part(this->clients_[fd], this, cmd));
	} */
	return (true);
}

// Verifier avec structure sever qu'on supprime bien tout
// Distinguer dans cette fonction une déconnexion voulue d'une erreur pour transférer un éventuel
// message aux autres clients en cas de déconnexion volontaire avec un int pr le type de déconnexion
// et une string pr le message à transférer
void	Server::disconnectClient(int fd) {
	DEBUG_LOG("Into disconnectClient");
	Client					*client = this->clients_[fd];
	std::list<unsigned int>	chans = client->getJoinedChannels();

	for (std::list<unsigned int>::iterator it = chans.begin(); it != chans.end(); it++) {
		Channel* channel = this->channels_[*it];
		if (!channel->disconnectClient(this, client, "")) {
			this->log("INFO", "CHANNEL", "channel " BLUE + channel->getName() + RESET " destroyed");
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
	if (nickname.empty()) {
		std::ostringstream oss;
		oss << fd;
		this->log("WARN", "DISCONNECT", "client [" + oss.str() + "] disconnected from server");
	} else {
		this->log("WARN", "DISCONNECT", nickname + " disconnected from server");
	}
}

// Gérer autres commandes
void Server::readClient(int fd) {
	char		buffer[1024] = {'\0'};
	int			recv_res = recv(fd, buffer, 1023, 0);
 
 	// cas d'une fermeture propre du client mais on doit aussi gérer QUIT ect (cf réponse chat gpt)
	if (recv_res <= 0) {
		disconnectClient(fd);
		return ;
	}
	std::string	msg(buffer);
	if (msg.length() == 0)
		return ;
	std::vector<std::string> lines = splitLines(msg);
	if (this->clients_[fd]->isAuth()) {
		DEBUG_LOG(this->clients_[fd]->getNick() + ": " + msg);
	} else {
		std::ostringstream	oss;
		oss << fd;
		DEBUG_LOG("[" + oss.str() + "]: " + msg);
	}
	for (size_t i = 0; i < lines.size(); i++) {
		if (!handleCommand(fd, lines[i]))
			break;
	}
}

// IMposer des noms de channels commençant par "#"
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
	std::cout << std::endl;
	this->log("WARN", "EXIT", "server interrupted by SIGINT signal");
	return ;
}

void	Server::sendWelcomeMessage_(int fd) {
	Client*		client = this->clients_[fd];
    std::string	nick = client->getNick();

    client->sendMessage(this, std::string(SERV_NAME) + " 001 " + nick + " :🤠 Howdy, partner! Welcome to the Wild West of IRC, where only the fastest typers survive!");
    client->sendMessage(this, std::string(SERV_NAME) + " 002 " + nick + " :Your host is " + SERV_NAME + ", runnin’ on version 1.0, straight outta the frontier.");
    client->sendMessage(this, std::string(SERV_NAME) + " 003 " + nick + " :This here server was founded on a bright morning in the Wild West, back in " + this->creatTime_ + ".");
    client->sendMessage(this, std::string(SERV_NAME) + " 004 " + nick + " " + SERV_NAME + " 1.0 Sheriff Deputy");
	client->sendMessage(this, std::string(SERV_NAME) + " 005 " + nick + CACTUS);
     
	client->setWelcomeSent(true);
    // Message of the Day (MOTD) ?
    // client->sendMessage(this, std::string(":") + SERV_NAME + " 375 " + nick + " :- Welcome to DustySaloon, the roughest and toughest IRC town in the West!");
    // client->sendMessage(this, std::string(":") + SERV_NAME + " 372 " + nick + " :- Grab your hat, watch out for bandits, and don’t go startin’ duels unless you’re quick on the draw!");
    // client->sendMessage(this, std::string(":") + SERV_NAME + " 372 " + nick + " :- Type /help if you need guidance from the Sheriff.");
    // client->sendMessage(this, std::string(":") + SERV_NAME + " 376 " + nick + " :- Saddle up and enjoy yer stay, partner! 🤠🌵🔥");
}

void	Server::log(const std::string& level, const std::string& category, const std::string message) {
	std::string color;
	if (level == "INFO") {
		color = GREEN;
	} else if (level == "WARN") {
		color = YELLOW;
	} else if (level == "ERROR") {
		color = RED;
	} else if (level == "MSG") {
		color = BLUE;
	}

	time_t now = time(NULL);  // Get current time
    struct tm *timeinfo = localtime(&now);  // Convert to local time

    char timestamp[80];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);  // Format time

	std::cout << "[" << timestamp << "]" << color << " [" << level << "] [" << category << "] " << RESET << message << std::endl;
}
/*
✔ Si le client se déconnecte volontairement (QUIT), il n'est pas nécessaire de lui envoyer un message, mais il faut notifier les autres clients.
dans disconnectClient : + relayer un message du client qui s'est déconnecté aux autres en meme tps que la notification de déconnexion ?
std::string quitMsg = ":" + this->clients_[fd].getNick() + " QUIT :Client exited\r\n";
broadcastMessage(quitMsg, fd); // Fonction qui envoie un message à tous les autres clients

✔ Si la déconnexion est forcée par le serveur, il est préférable d’envoyer un message ERROR. (créer une fonction pour y faire appel)
✔ Utilise broadcastMessage() pour prévenir les autres utilisateurs d’un départ.
*/
