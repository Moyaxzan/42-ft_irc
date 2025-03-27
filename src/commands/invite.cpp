#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>
#include <sstream>

/*
** ---------------------------- INVITE COMMAND ----------------------------
**
** Usage: INVITE <nickname> <channel>
**
** The INVITE command invites a user to enter a specific channel.
** If a key is required for the channel, it must be provided by the inviter by privmsg.
** Users cannot INVITE in invite-only channels unless they are operators.
** 
** Example:
** In irssi: /invite Bob #saloon OR /invite Bob
** Command received by server : INVITE Bob #saloon
**
** Expected Server Response:
**	:InvitorNick!user@host INVITE Bob #mychannel       					; Notification of INVITE
**	:server_name 341 nick target #channel_name INVITE Bob #mychannel	; Confirmation of INVITE
**	:server_name 403 nick #channel_name :No such channel				; If channel doesn't exist
**	:server_name 442 nick #channel_name :You're not on that channel		; If invitor is not on the channel
**	:server_name 482 nick #channel_name :You're not a channel operator  ; If channel is +i && invitor is not operator
**	:server_name 443 nick target #channel_name :is already on channel   ; If invited client is already on channel
**
*/

bool Command::invite(Client *client, Server *server, const std::string& line) {
	DEBUG_LOG("Inside invite handler");

	if (!client->isWelcomeSent()) // irssi seems to already be handling this case
		return (client->sendMessage(ERR_NOTREGISTERED()), false);
	std::istringstream iss(line);
	std::string command, targetNick, channel;

	// TEST NB ARG : voir si controlé par irssi, sinon, le faire + ERR_NEEDMOREPARAMS (461) : "Not enough parameters"
	if (!(iss >> command))
		return (false);
	if (!(iss >> targetNick)) 
		return (false);
	if (!(iss >> channel)) 
		return (false);
	Channel	*chan = server->getChannelByName(channel);
	if (!chan) // Does channel exist ?
		return (client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), channel)), false);
	if (!chan->isMember(client)) // is client member of channel ?
		return (client->sendMessage(ERR_NOTONCHANNEL(client->getNick(), channel)), false);
	if (chan->isInviteOnly() && !chan->isOperator(client)) // if channel is invite only is client an operator ?
		return (client->sendMessage(ERR_CHANOPRIVSNEEDED(client->getNick(), channel)), false);
	if (Command::isValidTarget(targetNick, client, server) == -1)
		return (false);
	Client *target = server->getClientByNick(targetNick);
	if (!target)
		return (false);
	std::string	invitorNick = client->getNick();
	std::string	invitorUsername = client->getUsername();
	if (chan->isMember(target)) // is invited client already on channel ?
		return (client->sendMessage(ERR_USERONCHANNEL(invitorNick, targetNick, channel)), false);
	chan->addInvited(target);
	target->sendMessage(RPL_INVITING(invitorNick, invitorUsername, channel, targetNick));
	client->sendMessage(INVITECONFIRMED(invitorNick, channel, targetNick));
	return (true);
}