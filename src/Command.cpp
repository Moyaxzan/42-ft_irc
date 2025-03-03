#include "../include/Command.hpp"
#include "../include/debug.hpp"
#include <sstream>
#include <iostream>

//***************	CONSTRUCTORS / DESTRUCTORS	*********//
Command::Command(void) {
}
Command::Command(const Command &other) {
	(void) other;
}

Command &Command::operator=(const Command &other) {
	(void) other;
	return (*this);
}

Command::~Command() {
}

//***************	MEMBER FUNCTIONS		*************//

bool Command::pass(Client *client, Server *server, std::string &line) {
	DEBUG_LOG("Into checkPassword function");
	std::string	clientPass = line.substr(5);

	if (client->isPasswdSet()) {
		client->sendMessage(ERR_ALREADYREGISTREDPASS);
		return (true); // return true or false ? check with irssi
	}
	if (clientPass.empty()) {
		client->sendMessage(ERR_NEEDMOREPARAMS(client->getNick()));
		return (false); // to check with irssi
	}
	if (clientPass == server->getPassword()) {
		client->setPasswdSet(true);
		client->sendMessage(CORRECTPASS());
		return (true);
	} else {
		client->sendMessage(ERR_WRONGPASS());
		server->disconnectClient(client->getId());
		return (false);
	}
}

bool Command::nick(Client *client, Server *server, std::string &line) {
	std::string	nickname = line.substr(5);

	DEBUG_LOG("Into handleNick function");
	if (!client->isPasswdSet()) {
		client->sendMessage(ERR_NOTREGISTEREDPASS());
		return (false);
	}
	if (!Command::isValidNickname(client, server, nickname)) {
		return (false);
	}
	server->addNickname(nickname);
	client->setNick(nickname); // set the client nickname in its instance
	client->sendMessage(NICKSET(nickname));
	return (true);
}

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
	if (!Command::isValidUsername(client, line, username))
		return (false);
	client->setUser(username); // set the client nickname in its instance
	client->sendMessage(USERSET(username));
	return (true);
}
 
//***************	UTILS  FUNCTIONS		*************//

// USER <username> <hostname> <servername> :<realname>
bool	Command::isValidUsername(Client *client, std::string line, std::string &username) {
	if (!Command::allUserElements(client, line, username)) // check if 4 params or if empty (=needmore params)
		return (false);
	// Ajouter vérification par le bot pour respecter la politique du serveur ?
	if (username.size() > 9 || !correctChars(username, 'u') || username == SERV_NAME) {
		std::string	nickname = client->getNick();
		client->sendMessage(ERR_ERRONEUSUSERNAME(nickname));
		return (false);
	}
	//parse hostname, servername and realname ?
	return (true);
}

bool	Command::isValidNickname(Client *client, Server *server, std::string nickname) {
	DEBUG_LOG("Into isValidNickname function");
	if (nickname.empty()) {
		client->sendMessage(ERR_NONICKNAMEGIVEN());
		return (false);
	}
	// Ajouter vérification par le bot pour respecter la politique du serveur ?
	if (nickname.size() > 9 || !correctChars(nickname, 'n') || nickname == SERV_NAME) {
		client->sendMessage(ERR_ERRONEUSNICKNAME(nickname));
		return (false);
	}
	// check if nickname is available
	if (server->getNicknames().count(nickname)) {
		client->sendMessage(ERR_NICKNAMEINUSE(nickname));
		return (false);
	}
	return (true);
}

bool	Command::correctChars(std::string nickname, char caller) {
	char	c;

	if (!isalpha(nickname[0])) // nick has to begin with a letter
		return (false);
	for (unsigned int i = 1; i < nickname.size(); i++) {
		c = nickname[i];
		if (!isprint(c)) // check if chars are printable
			return (false);
		if (caller == 'n') {
			if (!(isalnum(c) || c == '-' || c == '_' || c == '\\' || c == '|' || c == '[' || c == ']'))
			return (false); // check if it is an allowed char
		}
		else {
			if (!(isalnum(c) || c == '-' || c == '_' || c == '\\' || c == '|' || c == '[' || c == ']' || c == '{'  || c == '}'))
			return (false); // check if it is an allowed char
		}
	}
	return (true);
}

bool	Command::allUserElements(Client *client, std::string line, std::string &username) {
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
