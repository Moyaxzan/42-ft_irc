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

	//bind fd and ip::port
	int	opt = 1;
	setsockopt(this->serv_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int bind_res = bind(this->serv_socket_, (const sockaddr *)&this->socket_infos_, size);
	if (bind_res == -1)
		throw(BindError());
    std::cout << "ip: " << this->socket_infos_.sin_addr.s_addr << "   ";
	std::cout << "this->serv_socket_: " << this->serv_socket_ << "  " << std::endl;

	//sets socket as passive listener waiting for incoming connections 
    if (listen(this->serv_socket_, 20) == -1) // pourquoi 20 ?
		throw(ListenError());

	FD_SET(this->serv_socket_, &this->all_sockets_);
	this->fd_max_ = this->serv_socket_;
	this->password_=args.password;
}

Server::Server(Server& other) {
	*this = other;
}

Server::~Server(void) {
// close stuff
}

// *************************************** OVERLOAD OPERATORS **************************************************************//

Server& Server::operator=(Server& other) {
	this->all_sockets_ = other.all_sockets_;
	this->serv_socket_ = other.serv_socket_;
	this->socket_infos_ = other.socket_infos_;
	this->fd_max_ = other.fd_max_;
	this->password_ = other.password_;
	return (*this);
}

// *************************************** GETTERS/SETTERS **************************************************************//

// *************************************** MEMBER FUNCTIONS **************************************************************//

