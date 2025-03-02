#include "../include/Bot.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
        return -1;
    try
    {
        Bot bot(argv[1]);
        std::string recv_msg;    
        std::string bad_word;
        std::vector<std::string>::iterator it;

        bot.sendMsg("Hey, i'm the bot! ready to moderate.\n", 0);
        
        while (!(recv_msg = bot.recvMsg()).empty())
        {
            std::cout << recv_msg;
            t_msg msg = bot.parseMsg(recv_msg);
            bot.monitor(msg);
            bot.checkRoulette(msg);
        }
        close(bot.getClientSocket());
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
        return -1;
    }
}
