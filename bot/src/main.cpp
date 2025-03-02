#include "../include/Bot.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage is : ./Bot <port> <password>. Aborting..\n";
        return -1;
    }
    try
    {
        Bot bot(argv[1], argv[2]);
        std::string recv_msg;    
        std::string bad_word;
        std::vector<std::string>::iterator it;

        std::cout << "Connection accepted. Now monitoring\n";
        bot.sendMsg("It looks like this city needed a sheriff.. and here I am.\n", 0);
        
        while (!(recv_msg = bot.recvMsg()).empty())
        {
            std::cout << recv_msg;
            t_msg msg = bot.parseMsg(recv_msg);
            bot.monitor(msg);
            bot.checkRoulette(msg);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
        return -1;
    }
}
