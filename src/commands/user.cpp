#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>
#include <sstream>

bool	validUserChars(std::string nickname);
bool	allUserElements(Server *server, Client *client, std::string line, std::string &username);
bool	isValidUsername(Server *server, Client *client, std::string line, std::string &username);

/**
 * USER Command Handler
 *
 * Usage: USER <username> 0 * <realname>
 * 		  USER <username> <hostname> <servername> :<realname>
 * Example: USER cowboy 0 * "Billy the Kid"
 * 
 * Sets the user's metadata during registration. Only accepted once.
 * - <username>: IRC username (not the nickname).
 * - 0: Reserved, usually ignored.
 * - *: Unused, often set to "*".
 * - <realname>: Full name or description of the user.
 *
 * Required for completing registration along with NICK.
 */

// Add the server's instance in arguments if this : std::set<std::string>	usernames_; is needed for privmsg
bool Command::user(Server *server, Client *client, std::string &line) {
	DEBUG_LOG("Into handleUser function");
	if (!client->isNickSet()) {
		client->bufferMessage(server, ERR_NOTREGISTEREDNICK());
		return (false);
	}
	if (client->isUsernameSet()) {
		client->bufferMessage(server, ERR_ALREADYREGISTRED(client->getNick()));
		return (true);
	} 
	std::string	username; 
	if (!isValidUsername(server, client, line, username))
		return (false);
	client->setUser(username); // set the client's username in its instance
	client->bufferMessage(server, USERSET(username));
	return (true);
}

bool	validUserChars(std::string username) {
	char	c;

	if (!isalpha(username[0])) // user has to begin with a letter
		return (false);
	for (unsigned int i = 1; i < username.size(); i++) {
		c = username[i];
		if (!isprint(c)) // check if chars are printable
			return (false);
		if (!(isalnum(c) || c == '-' || c == '_' || c == '\\' || c == '|' || c == '[' || c == ']' || c == '{'  || c == '}'))
			return (false); // check if it is an allowed char
	}
	return (true);
}

bool	allUserElements(Server *server, Client *client, std::string line, std::string &username) {
	std::istringstream	iss(line);
	std::string			cmd, hostname, servername, realname;

	iss >> cmd >> username >> hostname >> servername; // extraction of words one by one
	std::getline(iss, realname);
	if (username.empty() || hostname.empty() || servername.empty() || realname.empty()) {
		client->bufferMessage(server, ERR_NEEDMOREPARAMS(client->getNick(), "USER"));
		return (false);
	}
	return (true);
}

bool	isValidUsername(Server *server, Client *client, std::string line, std::string &username) {
	if (!allUserElements(server, client, line, username)) // check if 4 params or if empty (=needmore params)
		return (false);
	if (username.size() > 9 || !validUserChars(username) || username == SERV_NAME) {
		client->bufferMessage(server, ERR_ERRONEUSUSERNAME(client->getNick()));
		return (false);
	}
	return (true);
}
