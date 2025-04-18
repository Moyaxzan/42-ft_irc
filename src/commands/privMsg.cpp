#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>
#include <sstream>
#include <cerrno>

/*
** --------------------------- PRIVMSG COMMAND ---------------------------
**
** Usage: PRIVMSG <target> :<message>
**
** The PRIVMSG command is used to send private messages to a user or a channel. 
** If the target is a channel, all users in the channel receive the message.
** If the target is a nickname, only that user receives it.
**
** Example:
**   PRIVMSG John :Howdy, partner!
**   PRIVMSG #saloon :Who’s up for a duel?
**
** Expected Server Response:
**   - The message is forwarded to the target.
**   - An error if the target does not exist or is unavailable.
**
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

bool	correctMsg(std::string line, std::string &message, Client *client, Server *server) {
	DEBUG_LOG("Into correctMsg()");
	if (line.size() > 512) // good IRC clients should already prevent this error by cutting the message before sending it
		return (client->bufferMessage(server, ERR_INPUTTOOLONG(client->getNick())), false);
	cleanMsg(message);
	if (message.empty())
		return (client->bufferMessage(server, ERR_NOTEXTTOSEND(client->getNick())), false);
	return (true);
}

int	getTargetFd(const std::map<std::string, int> &nickFd, const std::string &target) {
	std::map<std::string, int>::const_iterator it = nickFd.find(target);
	if (it != nickFd.end())
		return (it->second); // returns fd associated to the nickname
	return (-1);  // returns -1 if no client with the targeted nickname is found
}

int	Command::isValidTarget(std::string target, Client *client, Server *server) {
	DEBUG_LOG("Inside isValidTarget()");
	if (!server->getNicknames().count(target))
		return (client->bufferMessage(server, ERR_NOSUCHNICK(client->getNick(), target)), -1);
	if (target == client->getNick())
		return (client->bufferMessage(server, ERR_CANNOTSENDTOSELF(client->getNick())), -1);

	int	targetFd = getTargetFd(server->getNickFd(), target);
	if (targetFd == -1)
		return (-1);
	std::map<int, Client*> clients_ = server->getClients();
	if (!clients_[targetFd]->isWelcomeSent()) // check if targeted client is fully authenticated
		return (client->bufferMessage(server, ERR_TARGETNOTAUTH(client->getNick(), target)) , -1);
	return (targetFd);
}

void	handleSendError(int targetFd, std::string target, Client *client, Server *server) {
	std::cout << "Error: send() function failed" << std::endl;
	if (errno == EPIPE || errno == ECONNRESET) {
		client->bufferMessage(server, ERR_TARGETDISCONNECTED(client->getNick(), target));
		server->disconnectClient(targetFd);
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		client->bufferMessage(server, ERR_BUFFERFULL(client->getNick(), target));
	} else {
		client->bufferMessage(server, ERR_CANNOTSENDMSG(client->getNick(), target));
	}
	return ;
}

// :<nickname!username@hostname> PRIVMSG <target> :<message>
void	sendPrivMsg(std::string target, std::string message, Client *client, Server *server) {
	DEBUG_LOG("Inside sendPrivMsg");

	int targetFd = Command::isValidTarget(target, client, server);
	if (targetFd == -1)
		return ;	
	std::string sender = client->getNick() + "!" + client->getUsername() + "@127.0.0.1";
	std::string	privMsg = ":" + sender + " PRIVMSG " + target + " :" + message + "\r\n";

	server->getClientById(targetFd)->bufferMessage(server, privMsg);
	std::cout << "Private message successfully sent from " << client->getNick() << " to " << target << " with fd = " << targetFd << " :" << std::endl;
	std::cout << "|" << privMsg << "|" << std::endl;
	return ;
}

bool Command::privMsg(Client *client, Server *server, const std::string& line) {
	DEBUG_LOG("Inside privMsg handler");

	if (!client->isWelcomeSent())
		return (client->bufferMessage(server, ERR_NOTREGISTERED()), false);
	std::istringstream	iss(line);
	std::string			cmd, target, message;

	iss >> cmd >> target;
	std::getline(iss, message);
	if (!correctMsg(line, message, client, server))
		return (false);
	if (target[0] != '#') {
		sendPrivMsg(target, message, client, server);
	} else {
		Channel* chan = server->getChannelByName(&target[0]);
		if (!chan) {
			client->bufferMessage(server, ERR_NOSUCHCHANNEL(client->getNick(), &target[0]));
			return (false);
		}
		if (!chan->isMember(client) && !chan->isOperator(client)) {
			client->bufferMessage(server, ERR_CANNOTSENDTOCHAN(client->getNick(), chan->getName()));
			return (false);
		}
		chan->broadcast(server, client, PRIVMSG(
			client->getNick(),
			client->getUsername(),
			chan->getName(),
			message));
	}
	return (true);
}
