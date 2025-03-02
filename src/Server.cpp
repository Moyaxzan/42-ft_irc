#include "../include/Server.hpp"
#include "../include/debug.hpp"
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
	for (std::map<int, Client *>::iterator it = this->clients_.begin(); it != this->clients_.end(); it++) {
		delete it->second;
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


/*
envoie un message à tous les clients sauf le client emetteur
void Server::broadcastMessage(const std::string &message, int sender_fd) {
    std::map<int, Client>::iterator it;
    for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
        int client_fd = it->first;
        if (client_fd != sender_fd) { // Ne pas renvoyer le message à l'émetteur
            send(client_fd, message.c_str(), message.size(), 0);
        }
    }
}
*/

void	Server::ignoreCAP(int fd) {
	DEBUG_LOG("Into ignoreCAP");
	this->clients_[fd]->sendMessage(SERV_NAME CAPRESP);
}

bool	Server::checkPassword(int fd, std::string line) {
	DEBUG_LOG("Into checkPassword function");
	std::string	clientPass = line.substr(5);

	if (this->clients_[fd]->isPasswdSet()) {
		this->clients_[fd]->sendMessage(SERV_NAME ERR_ALREADYREGISTRED);
		return (true); // return true or false ? check with irssi
	}
	if (clientPass.empty()) {
		this->clients_[fd]->sendMessage(SERV_NAME  ERR_NEEDMOREPARAMS "* PASS :Not enough parameters");
		return (false); // to check with irssi
	}
	if (clientPass == this->password_) {
		this->clients_[fd]->setPasswdSet(true);
		this->clients_[fd]->sendMessage(SERV_NAME CORRECTPASS);
    return (true);
	} else {
		this->clients_[fd]->sendMessage(SERV_NAME ERR_WRONGPASS);
		disconnectClient(fd);
    return (false);
	}
}

bool	correctChars(std::string nickname, char caller) {
	char	c;

	if (!isalpha(nickname[0])) // nick has to begin with a letter
		return (false);
	for (unsigned int i = 1; i < nickname.size(); i++) {
		c = nickname[i];
		if (!isprint(c)) // check if chars are printable
			return (false);
		if (caller == 'n') {
			if (!(isalnum(c) || c == '-' || c == '_' || c == '\\' || c == '|' || c == '[' || c == ']'))
			return (false); // check if it is an allowed char
		}
		else {
			if (!(isalnum(c) || c == '-' || c == '_' || c == '\\' || c == '|' || c == '[' || c == ']' || c == '{'  || c == '}'))
			return (false); // check if it is an allowed char
		}
	}
	return (true);
}

bool	Server::isValidNickname(int fd, std::string nickname) {
	DEBUG_LOG("Into isValidNickname function");
	if (nickname.empty()) {
		this->clients_[fd]->sendMessage(SERV_NAME ERR_NONICKNAMEGIVEN);
		return (false);
	}
	// Ajouter vérification par le bot pour respecter la politique du serveur ?
	if (nickname.size() > 9 || !correctChars(nickname, 'n') || nickname == SERV_NAME) {
		this->clients_[fd]->sendMessage(SERV_NAME ERR_ERRONEUSNICKNAME + nickname + ERRONEUS);
		return (false);
	}
	if (this->nicknames_.count(nickname)) { // check if nickname is available
		this->clients_[fd]->sendMessage(SERV_NAME ERR_NICKNAMEINUSE + nickname + NICKINUSE);
		return (false);
	}
	return (true);
}

bool	Server::handleNick(int fd, std::string line) {
	std::string	nickname = line.substr(5);

	DEBUG_LOG("Into handleNick function");
	if (!this->clients_[fd]->isPasswdSet()) {
		this->clients_[fd]->sendMessage(SERV_NAME ERR_NOTREGISTEREDPASS);
		return (false);
	}
	if (!isValidNickname(fd, nickname))
		return (false);
	this->nicknames_.insert(nickname); // to lock the nickname
	this->clients_[fd]->setNick(nickname); // set the client nickname in its instance
	this->clients_[fd]->sendMessage(SERV_NAME NICKSET + nickname);
	return (true);
}

bool	Server::allUserElements(int fd, std::string line, std::string &username) {
	std::istringstream	iss(line);
	std::string			cmd, hostname, servername, realname;

	iss >> cmd >> username >> hostname >> servername; // extraction of words one by one
	std::getline(iss, realname);
	if (username.empty() || hostname.empty() || servername.empty() || realname.empty()) {
		std::string	nickname = this->clients_[fd]->getNick();
		this->clients_[fd]->sendMessage(SERV_NAME ERR_NEEDMOREPARAMS + nickname + " USER :Not enough parameters");
		return (false);
	}
	//if (realname[0] == ':') if parsing of realname
			//realname = realname.substr(1);
	return (true);
}

// USER <username> <hostname> <servername> :<realname>
bool	Server::isValidUsername(int fd, std::string line, std::string &username) {
	if (!allUserElements(fd, line, username)) // check if 4 params or if empty (=needmore params)
		return (false);
	// Ajouter vérification par le bot pour respecter la politique du serveur ?
	if (username.size() > 9 || !correctChars(username, 'u') || username == SERV_NAME) {
		std::string	nickname = this->clients_[fd]->getNick();
		this->clients_[fd]->sendMessage(SERV_NAME ERR_ERRONEUSUSERNAME + nickname + ERRONEUSUSERNAME);
		return (false);
	}
	//parse hostname, servername and realname ?
	return (true);
}

bool	Server::handleUser(int fd, std::string line) {
	DEBUG_LOG("Into handleUser function");
	if (!this->clients_[fd]->isNickSet()) {
		this->clients_[fd]->sendMessage(SERV_NAME ERR_NOTREGISTEREDNICK);
		return (false);
	}
	if (this->clients_[fd]->isUsernameSet()) {
		std::string nickname = this->clients_[fd]->getNick();
		this->clients_[fd]->sendMessage(SERV_NAME "462 " + nickname + " :You may not reregister");
		return (true);
	}
	std::string	username;
	if (!isValidUsername(fd, line, username))
		return (false);
	this->clients_[fd]->setUser(username); // set the client nickname in its instance
	this->clients_[fd]->sendMessage(SERV_NAME USERSET + username);
	return (true);
}


bool	Server::handleCommand(int fd, std::string cmd) {
	//std::vector<std::string> lines = splitLines(cmd);
	DEBUG_LOG("Into handleCommand function");
	DEBUG_LOG("Command = " + cmd);
	//for (size_t i = 0; i < lines.size(); i++) {
	if (cmd.find("CAP LS") == 0) //Ignore "CAP LS *"
		ignoreCAP(fd);
	else if (cmd.find("PASS ") == 0) {
		if (!checkPassword(fd, cmd))
			return (false); // blocking command
	}
	else if (cmd.find("NICK ") == 0) {
		if (!handleNick(fd, cmd))
			return (false);
	}
	else if (cmd.find("USER ") == 0) {
		if (!handleUser(fd, cmd))
			return (false);
	}
	if (!this->clients_[fd]->isWelcomeSent() && this->clients_[fd]->isAuth()) {
		this->sendWelcomeMessage_(fd);
	}
	return (true);
}

// Verifier avec structure sever qu'on supprime bien tout
// Distinguer dans cette fonction une déconnexion voulue d'une erreur pour transférer un éventuel
// message aux autres clients en cas de déconnexion volontaire avec un int pr le type de déconnexion
// et une string pr le message à transférer
void	Server::disconnectClient(int fd) {
	DEBUG_LOG("Into disconnectClient");
	std::string	nickname = this->clients_[fd]->getNick();
	close (fd);
	FD_CLR(fd, &this->all_sockets_);
	this->nicknames_.erase(nickname); // to free the nickname
	this->clients_.erase(fd); // destroys the instance Client
	//change fd_max_ if it is equal to fd and look for the new higher fd
	// signaler aux autres clients la déconnexion du client actuel (avec broadcastmessage ?)
	if (nickname.empty())
		std::cout << "Client " << fd << " disconnected" << std::endl;
	else
		std::cout << "Client " << nickname << " disconnected" << std::endl;
}

// Gérer autres commandes
// :serveur CODE <client> <paramètre> :message
// 401 ERR_NOSUCHNICK	"No such nick/channel" = tentative d'envoi d'un msg à un nick inexistant
void Server::readClient(int fd) {
	char		buffer[1024] = {'\0'};
	int			recv_res = recv(fd, buffer, 1023, 0);
	std::string	msg(buffer);
 
	DEBUG_LOG("Into readClient");
	std::cout << "[" << fd << "] : |"<< msg << "|" << std::endl;
 	// cas d'une fermeture propre du client mais on doit aussi gérer QUIT ect (cf réponse chat gpt)
	if (recv_res <= 0) {
		DEBUG_LOG("Into previous recv_res == 0");
		disconnectClient(fd);
		return ;
	}
	std::vector<std::string> lines = splitLines(msg);
	for (std::vector<std::string>::iterator line = lines.begin(); line != lines.end(); line++) {
		if (!handleCommand(fd, *line))
			break ;
	}
}

// see https://www.codequoi.com/programmation-reseau-via-socket-en-c/#c%C3%B4t%C3%A9-serveur--accepter-des-connexions-client-via-socket
// for guideline
void Server::runServer(void)
{
	fd_set readfds;
	while (1) {
		readfds = this->all_sockets_;
		if (select(this->fd_max_ + 1, &readfds, NULL, NULL, NULL) == -1) {
			throw(SelectError());
		}
		for (int i = 0; i <= this->fd_max_; i++) {
			if (FD_ISSET(i, &readfds)) {
				if (i == this->serv_socket_) {
					this->newClient_();
				} else {
					this->readClient(i);
				}
			}
		}
	}
	return ;
}

void	Server::sendWelcomeMessage_(int fd) {
	Client*		client = this->clients_[fd];
    std::string	nick = client->getNick();

    client->sendMessage(std::string(SERV_NAME) + "001 " + nick + " :🤠 Howdy, partner! Welcome to the Wild West of IRC, where only the fastest typers survive!");
    client->sendMessage(std::string(SERV_NAME) + "002 " + nick + " :Your host is " + SERV_NAME + ", runnin’ on version 1.0, straight outta the frontier.");
    client->sendMessage(std::string(SERV_NAME) + "003 " + nick + " :This here server was founded on a bright morning in the Wild West, back in " + this->creatTime_ + ".");
    client->sendMessage(std::string(SERV_NAME) + "004 " + nick + " " + SERV_NAME + " 1.0 Sheriff Deputy");
     
	client->setWelcomeSent(true);
    // Message of the Day (MOTD) ?
    // client->sendMessage(std::string(":") + SERV_NAME + " 375 " + nick + " :- Welcome to DustySaloon, the roughest and toughest IRC town in the West!");
    // client->sendMessage(std::string(":") + SERV_NAME + " 372 " + nick + " :- Grab your hat, watch out for bandits, and don’t go startin’ duels unless you’re quick on the draw!");
    // client->sendMessage(std::string(":") + SERV_NAME + " 372 " + nick + " :- Type /help if you need guidance from the Sheriff.");
    // client->sendMessage(std::string(":") + SERV_NAME + " 376 " + nick + " :- Saddle up and enjoy yer stay, partner! 🤠🌵🔥");
}
/*
✔ Si le client se déconnecte volontairement (QUIT), il n'est pas nécessaire de lui envoyer un message, mais il faut notifier les autres clients.
dans disconnectClient : + relayer un message du client qui s'est déconnecté aux autres en meme tps que la notification de déconnexion ?
std::string quitMsg = ":" + this->clients_[fd].getNick() + " QUIT :Client exited\r\n";
broadcastMessage(quitMsg, fd); // Fonction qui envoie un message à tous les autres clients

✔ Si la déconnexion est forcée par le serveur, il est préférable d’envoyer un message ERROR. (créer une fonction pour y faire appel)
✔ Utilise broadcastMessage() pour prévenir les autres utilisateurs d’un départ.
*/
