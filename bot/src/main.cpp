#include "../include/Bot.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
        return -1;
    try
    {
        Bot bot(argv[1]);
        std::string msg;
        // bot.sendMsg("Hey, i'm the bot! ready to moderate.\n");
        while (!(msg = bot.recvMsg()).empty())
        {
            if (bot.isStrPbmatic(msg))
                bot.sendMsg("Problematic content detected ! I will strike.\n");
            else if (std::string(msg).find("/RCR") != msg.npos)
            {
                bot.sendMsg("Oh oh! It seems like someone launched a Russian Chat Roulette! Let's do it!\n");
                bot.launchRoulette();
            }
        }
        close(bot.getClientSocket());
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
        return -1;
    }
}
