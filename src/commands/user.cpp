#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>
#include <sstream>

bool	validUserChars(std::string nickname);
bool	allUserElements(Client *client, std::string line, std::string &username);
bool	isValidUsername(Client *client, std::string line, std::string &username);

/**
 * USER Command Handler
 *
 * Usage: USER <username> 0 * <realname>
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

bool Command::user(Client *client, std::string &line) {
	DEBUG_LOG("Into handleUser function");
	if (!client->isNickSet()) {
		client->sendMessage(ERR_NOTREGISTEREDNICK());
		return (false);
	}
	if (client->isUsernameSet()) {
		client->sendMessage(ERR_ALREADYREGISTRED(client->getNick()));
		return (true);
	} 
	std::string	username; 
	if (!isValidUsername(client, line, username))
		return (false);
	client->setUser(username); // set the client nickname in its instance
	client->sendMessage(USERSET(username));
	return (true);
}

bool	validUserChars(std::string username) {
	char	c;

	if (!isalpha(username[0])) // nick has to begin with a letter
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

bool	allUserElements(Client *client, std::string line, std::string &username) {
	std::istringstream	iss(line);
	std::string			cmd, hostname, servername, realname;

	iss >> cmd >> username >> hostname >> servername; // extraction of words one by one
	std::getline(iss, realname);
	if (username.empty() || hostname.empty() || servername.empty() || realname.empty()) {
		std::string	nickname = client->getNick();
		client->sendMessage(ERR_NEEDMOREPARAMS(nickname));
		return (false);
	}
	//if (realname[0] == ':') if parsing of realname
			//realname = realname.substr(1);
	return (true);
}

// USER <username> <hostname> <servername> :<realname>
bool	isValidUsername(Client *client, std::string line, std::string &username) {
	if (!allUserElements(client, line, username)) // check if 4 params or if empty (=needmore params)
		return (false);
	// Ajouter vérification par le bot pour respecter la politique du serveur ?
	if (username.size() > 9 || !validUserChars(username) || username == SERV_NAME) {
		std::string	nickname = client->getNick();
		client->sendMessage(ERR_ERRONEUSUSERNAME(nickname));
		return (false);
	}
	//parse hostname, servername and realname ?
	return (true);
}
