#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>

bool	isValidNickname(Client *client, Server *server, std::string nickname);
bool	validNickChars(std::string nickname);

/**
 * NICK Command Handler
 *
 * Usage: NICK <nickname>
 * Example: NICK LoneRider
 * 
 * Sets or changes the user's nickname. Required for registration.
 * - <nickname>: The desired IRC nickname (must be unique and valid).
 *
 * If the nickname is already in use or invalid, the server responds 
 * with an error such as ERR_NICKNAMEINUSE (433) or ERR_ERRONEUSNICKNAME (432).
 */

bool Command::nick(Client *client, Server *server, std::string &line) {
	DEBUG_LOG("Into handleNick function");
	std::string	nickname = line.substr(5);

	if (!client->isPasswdSet()) {
		client->bufferMessage(server, ERR_NOTREGISTEREDPASS());
		return (false);
	}
	if (!isValidNickname(client, server, nickname)) {
		return (false);
	}
	server->addNickname(nickname, client->getId());
	client->setNick(nickname); // set the client's nickname in its instance
	client->bufferMessage(server, NICKSET(nickname));
	return (true);
}

bool	validNickChars(std::string nickname) {
	char	c;

	if (!isalpha(nickname[0])) // nick has to begin with a letter
		return (false);
	for (unsigned int i = 1; i < nickname.size(); i++) {
		c = nickname[i];
		if (!isprint(c)) // check if chars are printable
			return (false);
		if (!(isalnum(c) || c == '-' || c == '_' || c == '\\' || c == '|' || c == '[' || c == ']'))
			return (false); // check if it is an allowed char
	}
	return (true);
}

bool	isValidNickname(Client *client, Server *server, std::string nickname) {
	DEBUG_LOG("Into isValidNickname function");
	if (nickname.empty()) {
		client->bufferMessage(server, ERR_NONICKNAMEGIVEN());
		return (false);
	}
	if (nickname.size() > 9 || !validNickChars(nickname) || nickname == SERV_NAME || nickname == "sheriff") {
		client->bufferMessage(server, ERR_ERRONEUSNICKNAME(nickname));
		return (false);
	}
	if (server->getNicknames().count(nickname)) { // check if nickname is available
		client->bufferMessage(server, ERR_NICKNAMEINUSE(nickname));
		return (false);
	}
	return (true);
}
