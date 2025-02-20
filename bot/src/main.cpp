#include "../include/Bot.hpp"

int main(int argc, char **argv)
{
    (void)argc;

    Bot le_sheriff(argv[1]);

    char buffer[1024];
    memset(buffer, '\0', 1024);
    send(le_sheriff.getClientSocket(), "Hey, i'm the bot! ready to moderate.\n", 37, 0);
    while (recv(le_sheriff.getClientSocket(), buffer, 1024, 0) != 0 || recv(le_sheriff.getClientSocket(), buffer, 1024, 0) != -1)
    {
        std::string msg = buffer;
        if (le_sheriff.isStrPbmatic(msg))
        {
            std::string warning = "Problematic content detected ! I will strike.\n";
            send(le_sheriff.getClientSocket(), warning.c_str(), warning.size(), 0);
        }
        else if (std::string(msg).find("/RCR") != msg.npos)
        {
            std::string begin_game = "Oh oh! It seems like someone launched a Russian Chat Roulette! Let's do it!\n";
            send(le_sheriff.getClientSocket(), begin_game.c_str(), begin_game.size(), 0);
            le_sheriff.launchRoulette();
        }
        std::cout << msg << "\n";
        memset(buffer, '\0', 1024);
    }
    close(le_sheriff.getClientSocket());
}
