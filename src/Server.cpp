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
    int bind_res = bind(this->serv_socket_, (const sockaddr *)&this->socket_infos_, size);
	if (bind_res == -1)
		throw(BindError());
    std::cout << "ip: " << this->socket_infos_.sin_addr.s_addr << "   ";
	std::cout << "this->serv_socket_: " << this->serv_socket_ << "  " << std::endl;

	//sets socket as passive listener waiting for incoming connections 
    if (listen(this->serv_socket_, 20) == -1)
		throw(ListenError());

	FD_SET(this->serv_socket_, &this->all_sockets_);
	this->fd_max_ = this->serv_socket_;
}

Server::Server(Server& other) {
	*this = other;
}

Server& Server::operator=(Server& other) {
	this->all_sockets_ = other.all_sockets_;
	this->serv_socket_ = other.serv_socket_;
	this->socket_infos_ = other.socket_infos_;
	this->fd_max_ = other.fd_max_;
	return (*this);
}

Server::~Server(void) {
// close stuff
}

// see https://www.codequoi.com/programmation-reseau-via-socket-en-c/#c%C3%B4t%C3%A9-serveur--accepter-des-connexions-client-via-socket
// for guideline
void Server::runServer(void)
{
	fd_set readfds;
	while (!stopSig) {
		readfds = this->all_sockets_;
		//select catches signal...
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
	std::cout << "signal recieved" << std::endl;
	return ;
}


void Server::newClient_(void) {
	int accept_fd = accept(this->serv_socket_, NULL, NULL);
	if (accept_fd == -1)
		throw(AcceptError());
	FD_SET(accept_fd, &this->all_sockets_);
	if (accept_fd > this->fd_max_)
		this->fd_max_ = accept_fd;
	std::cout << "accept res = " << accept_fd << std::endl;
}

void Server::readClient(int fd) {
	char buffer[1024] = {'\0'};
	int recv_res;

	recv_res = recv(fd, buffer, 1023, 0);
	if (recv_res == 0) {
		std::cout << "Client " << fd << " disconnected";
	} else {
		std::cout << "[" << fd << "] :"<< buffer << std::endl;
	}
}
