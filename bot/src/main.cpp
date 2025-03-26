#include "../include/Bot.hpp"
#include "../include/global.hpp"

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
        std::vector<std::string>::iterator it;

        std::cout << "Connection accepted. Now monitoring\n";

        while (!(recv_msg = bot.recvMsg()).empty())
        {
            if (recv_msg.find("INVITE") != std::string::npos)   // detect invite and join appropriate chan
            {
                std::string channel_tmp = recv_msg.substr(recv_msg.find("#"));
                if (channel_tmp.find("\r") != std::string::npos && channel_tmp.find("\n") != std::string::npos)
                    channel_tmp.resize(channel_tmp.length() - 2);
                bot.sendMsg("JOIN " + channel_tmp, 0, true);
                channel = channel_tmp;
                break ;
            }
        }

        bot.sendMsg("It looks like this city needed a sheriff.. and here I am.\n", 0, false);
        
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