void Server::newClient_(void) {
	int	accept_fd = accept(this->serv_socket_, NULL, NULL);
	if (accept_fd == -1)
		throw(AcceptError());
	FD_SET(accept_fd, &this->all_sockets_);
	if (accept_fd > this->fd_max_)
		this->fd_max_ = accept_fd;
	this->authClients_.insert(accept_fd, 0);
	//create a client instance in map here or after authentication ?
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

void	sendToClient(int fd, std::string message, std::string print, std::string nickname) {
	send(fd, message.c_str(), message.size(), 0);
	if (nickname.empty())
		std::cout << "Client [" << fd << "]: " << print << std::endl;
	else
		std::cout << "Client [" << nickname << "]: " << print << std::endl;
	DEBUG_LOG("Message sent by server to client: " + message);
}

void	Server::checkPassword(int fd, std::string line) {
	DEBUG_LOG("Into checkPassword function");
	std::string	clientPass = line.substr(5);
	//std::string	response;

	if (this->authClients_.count(fd) && this->authClients_[fd] != 0) {
		sendToClient(fd, ":" SERV_NAME " 462 * :You may not reregister\r\n", "already registered", "");
		return ;
	}
	if (clientPass == this->password_) {
		this->authClients_[fd] = 1;
		sendToClient(fd, ":" SERV_NAME " NOTICE AUTH :Password accepted\r\n", "correct password", "");
		//response = ":" SERV_NAME " NOTICE AUTH :Password accepted\r\n";
		//send(fd, response.c_str(), response.size(), 0);
		//std::cout << "Client " << fd << " correct password" << std::endl;
	} else {
		sendToClient(fd, ":" SERV_NAME " NOTICE AUTH :Invalid password\r\n", "wrong password", "");
		//response = ":" SERV_NAME " NOTICE AUTH :Invalid password\r\n";
		//send(fd, response.c_str(), response.size(), 0);
		//std::cout << "Client " << fd << ": wrong password" << std::endl;
		disconnectClient(fd);
	}		
}

// si aucun nick fourni ou déjà pris, 2 solutions : soit on vire le client, soit on laisse la connexion ouverte mais on bloque toute autre action
void	Server::handleNick(int fd, std::string line) {
	std::string	nickname = line.substr(5);
	std::string	response;

	if (nickname.empty()) {
		response = ":" SERV_NAME " 431 * :No nickname given\r\n";
		send(fd, response.c_str(), response.size(), 0);
		std::cout << "Client " << fd << ": no nickname given" << std::endl;
		//disconnectClient(fd);
		return ;
	}
	if (this->nicknames_.count(nickname)) { // Vérifie que le pseudo n'est pas déjà pris, standard IRC n'autorise pas deux utilisateurs avec le même NICK, peu importe leur USER
		response = ":" SERV_NAME " 433 * " + nickname + " :Nickname is already in use\r\n";
		send(fd, response.c_str(), response.size(), 0);
		return ;
	}
	this->nicknames_.insert(nickname); // to lock the nickname
	// set the client nickname in its instance
	if (this->authClients_[fd] == 1) // to preserve already connected clients' status
		this->authClients_[fd] = 2;
	std::string response = ":" SERV_NAME " NOTICE AUTH :Nickname set to " + nickname + "\r\n";
	send(fd, response.c_str(), response.size(), 0);
	std::cout << "Client " << fd << " nickname set successfully to : " << nickname << std::endl;
}

// Ajouter USER et autres commandes
// Check if (authCliens_ < 3) ? 
// bloquer en cas d'authentification déjà valide
void	Server::authenticate(int fd, std::string msg) {
	std::vector<std::string> lines = splitLines(msg);
	DEBUG_LOG("Into authenticate function");

	for (size_t i = 0; i < lines.size(); i++) {
		DEBUG_LOG("Into loop, line: " + lines[i]);
		if (lines[i].find("PASS ") == 0)
			checkPassword(fd, lines[i]);
		if (lines[i].find("NICK ") == 0)
			handleNick(fd, lines[i]);
	}
	// addCLient() function ? (mettre une condition dedans où je vérifié si ce n'est pas déjà fait puisque la fonction peut-etre appelé pour un client déjà connecté)
}


// Verifier avec structure sever qu'on supprime bien tout
// Distinguer dans cette fonction une déconnexion voulue d'une erreur pour transférer un éventuel
// message aus autres clients en cas de déconnexion volontaire avec un int pr le type de déconnexion
// et une string pr le message à transférer
void	Server::disconnectClient(int fd) {
		std::string	nickname = this->clients_[fd].getNick();
		close (fd);
		FD_CLR(fd, &this->all_sockets_);
		if (this->authClients_.count(fd))
			this->authClients_.erase(fd);
		this->nicknames_.erase(nickname); // to free the nickname
		// destroy the client's instance ?
		//change fd_max_ if it is equal to fd and look for the new higher fd
		// signaler aux autres clients la déconnexion du client actuel (avec broadcastmessage ?)
		std::cout << "Client " << nickname << " disconnected" << std::endl;
}

void	Server::ignoreCAP(int fd) {
	sendToClient(fd, ":" SERV_NAME " CAP * LS : \r\nCAP END\r\n", "CAP response sent", "");
	//std::string capResponse = ":" SERV_NAME " CAP * LS : \r\nCAP END\r\n"; // capacities list (none here)
	//send(fd, capResponse.c_str(), capResponse.size(), 0);
	//DEBUG_LOG("CAP response sent to client : " + capResponse);
}

// Gérer autres commandes
void Server::readClient(int fd) {
	char	buffer[1024] = {'\0'};
	int		recv_res;

	recv_res = recv(fd, buffer, 1023, 0);
	if (recv_res == 0) {
		disconnectClient(fd);
		return ;
	} // cas d'une fermeture propre du client mais on doit aussi gérer QUIT (cf réponse chat gpt)
	std::string	msg(buffer);
	std::cout << "[" << fd << "] :"<< msg << std::endl;

	//Ignore "CAP LS 302"
	if (msg.find("CAP LS") == 0)
		ignoreCAP(fd);

	// Handle authentication: password, nick and user
	if (msg.find("CAP END") == 0) {
		DEBUG_LOG("Into CAP END");
		authenticate(fd, msg);
	}
	if (msg.find("PASS") == 0)
		authenticate(fd, msg);
	if (msg.find("NICK") == 0)
		authenticate(fd, msg);
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

/*
✔ Si le client se déconnecte volontairement (QUIT), il n'est pas nécessaire de lui envoyer un message, mais il faut notifier les autres clients.
dans disconnectClient : + relayer un message du client qui s'est déconnecté aux autres en meme tps que la notification de déconnexion ?
std::string quitMsg = ":" + this->clients_[fd].getNick() + " QUIT :Client exited\r\n";
broadcastMessage(quitMsg, fd); // Fonction qui envoie un message à tous les autres clients

✔ Si la déconnexion est forcée par le serveur, il est préférable d’envoyer un message ERROR. (créer une fonction pour y faire appel)
✔ Utilise broadcastMessage() pour prévenir les autres utilisateurs d’un départ.
*/
