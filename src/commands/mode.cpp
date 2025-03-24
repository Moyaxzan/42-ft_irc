#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include <iostream>
#include <sstream>

/**
 * MODE Command Handler
 *
 * Usage: MODE <target> <modes> [<params>]
 * Example: MODE MyNick +i
 * 
 * The MODE command is used to change the modes of a client or a channel.
 * It can set or remove specific modes for users or channels.
 * 
 * Parameters:
 * - <target>: The user or channel to modify the mode for (e.g., a nickname or channel name).
 * - <modes>: A string of mode characters (e.g., +i, -o).
 * - [<params>]: Optional parameters depending on the mode being set (e.g., a user to give or remove operator status).
 *
 * Common Modes:
 * - +i (Invisible): User is invisible to others, meaning they won't show up in the user list.
 * - +t (Topic Protection): Only channel operators can change the topic of the channel.
 * - +k <password>: Sets a password for a channel. Only users who know the password can join.
 * - +o (Operator): Grants operator status to the target (user or channel).
 * - +l <limit>: Sets a maximum number of users allowed in a channel. If the limit is reached, no more users can join.
 *
 * Example:
 * - MODE MyNick +i  // Sets the "invisible" mode for user "MyNick"
 * - MODE #channel +o MyNick  // Grants operator status to "MyNick" on #channel
 * - MODE #channel +l 50  // Sets the channel user limit to 50 users
 * - MODE #channel +k secretpass  // Sets a password "secretpass" for #channel
 * - MODE #channel +t  // Only operators can change the topic for #channel
 */


// static bool isValidMode(const std::string &mode) {
// 	if (mode.empty() || (mode[0] != '+' && mode[0] != '-'))
// 		return false;

// 	// Check each mode character
// 	for (size_t i = 1; i < mode.size(); i++) {
// 		if (mode[i] != 'i' && mode[i] != 'o' && mode[i] != 'l' &&
// 			mode[i] != 'k' && mode[i] != 't') {
// 			return false;
// 		}
// 	}
// 	return true;
// }

static bool handleChannelMode(Server* server, Client* client, Channel* channel, const std::string& mode) {
	char modeType = mode[0];  // '+' or '-'
	char modeFlag = mode[1];

	(void) server;
	switch (modeFlag) {
		case 'i': 
			channel->setInviteOnly(modeType == '+');
			break;
		case 'o':
			// Handle operator mode
			break;
		case 'l':
			// Handle user limit mode
			break;
		case 'k':
			// Handle channel key mode
			break;
		case 't':
			channel->setRestrictedTopic(modeType == '+');
			break;
		default:
			client->sendMessage(ERR_UNKNOWNMODE(client->getNick(), mode));
			return (false);
	}
	channel->broadcast(client, std::string(":") + client->getNick() + " MODE " + channel->getName() + " " + mode);
	return (true);
}

bool Command::mode(Server* server, Client *client, const std::string& line) {
	DEBUG_LOG("in mode handler");

	std::string command, target, mode;
	std::istringstream	iss(line);
	iss >> command >> target >> mode;
	if (target[0] == '#') {
		Channel* channel = server->getChannelByName(target);
		if (!channel) {
			client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), target));
		} else if (client->joined(channel->getId()) == false) {
			client->sendMessage(ERR_NOTONCHANNEL(client->getNick(), target));
		}
		// } else if (isValidMode(mode) == false) {
		// 	client->sendMessage(ERR_UNKNOWNMODE(client->getNick(), mode));
		// }
		return handleChannelMode(server, client, channel, mode);
	} else {
		if (target != client->getNick()) {
			// send error message ?
			return (false);
		}
		client->setInvisible(true);
		client->sendMessage(std::string(":") + target + " MODE " + target + " " + mode);
	}
	return (true);
}
