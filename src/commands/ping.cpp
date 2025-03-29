#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>
#include <sstream>

/**
 * PING Command Handler
 *
 * Usage: PING <target>
 * Example: PING localhost
 * 
 * Used by the server to check if a client is still connected.
 * - <target>: The server or client to be pinged (usually the server name or client's hostname).
 *
 * The server responds with a PONG message if the client is reachable.
 * If the PING command is not received or responded to correctly, the connection may be terminated.
 */

bool Command::ping(Client *client, const std::string& line) {
	DEBUG_LOG("Into ping command");
	std::istringstream iss(line);
	std::string ping, target;

	iss >> ping >> target;
	return (client->sendMessage(PONG(target)));
}
