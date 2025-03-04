#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>


bool Command::privMsg(Client *client, Server *server, const std::string& line) {
	DEBUG_LOG("inside privMsg handler");
	(void) client;
	(void) server;
	(void) line;
	return (true);
}
