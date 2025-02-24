#include "../include/Bot.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
        return -1;
    try
    {
        Bot bot(argv[1]);
        int err;

        char buffer[1024];
        memset(buffer, '\0', 1024);
        bot.sendMsg("Hey, i'm the bot! ready to moderate.\n");
        while ((err = recv(bot.getClientSocket(), buffer, 1024, 0)) != -1)
        {
            std::cout << bot.getClientSocket();
            std::string msg = buffer;
            if (bot.isStrPbmatic(msg))
            {
                std::string warning = "Problematic content detected ! I will strike.\n";
                send(bot.getClientSocket(), warning.c_str(), warning.size(), 0);
            }
            else if (std::string(msg).find("/RCR") != msg.npos)
            {
                std::string begin_game = "Oh oh! It seems like someone launched a Russian Chat Roulette! Let's do it!\n";
                send(bot.getClientSocket(), begin_game.c_str(), begin_game.size(), 0);
                bot.launchRoulette();
            }
            std::cout << msg << "\n";
            memset(buffer, '\0', 1024);
        }
        close(bot.getClientSocket());
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
        return -1;
    }
}
