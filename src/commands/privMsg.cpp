#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>
#include <sstream>

/**
 * PRIVMSG Command Handler
 * 
 * Irssi command : /msg <nickname> <message> OR (/msg #channel <message> || /join #channel )
 *																			Bonjour tout le monde !
 *
 * Usage:   PRIVMSG <nickname> :<message>				OR		PRIVMSG #channel :<message>
 * Example: PRIVMSG camelia :Salut, comment ça va ?		OR		PRIVMSG #channel :Bonjour tout le monde !
 * 
 * Sends a message to a client or to a channel's clients
 * - <nickname>: The tageted client's nickname
 * - #channel: The tageted channel's name
 */

// Carriage Return = '\r' ; Line Feed = '\n' | rename function
void	removeCRLF(std::string &message) {
	DEBUG_LOG("Into removeCRLF()");
	if (!message.empty() && message[0] == ' ')
		message = message.erase(0, 1);
	if (!message.empty() && message[0] == ':')
		message = message.erase(0, 1);
	if (message.size() >= 2 && message.substr(message.size() - 1) == "\r")
		message = message.erase(message.size() - 1);
}

bool	correctMsg(std::string line, std::string &message, Client *client) {
	DEBUG_LOG("Into correctMsg()");
	removeCRLF(message);
	if (message.empty())
		return (client->sendMessage(ERR_NOTEXTTOSEND(client->getNick())), false);
	// Les clients IRC bien codés coupent déjà leurs messages à cette limite avant de les envoyer
	if (line.size() > 512)
		return (client->sendMessage(ERR_NOTEXTTOSEND(client->getNick())), false);
	return (true);
	// Ajouter vérification par le bot
}

bool	isValidTarget(std::string target, Client *client, Server *server) {
	if (!server->getNicknames().count(target))
		return (client->sendMessage(ERR_NOSUCHNICK(client->getNick())), false);
	if (target == client->getNick())
		return (client->sendMessage(ERR_CANNOTSENDTOSELF(client->getNick())), false);
	return (true);
}

int	getTargetFd(const std::map<std::string, int> &nickFd, const std::string &target) {
	std::map<std::string, int>::const_iterator it = nickFd.find(target);
	if (it != nickFd.end())
		return (it->second); // returns fd associated to the nickname
	return (-1);  // returns -1 if no client with the target nickname is found
}

// :<nickname!username@hostname> PRIVMSG <target> :<message>
void	sendPrivMsg(std::string target, std::string message, Client *client, Server *server) {
	DEBUG_LOG("Inside sendPrivMsg");
	if (!isValidTarget(target, client, server))
		return ;
	int targetFd = getTargetFd(server->getNickFd(), target);
	//if (targetFd == -1)
		//return ;
	DEBUG_LOG("target fd:");
	DEBUG_LOG(targetFd);
	std::string sender = client->getNick() + "!" + client->getUsername() + "@127.0.0.1";
	std::string	privMsg = ":" + sender + " PRIVMSG " + target + " :" + message + "\r\n";
	if (send(targetFd, privMsg.c_str(), privMsg.size(), 0) == -1)
		std::cout << "Couldn't send private message: \"" << privMsg << "\" from " << client->getNick() << " to " << targetFd << std::endl;
	std::cout << "Message: \"" << privMsg << "\" sent from " << client->getNick() << " to " << target << " with fd = " << targetFd << std::endl;
	return ;
}

bool Command::privMsg(Client *client, Server *server, const std::string& line) {
	DEBUG_LOG("Inside privMsg handler");

	if (!client->isWelcomeSent())
		return (client->sendMessage(ERR_NOTREGISTERED()), false);
	std::istringstream	iss(line);
	std::string			cmd, target, message;

	iss >> cmd >> target;	
	std::getline(iss, message);
	std::cout << "Message before correctMsg(): \"" << message << "\"" << std::endl;
	if (!correctMsg(line, message, client))
		return (false);
	std::cout << "Message after correctMsg() : \"" << message << "\"" << std::endl;
	if (target[0] == '#')
		;// handle channel messages
	else
		sendPrivMsg(target, message, client, server);
	return (true);
	/*
	2) -> sendPrivMsg : get the target fd and send the message: :<sender> PRIVMSG <target> :<message>
	if client is +i mode (invisible = true), message can be sent only by the target's friends or by someone from the same channel

	-> broadcastPrivmsg : check if the channel exists and if the sender belongs to the channel ; send the message to all the channel's clients except the sender
	ERR_CANNOTSENDTOCHAN (404) <channel> :Cannot send to channel
	ERR_NOSUCHCHANNEL (403) <channel> :No such channel
	ERR_NOTONCHANNEL (442) <channel> :You're not on that channel
	*/
}
