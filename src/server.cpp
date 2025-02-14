/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 16:44:01 by kipouliq          #+#    #+#             */
/*   Updated: 2025/02/14 18:37:09 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"
#include <arpa/inet.h>
#include <string>
#include <cstdlib>
#include <cstdio>

void start_server(t_args &args)
{
    sockaddr_in socket_infos;
    socklen_t size;

    socket_infos.sin_family = AF_INET;
    socket_infos.sin_port = htons(atoi((args.portStr.c_str())));
    socket_infos.sin_addr.s_addr = inet_addr("127.0.0.1");
    size = sizeof(socket_infos);
    std::cout << "Starting server!" << std::endl;
    int server_ip = inet_addr("127.0.0.1");
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0); // to protect
    int bind_res = bind(socket_fd, (const sockaddr *)&socket_infos, size);
    std::cout << server_ip << "   " << socket_fd << "  " << bind_res << std::endl;
    listen(socket_fd, 20);
    int accept_fd = accept(socket_fd, (sockaddr *)&socket_infos, &size);
    std::cout << "accept res = " << accept_fd << std::endl;
    char buffer[1024]; // bzero | memset
    while (recv(accept_fd, buffer, 1023, 0) != 0)
    {
        std::cout << buffer << std::endl;
        // bzero ou memset a 0
    }
    // std::perror("recv: ");
}