#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>
#include <sstream>

/*
** ---------------------------- KICK COMMAND ----------------------------
**
** Usage: KICK <channel> <nickname> [<message>]
**
** The KICK command allows an operator to kick a user from a specific channel.
** It can be provided with a message describing the reason of the kicking.
** A user can kick itself from a channel.
** 
** Example:
** In irssi: /kick #saloon Bob [Too noisy] OR /kick Bob [Too noisy]
**
** --------------------------------------------MODIFY----------------------------------------------------------------------
** Expected Server Response:
**	:KickerNick!user@host KICK #mychannel Bob :reason     					; Notification of KICK to all channel members
**  :localhost NOTICE nick #channel :You have been kicked by kicker :reason ; Notification of KICK to client
**	:server_name 403 nick #channel_name :No such channel					; If channel doesn't exist
**	:server_name 442 nick #channel_name :You're not on that channel			; If kicker is not on the channel
**	:server_name 482 nick #channel_name :You're not a channel operator  	; If kicker is not an operator
**	:server_name 441 nick target #channel_name :is not on channel  		 	; If kicked client is not on channel
**
*/

std::vector<std::string>	split(std::string str, char delimiter) {
	std::vector<std::string>	vect;
	std::stringstream			ss(str);
	std::string					line;

	while (std::getline(ss, line, delimiter)) {
		if (!line.empty() && line[line.size() - 1] == '\r') // To erase '\r' if it is present
			line.erase(line.size() - 1);
		vect.push_back(line);
	}
	return (vect);
}

void	Command::removeClientFromChannel(Channel *channel, Client *client) {
	channel->removeMember(client);
	if (channel->isOperator(client))
		channel->removeOperator(client);
}

bool Command::kick(Client *client, Server *server, const std::string& line) {
	DEBUG_LOG("Inside kick handler");

	if (!client->isWelcomeSent()) // irssi seems to already be handling this case
		return (client->sendMessage(ERR_NOTREGISTERED()), false);
	std::istringstream iss(line);
	std::string command, channel, nick, reason;

	// TEST NB ARG : voir si controlé par irssi, sinon, le faire + ERR_NEEDMOREPARAMS (461) : "Not enough parameters"
	if (!(iss >> command))
		return (false);
	if (!(iss >> channel)) 
		return (false);
	if (!(iss >> nick)) 
		return (false);
	std::getline(iss, reason);
	Channel	*chan = server->getChannelByName(channel);
	if (!chan) // Does channel exist ?
		return (client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), channel)), false);
	if (!chan->isMember(client)) // is client member of channel ?
		return (client->sendMessage(ERR_NOTONCHANNEL(client->getNick(), channel)), false);
	if (!chan->isOperator(client)) // is client an operator ?
		return (client->sendMessage(ERR_CHANOPRIVSNEEDED(client->getNick(), channel)), false);
	std::string	kickerNick = client->getNick();
	std::string	kickerUsername = client->getUsername();

	if (reason == " :")
		reason = " :No reason given";
	Client *target = server->getClientByNick(nick);
	if (!target)
		// client->sendMessage(ERR_NOSUCHNICK(client->getNick()));
		client->sendMessage(ERR_NOSUCHNICK(client->getNick(), nick));
	else if (!chan->isMember(target)) // is kicked client on channel ?
		client->sendMessage(ERR_USERNOTINCHANNEL(kickerNick, nick, channel));
	else if (!target->isWelcomeSent())// check if targeted client is fully authenticated
		client->sendMessage(ERR_TARGETNOTAUTH(client->getNick(), nick));
	else {
		chan->broadcast(client, BROADKICK(kickerNick, kickerUsername, channel, nick, reason));
		client->sendMessage(BROADKICK(kickerNick, kickerUsername, channel, nick, reason));
		target->sendMessage(NOTIFYKICK(nick, channel, kickerNick, reason));
		removeClientFromChannel(chan, target);
	}
	return (true);
}
