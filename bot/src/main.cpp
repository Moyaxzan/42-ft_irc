#include "../include/Bot.hpp"

int main(int argc, char **argv)
{
    (void) argc;
    sockaddr_in server_infos;
    server_infos.sin_family = AF_INET;
    server_infos.sin_port = htons(atoi(argv[1]));
    server_infos.sin_addr.s_addr = inet_addr(SERV_IP);
    
    std::cout << atoi(argv[1]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
        return (std::cout << "Socket error! Aborting..\n", -1);

    if (connect(client_socket, (sockaddr *)&server_infos, sizeof(server_infos)) == -1)
        return (std::cout << "Can't connect to server! Aborting..\n", -1);

    std::cout << "Connected to server! bye.\n";
    close(client_socket);
}
