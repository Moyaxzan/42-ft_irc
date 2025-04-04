#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include <cstddef>
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
		client->bufferMessage(server, ERR_NOSUCHNICK(client->getNick(), target_client));
		return false;
	}
	if (modeType == true)
	{
		if (channel->isOperator(to_promote) == true)
		{
			client->bufferMessage(server, NOTICE_ALREADYOP(to_promote->getNick(), channel->getName()));
			return false;
		}
		channel->addOperator(to_promote);
		channel->broadcast(server, NULL, NOTICE_OPER(to_promote->getNick(), channel->getName()));
	}
	else if (modeType == false)
	{
		if (to_promote->getNick() == client->getNick() && channel->getOperators().size() == 1)
		{
			client->bufferMessage(server, ERR_CHANOPNEEDED(client->getNick(), channel->getName()));
			return false;
		}
		if (channel->isOperator(to_promote) == false)
		{
			client->bufferMessage(server, NOTICE_NOTOP(to_promote->getNick(), channel->getName()));
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
		client->bufferMessage(server, ERR_NEEDMOREPARAMS(client->getNick(), channel->getName() + " mode +l:"));
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
			client->bufferMessage(server, ERR_BADCHANLIMIT(client->getNick(), channel->getName()));
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

static bool handleChannelEmptyMode(Server* server, Client* client, Channel* channel) {
	std::string finalMsg(SERV_NAME);
	finalMsg += " 324 ";
	finalMsg += client->getNick();
	finalMsg += " ";
	finalMsg += channel->getName();
	if (channel->getPassword().length() > 0) {
		finalMsg += " +k ";
		finalMsg += channel->getPassword();
		finalMsg += " ";
	}
	if (channel->isTopicRestricted()) {
		finalMsg += " +t ";
	}
	if (!channel->getOperators().empty()) {
		finalMsg += " +o ";
		std::vector<Client *>			ops = channel->getOperators();
		std::vector<Client *>::iterator it; 
		std::vector<Client *>::iterator end = ops.end();
		for (it = ops.begin(); it != end; it++) {
			finalMsg += (*it)->getNick();
			if (it + 1 != end) {
				finalMsg += " ";
			}
		}
	}
	if (channel->getUserLimit() > 0) {
		finalMsg += "l ";
		std::stringstream strs;
		strs << channel->getUserLimit();
		finalMsg += strs.str();
	}
	client->bufferMessage(server, finalMsg);
	return (true);
}

static bool handleChannelMode(Server *server, Client* client, Channel* channel, const std::string& mode, std::vector<std::string> args) {
	if (mode.empty()) {
		return handleChannelEmptyMode(server, client, channel);
	}
	if (mode[1] == 'b') {
		client->bufferMessage(server, ENDOFBANLIST(client->getNick(), channel->getName()));
		return true;
	}
	if (!channel->isOperator(client)) {
		client->bufferMessage(server, ERR_CHANOPRIVSNEEDED(client->getNick(), channel->getName()));
		return false;
	}

	unsigned int lokCpt = 0;
	for (int i = 0; mode[i]; i++) {
		if (mode[i] == 'l' || mode[i] == 'o' || mode[i] == 'k')
			lokCpt++;
	}
	std::cout << "count = " << lokCpt << std::endl;
	std::cout << "args size = " << args.size() << "\n";
	if (lokCpt != args.size())
		client->bufferMessage(server, ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
	char modeType = mode[0];
	std::string appliedModes;
	std::vector<std::string> appliedArgs;
	size_t argIndex = 0;

	for (size_t i = 1; i < mode.length(); i++) {
		char modeFlag = mode[i];

		switch (modeFlag) {
			case 'i':
				channel->setInviteOnly(modeType == '+');
				appliedModes += modeType; appliedModes += 'i';
				break;
			case 'o':
				if (argIndex < args.size()) {
					if (checkAddOperator(server, channel, client, modeType == '+', args[argIndex])) {
						appliedModes += modeType;
						appliedModes += 'o';
						appliedArgs.push_back(args[argIndex]);
					}
					argIndex++;
				}
				break;
			case 'l':
				if (modeType == '+' && argIndex < args.size()) {
					if (setChannelUserLimit(server, channel, client, modeType == '+', args[argIndex])) {
						appliedModes += modeType;
						appliedModes += 'l';
						appliedArgs.push_back(args[argIndex]);
					}
					argIndex++;
				} else if (modeType == '-') {
					if (setChannelUserLimit(server, channel, client, false, "")) {
						appliedModes += modeType; appliedModes += 'l';
					}
				}
				break;
			case 'k':
				if (modeType == '+' && argIndex < args.size()) {
					if (setChannelPwd(channel, modeType == '+', args[argIndex])) {
						appliedModes += modeType;
						appliedModes += 'k';
						appliedArgs.push_back(args[argIndex]);
					}
					argIndex++;
				} else if (modeType == '-') {
					if (setChannelPwd(channel, false, "")) {
						appliedModes += modeType; appliedModes += 'k';
					}
				}
				break;
			case 't':
				channel->setRestrictedTopic(modeType == '+');
				appliedModes += modeType; appliedModes += 't';
				break;
			default:
				client->bufferMessage(server, ERR_UNKNOWNMODE(client->getNick(), std::string(1, modeFlag)));
				return false;
		}
	}

	if (!appliedModes.empty()) {
		std::string modeMessage = ":" + client->getNick() + "!" + client->getUsername() + "@127.0.0.1 MODE " + channel->getName() + " " + appliedModes;
		for (size_t i = 0; i < appliedArgs.size(); i++) {
			modeMessage += " " + appliedArgs[i];
		}
		channel->broadcast(server, NULL, modeMessage);
	}
	
	return true;
}

bool Command::mode(Server* server, Client *client, const std::string& line) {
	DEBUG_LOG("in mode handler");

	std::istringstream iss(line);
	std::string command, target, mode;
	std::vector<std::string> args;

	iss >> command >> target;
	iss >> mode;

	std::string arg;
	while (iss >> arg) {
		args.push_back(arg);
	}

	if (target[0] == '#') {
		Channel* channel = server->getChannelByName(target);
		if (!channel) {
			client->bufferMessage(server, ERR_NOSUCHCHANNEL(client->getNick(), target));
			return false;
		} else if (!client->joined(channel->getId())) {
			client->bufferMessage(server, ERR_NOTONCHANNEL(client->getNick(), target));
			return false;
		}
		return handleChannelMode(server, client, channel, mode, args);
	} else {
		if (target != client->getNick()) {
			return false;
		}
		client->setInvisible(true);
		client->bufferMessage(server, ":" + target + " MODE " + target + " " + mode);
	}

	server->log("INFO", "MODE", client->getNick() + " sets " + mode + " on " + target);
	return true;
}
