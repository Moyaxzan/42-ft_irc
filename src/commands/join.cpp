#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

bool isValidChannelName(const std::string &name);

/*
** ---------------------------- JOIN COMMAND ----------------------------
**
** Usage: JOIN <channel> [<key>]
**
** The JOIN command allows a user to enter a specific channel.
** If a key is required for the channel, it must be provided.
** Users cannot join invite-only channels unless invited,
** and cannot join full channels.
**
** Example:
**   JOIN #saloon
**   JOIN #bank heist123
**
** Expected Server Response:
**	:nick!user@host JOIN :#channel_name       					; Confirmation of joining
**	:server_name 331 nick #channel_name :No topic is set        ; If no topic exists
**	:server_name 332 nick #channel_name :Topic of the channel   ; Topic (if set)
**	:server_name 353 nick = #channel_name :@OpUser User1 User2  ; List of users
**	:server_name 366 nick #channel_name :End of /NAMES list.    ; End of user list
**	:server_name 324 nick #channel_name +nt                     ; Channel modes (optional)
**
*/

//
bool Command::join(Client *client, Server *server, std::string &line)
{

    std::istringstream iss(line);
    std::string command, channelName, password;

    if (!(iss >> command))
    {
        return (false);
    }
    if (!(iss >> channelName))
    {
        return (false);
    }
    if (!(iss >> password))
    {
        password = "";
    }
    if (!isValidChannelName(channelName))
    {
        client->sendMessage(ERR_BADCHANNAME(client->getNick(), channelName));
        return (false);
    }
    Channel *chan;
    server->addChannel(channelName, client, password);
    chan = server->getChannelByName(channelName);
    if (!chan)
    {
        return (false);
    }
    std::string nick = client->getNick();
    std::string user = client->getUsername();
    if (chan->isInviteOnly() && !chan->isInvited(client))
    {
        client->sendMessage(ERR_INVITEONLYCHAN(client->getNick(), channelName));
    }
    else if (chan->getPassword() != password)
    {
        client->sendMessage(ERR_BADCHANNELKEY(client->getNick(), channelName));
    }
    else
    {
        chan->addMember(client);
        client->sendMessage(JOINCONFIRMED(nick, user, channelName));
        chan->broadcast(client, JOINCONFIRMED(nick, user, channelName));
        if (!chan->getTopic().length())
        {
            client->sendMessage(RPL_NOTOPIC(nick, channelName));
        }
        else
        {
            client->sendMessage(RPL_TOPIC(nick, channelName, chan->getTopic()));
        }
        client->sendMessage(LISTNAMES(nick, channelName, chan->getNames()));
        client->sendMessage(ENDOFNAMES(nick, channelName));
    }
    return (true);
}

bool isValidChannelName(const std::string &name)
{
    if (name.empty()) // Test with only spaces
        return false;

    // Check if it starts with '#'
    if (name[0] != '#')
        return (false);

    // Must have at least one character after the prefix
    if (name.size() == 1)
        return (false);

    // Check for invalid characters (spaces, commas, control characters)
    for (size_t i = 1; i < name.size(); i++)
    {
        if (name[i] == ' ' || name[i] == ',' || std::iscntrl(name[i]))
            return (false);
    }

    // Check length
    if (name.size() > 50)
        return (false);

    return (true);
}
