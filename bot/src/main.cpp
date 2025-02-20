#include "../include/Bot.hpp"

int main(int argc, char **argv)
{
    (void)argc;

    Bot le_sheriff;

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

    std::cout << "Connected to server!\n";
    char buffer[1024];
    memset(buffer, '\0', 1024);
    send(client_socket, "Hey, i'm the bot! ready to moderate.\n", 37, 0);
    while (recv(client_socket, buffer, 1024, 0) != 0 || recv(client_socket, buffer, 1024, 0) != -1)
    {
        std::string msg = buffer;
        if (le_sheriff.isStrPbmatic(msg))
        {
            std::string warning = "Problematic content detected ! I will strike.\n";
            send(client_socket, warning.c_str(), warning.size(), 0);
        }
        else if (std::string(msg).find("/RCR") != msg.npos)
        {
            std::cout << "Oh oh! It seems like someone launched a Russian Chat Roulette! Let's do it!\n";
            le_sheriff.launchRoulette();
        }
        std::cout << msg << "\n";
        memset(buffer, '\0', 1024);
    }
    close(client_socket);
}
