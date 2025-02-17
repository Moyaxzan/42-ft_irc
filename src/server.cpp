/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 16:44:01 by kipouliq          #+#    #+#             */
/*   Updated: 2025/02/17 12:25:29 by tsaint-p         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"
#include <arpa/inet.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>


// see https://www.codequoi.com/programmation-reseau-via-socket-en-c/#c%C3%B4t%C3%A9-serveur--accepter-des-connexions-client-via-socket
// for guideline
void start_server(t_args &args)
{
    sockaddr_in socket_infos;
    socklen_t size;

	//structure for bind
    socket_infos.sin_family = AF_INET; // to use IPV4
    socket_infos.sin_port = htons(args.port);
    socket_infos.sin_addr.s_addr = inet_addr("127.0.0.1");
    size = sizeof(socket_infos);

    std::cout << "Starting server!" << std::endl;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		throw(SocketError());
	}

	//bind fd and ip::port
    int bind_res = bind(socket_fd, (const sockaddr *)&socket_infos, size);
	if (bind_res == -1)
		throw(BindError());
    std::cout << socket_infos.sin_addr.s_addr << "   " << socket_fd << "  " << bind_res << std::endl;

	//sets socket as passive listener waiting for incoming connections 
    if (listen(socket_fd, 20) == -1)
		throw(ListenError());

	//creates new fd to communicate with client
    int accept_fd = accept(socket_fd, (sockaddr *)&socket_infos, &size);
	if (accept_fd == -1)
		throw(AcceptError());
    std::cout << "accept res = " << accept_fd << std::endl;

	// loop to recieve data sent by client
    char buffer[1024];
	std::memset(buffer, 0, 1024);
    while (recv(accept_fd, buffer, 1023, 0) != 0)
    {
        std::cout << buffer << std::endl;
		std::memset(buffer, 0, 1024);
    }
	return ;
}
