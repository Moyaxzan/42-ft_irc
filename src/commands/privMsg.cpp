#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>

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

bool Command::privMsg(Client *client, Server *server, const std::string& line) {
	DEBUG_LOG("inside privMsg handler");
	(void) client;
	(void) server;
	(void) line;
	return (true);
}
