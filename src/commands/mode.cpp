#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>

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

static bool checkAddOperator(Server *server, Channel *channel, Client *client, bool modeType, const std::string &target_client)
{
	Client *to_promote = channel->findMember(target_client);

	if (!to_promote)
	{
		client->sendMessage(server, ERR_NOSUCHNICK(client->getNick(), target_client));
		return false;
	}
	if (modeType == true)
	{
		if (channel->isOperator(to_promote) == true)
		{
			client->sendMessage(server, NOTICE_ALREADYOP(to_promote->getNick(), channel->getName()));
			return false;
		}
		channel->addOperator(to_promote);
		channel->broadcast(server, NULL, NOTICE_OPER(to_promote->getNick(), channel->getName()));
	}
	else if (modeType == false)
	{
		if (to_promote->getNick() == client->getNick() && channel->getOperators().size() == 1)
		{
			client->sendMessage(server, ERR_CHANOPNEEDED(client->getNick(), channel->getName()));
			return false;
		}
		if (channel->isOperator(to_promote) == false)
		{
			client->sendMessage(server, NOTICE_NOTOP(to_promote->getNick(), channel->getName()));
			return false;
		}
		channel->removeOperator(to_promote);
		channel->broadcast(server, NULL, NOTICE_UNOPER(to_promote->getNick(), channel->getName()));
	}
	return true;
}

static bool setChannelUserLimit(Server *server, Channel *channel, Client *client, bool modeType, std::string arg)
{
	if (modeType == true && arg.length() == 0)
	{
		client->sendMessage(server, ERR_NEEDMOREPARAMS(client->getNick(), channel->getName() + " mode +l:"));
		return false;
	}
	if (modeType == false)
		channel->setUserLimit(-1);
	else
	{
		int limit = atoi(arg.c_str());
		if (limit == 0)
			limit = -1;
		else if (limit > 1024)
		{
			client->sendMessage(server, ERR_BADCHANLIMIT(client->getNick(), channel->getName()));
			return false;
		}
		channel->setUserLimit(limit);
	}
	return true;
}

static bool setChannelPwd(Channel *channel, bool modeType, const std::string &pwd)
{
	if (modeType == false)
	{
		channel->setPassword("");
		return true;
	}
	channel->setPassword(pwd);
	return true;
}

static bool handleChannelMode(Server *server, Client* client, Channel* channel, const std::string& mode, const std::string &arg) {
	char modeType = mode[0];  // '+' or '-'
	char modeFlag = mode[1];
	
	if (!mode.length())
		return false;		// ignoring irssi freaking random empty "mode"
	else if (mode.length() != 2)
		return (client->sendMessage(server, ERR_UNKNOWNMODE(client->getNick(), mode)));
	if (!channel->isOperator(client))
		return (client->sendMessage(server, ERR_CHANOPRIVSNEEDED(client->getNick(), channel->getName())));
	switch (modeFlag) {
		case 'i': 
			channel->setInviteOnly(modeType == '+');
			break;
		case 'o':
			checkAddOperator(server, channel, client, modeType == '+', arg);
			break;
		case 'l':
			setChannelUserLimit(server, channel, client, modeType == '+', arg);
			break;
		case 'k':
			setChannelPwd(channel, modeType == '+', arg);
			break;
		case 't':
			channel->setRestrictedTopic(modeType == '+');
			break;
		default:
			client->sendMessage(server, ERR_UNKNOWNMODE(client->getNick(), mode));
			return (false);
	}
	server->log("INFO", "MODE", client->getNick() + " sets " + mode + " on " + arg);
	channel->broadcast(server, client, std::string(":") + client->getNick() + " MODE " + channel->getName() + " " + mode);
	return (true);
}

bool Command::mode(Server* server, Client *client, const std::string& line) {
	DEBUG_LOG("in mode handler");

	std::string command, target, mode, arg;
	std::istringstream	iss(line);
	if (!(iss >> command >> target >> mode)) {
		client->sendMessage(server, ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
		return (false);
	}
	iss >> arg;
	if (target[0] == '#') {
		Channel* channel = server->getChannelByName(target);
		if (!channel) {
			client->sendMessage(server, ERR_NOSUCHCHANNEL(client->getNick(), target));
		} else if (client->joined(channel->getId()) == false) {
			client->sendMessage(server, ERR_NOTONCHANNEL(client->getNick(), target));
		}
		return handleChannelMode(server, client, channel, mode, arg);
	} else {
		if (target != client->getNick()) {
			// send error message ?
			return (false);
		}
		client->setInvisible(true);
		client->sendMessage(server, std::string(":") + target + " MODE " + target + " " + mode);
	}
	server->log("INFO", "MODE", client->getNick() + " sets " + mode + " on " + target);
	return (true);
}
