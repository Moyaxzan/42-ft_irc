#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>
#include <sstream>

/*
** ---------------------------- KICK COMMAND ----------------------------
**
** Usage: KICK <canal> <utilisateur> [<message>]
**
** The KICK command allows an operator to kick a user from a specific channel.
** It can be provided with a message describing the reason of the kicking.
** A user can kick itself from a channel.
** 
** Example:
** In irssi: /kick #saloon user1,user2 [Too noisy]
**
** --------------------------------------------MODIFY----------------------------------------------------------------------
** Expected Server Response:
**	:InvitorNick!user@host INVITE Bob #mychannel       					; Notification of INVITE
**	:server_name 341 nick target #channel_name INVITE Bob #mychannel	; Confirmation of INVITE
**	:server_name 403 nick #channel_name :No such channel				; If channel doesn't exist
**	:server_name 442 nick #channel_name :You're not on that channel		; If invitor is not on the channel
**	:server_name 482 nick #channel_name :You're not a channel operator  ; If channel is +i && invitor is not operator
**	:server_name 443 nick target #channel_name :is already on channel   ; If invited client is already on channel
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
	std::string command, channel, targets, reason;

	// TEST NB ARG : voir si controlé par irssi, sinon, le faire + ERR_NEEDMOREPARAMS (461) : "Not enough parameters"
	if (!(iss >> command))
		return (false);
	if (!(iss >> channel)) 
		return (false);
	if (!(iss >> targets)) 
		return (false);
	if (!(iss >> reason)) 
		return (false);
	Channel	*chan = server->getChannelByName(channel);
	if (!chan) // Does channel exist ?
		return (client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), channel)), false);
	if (!chan->isMember(client)) // is client member of channel ?
		return (client->sendMessage(ERR_NOTONCHANNEL(client->getNick(), channel)), false);
	if (!chan->isOperator(client)) // is client an operator ?
		return (client->sendMessage(ERR_CHANOPRIVSNEEDED(client->getNick(), channel)), false);
	std::string	kickerNick = client->getNick();
	std::string	kickerUsername = client->getUsername();

	if (reason == ":")
		reason = ":No reason given";
	std::vector<std::string> targetsNick = split(targets, ',');
	for (unsigned int i = 0; i < targetsNick.size(); i++) {
		Client *target = server->getClientByNick(targetsNick[i]);
		if (!target)
			client->sendMessage(ERR_NOSUCHNICK(client->getNick(), targetsNick[0]));
		else if (!chan->isMember(target)) // is kicked client on channel ?
			client->sendMessage(ERR_USERNOTINCHANNEL(kickerNick, targetsNick[i], channel));
		else if (!target->isWelcomeSent())// check if targeted client is fully authenticated
			client->sendMessage(ERR_TARGETNOTAUTH(client->getNick(), targetsNick[i]));
		else {
			removeClientFromChannel(chan, target);
			chan->broadcast(client, BROADKICK(kickerNick, kickerUsername, channel, targetsNick[i], reason));
			target->sendMessage(NOTIFYKICK(targetsNick[i], channel, kickerNick, reason));
			// target->sendMessage(kickerNick + "!" + kickerUsername + "@127.0.0.1 KICK " + chan->getName() + " " + target->getNick() + " " + reason);
		}
	}
	return (true);
}
