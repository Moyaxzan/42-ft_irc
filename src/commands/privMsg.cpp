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

// Carriage Return = '\r' | Line Feed = '\n'
void	cleanMsg(std::string &message) {
	DEBUG_LOG("Into cleanMsg()");
	if (!message.empty() && message[0] == ' ')
		message.erase(0, 1);
	if (!message.empty() && message[0] == ':')
		message.erase(0, 1);
	if (!message.empty() && message[message.size() - 1] == '\r')
		message.erase(message.size() - 1);
}

bool	correctMsg(std::string line, std::string &message, Client *client) {
	DEBUG_LOG("Into correctMsg()");
	if (line.size() > 512) // good IRC clients should already prevent this error by cutting the message before sending it
		return (client->sendMessage(ERR_INPUTTOOLONG(client->getNick())), false);
	cleanMsg(message);
	if (message.empty()) // inutile, vérifier plutot que le message ne comporte pas que des espaces
		return (client->sendMessage(ERR_NOTEXTTOSEND(client->getNick())), false);
	// Ajouter vérification par le bot
	return (true);
}

int	getTargetFd(const std::map<std::string, int> &nickFd, const std::string &target) {
	std::map<std::string, int>::const_iterator it = nickFd.find(target);
	if (it != nickFd.end())
		return (it->second); // returns fd associated to the nickname
	return (-1);  // returns -1 if no client with the targeted nickname is found
}

int	isValidTarget(std::string target, Client *client, Server *server) {
	DEBUG_LOG("Inside isValidTarget()");
	if (!server->getNicknames().count(target))
		return (client->sendMessage(ERR_NOSUCHNICK(client->getNick())), -1);
	if (target == client->getNick())
		return (client->sendMessage(ERR_CANNOTSENDTOSELF(client->getNick())), -1);

	int	targetFd = getTargetFd(server->getNickFd(), target);
	if (targetFd == -1)
		return (-1); // useful ? 
	std::map<int, Client*> clients_ = server->getClients();
	if (!clients_[targetFd]->isWelcomeSent())// check if targeted client is fully authenticated
		return (client->sendMessage(ERR_TARGETNOTAUTH(client->getNick(), target)) , -1);
	return (targetFd);
}

void	handleSendError(int targetFd, std::string target, Client *client, Server *server) {
	std::cout << "Error: send() function failed" << std::endl;
	if (errno == EPIPE || errno == ECONNRESET) {
		client->sendMessage(ERR_TARGETDISCONNECTED(client->getNick(), target));
		server->disconnectClient(targetFd);
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		client->sendMessage(ERR_BUFFERFULL(client->getNick(), target));
	} else {
		client->sendMessage(ERR_CANNOTSENDMSG(client->getNick(), target));
	}
	return ;
}

// :<nickname!username@hostname> PRIVMSG <target> :<message>
void	sendPrivMsg(std::string target, std::string message, Client *client, Server *server) {
	DEBUG_LOG("Inside sendPrivMsg");

	int targetFd = isValidTarget(target, client, server);
	if (targetFd == -1)
		return ;	
	std::string sender = client->getNick() + "!" + client->getUsername() + "@127.0.0.1";
	std::string	privMsg = ":" + sender + " PRIVMSG " + target + " :" + message + "\r\n";
	if (send(targetFd, privMsg.c_str(), privMsg.size(), 0) == -1) {
		handleSendError(targetFd, target, client, server);
		return ;
	}
	std::cout << "Private message successfully sent from " << client->getNick() << " to " << target << " with fd = " << targetFd << " :" << std::endl;
	std::cout << "|" << privMsg << "|" << std::endl;
	return ;
}

bool Command::privMsg(Client *client, Server *server, const std::string& line) {
	DEBUG_LOG("Inside privMsg handler");

	if (!client->isWelcomeSent()) // irssi seems to already be handling this case
		return (client->sendMessage(ERR_NOTREGISTERED()), false);
	std::istringstream	iss(line);
	std::string			cmd, target, message;

	iss >> cmd >> target;	
	std::getline(iss, message);
	if (!correctMsg(line, message, client))
		return (false);
	if (target[0] == '#')
		;// handle channel messages
	else
		sendPrivMsg(target, message, client, server);
	return (true);
}
