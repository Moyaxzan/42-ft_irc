#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>

/**
 * PASS Command Handler
 *
 * Usage: PASS <password>
 * Example: PASS secret123
 * 
 * Provides a password for authentication before registration.
 * - <password>: The server password (if required). Only accepted once.
 *
 * This command must be sent before NICK and USER to complete registration.
 * If the password is incorrect or missing, the server responds with 
 * an error such as ERR_WRONGPASS or ERR_NEEDMOREPARAMS (461).
 */

bool Command::pass(Client *client, Server *server, std::string &line) {
	DEBUG_LOG("Into checkPassword function");
	std::string	clientPass = line.substr(5);

	if (client->isPasswdSet()) {
		client->bufferMessage(server, ERR_ALREADYREGISTREDPASS());
		return (true);
	}
	if (clientPass.empty()) {
		client->bufferMessage(server, ERR_NEEDMOREPARAMS(client->getNick(), "PASS"));
		return (false);
	}
	if (clientPass == server->getPassword()) {
		client->setPasswdSet(true);
		client->bufferMessage(server, CORRECTPASS());
		return (true);
	} else {
		client->bufferMessage(server, ERR_WRONGPASS());
		return (false);
	}
}

