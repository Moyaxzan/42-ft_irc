#include "../../include/Command.hpp"
#include "../../include/Channel.hpp"
#include "../../include/debug.hpp"
#include <sstream>

/**
 * NAMES Command Handler
 *
 * Usage: NAMES [<channel>]
 * Example: NAMES #coucou
 * 
 * Lists the users in a given channel or all visible channels.
 * - <channel>: The specific channel to retrieve user list from.
 *   If omitted, returns users from all public channels.
 *
 * Server response:
 * - RPL_NAMREPLY (353): Lists users in the requested channel.
 *   Format: 353 <user> <symbol> <channel> :<usernames>
 *   - <symbol>: '=' for public, '*' for private, '@' for secret channels.
 *   - Users with '@' are operators.
 * - RPL_ENDOFNAMES (366): Indicates the end of the NAMES list.
 *
 * Example response:
 *   :server 353 LoneRider = #coucou :@admin user1 user2
 *   :server 366 LoneRider #coucou :End of /NAMES list.
 *
 * If the channel does not exist or is private/secret and the user is not a member,
 * no response is sent.
 */

bool Command::names(Client *client, Server *server, const std::string &line)
{
    DEBUG_LOG("Into /NAMES handler");
    
    std::string command, channel_name;
	std::istringstream	iss(line);
	iss >> command >> channel_name;
    Channel *channel;

    if (!channel_name.length())
    {
        client->sendMessage(ERR_NEEDMOREPARAMS(client->getNick(), "NAMES"));
        return false;
    }
    channel = server->getChannelByName(channel_name);
    if (!channel)
    {
        client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), channel_name));
        return false;
    }
    client->sendMessage(LISTNAMES(client->getNick(), channel_name, channel->getNames()));
    return true;
}
