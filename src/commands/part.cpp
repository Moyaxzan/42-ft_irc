#include "../../include/Command.hpp"
#include <sstream>

/*
** ---------------------------- PART COMMAND ----------------------------
**
** Usage: PART <channel> [<message>]
**
** The PART command allows a user to leave a specific channel.
** If a message is provided, it will be shown to the remaining users.
** Users can only part from channels they are currently in.
**
** Example:
**   PART #saloon
**   PART #bank "I'm outta here!"
**
** Expected Server Response:
**   :nick!user@host PART #channel_name :Leaving message         ; Confirmation of leaving
**
** Possible Errors:
**   :server_name 403 nick #channel_name :No such channel        ; If the channel doesn’t exist
**   :server_name 442 nick #channel_name :You're not on that channel ; If the user is not in the channel
**
*/

static void disconnectClientFromChannels(Client *client, Server *server, std::string channelsNamesRaw, std::string reason)
{
    std::vector<std::string> channels_names;
    std::vector<std::string>::iterator it;
    Channel *chan;

    channels_names = split(channelsNamesRaw, ',');
    for (it = channels_names.begin(); it != channels_names.end(); it++)
    {
        chan = server->getChannelByName(*it);
        if (!chan) {
            client->sendMessage(server, ERR_NOSUCHCHANNEL(client->getNick(), *it));
        } else if (!chan->isMember(client)) {
            client->sendMessage(server, ERR_NOTONCHANNEL(client->getNick(), chan->getName()));
        } else {
			if (!chan->disconnectClient(server, client, reason)) {
				server->deleteChan(chan);
				return;
			}
			client->sendMessage(server, PART(client->getNick(), client->getUsername(), chan->getName(), reason));
		}
    }
}

std::string	Command::part(Client *client, Server *server, const std::string & line)
{
    DEBUG_LOG("Into part cmd handler\n");
    std::istringstream iss(line);
    std::string command, channelName, reason;

    iss >> command >> channelName;
    if (line.find(':') != std::string::npos)
        reason = line.substr(line.find(':')).erase(0, 1);
    if (!channelName.length())
    {
        client->sendMessage(server, ERR_NEEDMOREPARAMS(client->getNick(), "PART"));
        return ("");
    }
    disconnectClientFromChannels(client, server, channelName, reason);
    return channelName;
}
