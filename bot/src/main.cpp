#include "../include/Bot.hpp"
#include "../include/global.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage is : ./Bot <port> <password>. Aborting..\n";
        return -1;
    }
	std::string recv_msg;    
	std::vector<std::string>::iterator it;
    try {
		Bot bot(argv[1], argv[2]);
		std::cout << "Connection accepted. Now monitoring\n";
			while (!quit && !(recv_msg = bot.recvMsg()).empty()) {
				if (recv_msg.find("INVITE ") != std::string::npos) {  // detect invite and join appropriate chan
					std::string channel_tmp = recv_msg.substr(recv_msg.find("#"));
					if (channel_tmp.find("\r") != std::string::npos && channel_tmp.find("\n") != std::string::npos)
						channel_tmp.resize(channel_tmp.length() - 2);
					bot.sendMsg("JOIN " + channel_tmp, 0, true);
					if (!(recv_msg = bot.recvMsg()).empty() && recv_msg.find(" JOIN ") != recv_msg.npos) {
						channel = channel_tmp;
						std::cout << "entering channel " << channel << std::endl;
						break ;
					}
				}
			}

			bot.sendMsg("It looks like this city needed a sheriff.. and here I am.\n", 0, false);
			bot.sendMsg("Just a quick reminder, fellers : I need operator rights if you want me to do the cleaning 'round here\n", 1, false);
			while (!quit && !(recv_msg = bot.recvMsg()).empty()) {
				if (recv_msg.find("PART ") != recv_msg.npos) {
					if (bot.handlePart()) {
						break;
					}
				}
				if (recv_msg.find("KICK ") != recv_msg.npos && bot.handleKick(recv_msg)) {
					break;
				}
				if (recv_msg.find("SHUTDOWN BOT PLEASE") != recv_msg.npos)
					quit = true;
				t_msg msg = bot.parseMsg(recv_msg);
				bot.monitor(msg);
				bot.checkRoulette(msg);
			}
    } catch (std::exception &e) {
        std::cerr << e.what();
        return -1;
    }
	return (0);
}
