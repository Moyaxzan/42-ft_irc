#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/debug.hpp"
#include "../include/Command.hpp"
#include <iostream>


Channel::Channel(void) {
}

Channel::Channel(unsigned int id, std::string name) {
	this->id_ = id;
	this->name_ = name;
	this->inviteOnly_ = false;
	this->restrictedTopic_ = true;
	this->password_ = "";
	this->topic_ = "";
	this->userLimit_ = -1;
}

Channel::Channel(Channel& other) {
	*this = other;
}

Channel& Channel::operator=(Channel& other) {
	this->inviteOnly_ = other.inviteOnly_;
	this->restrictedTopic_ = other.restrictedTopic_;
	this->id_ = other.id_;
	this->topic_ = other.topic_;
	this->name_ = other.name_;
	this->password_ = other.password_;
	this->members_ = other.members_;
	this->operators_ = other.operators_;
	return (*this);
}

Channel::~Channel(void) {
}

// GETTERS
bool	Channel::isInviteOnly(void) const {
	return (this->inviteOnly_);
}

bool	Channel::isTopicRestricted(void) const {
	return (this->restrictedTopic_);
}

unsigned int		Channel::getId(void) const {
	return (this->id_);
}

int Channel::getUserLimit(void) const {
	return (this->userLimit_);
}

std::vector<Client *>	Channel::getOperators(void) const {
	return (this->operators_);
}
std::list<Client *>	Channel::getMembers(void) const {
	return (this->members_);
}

std::string	Channel::getTopic(void) const {
	return (this->topic_);
}

std::string	Channel::getName(void) const {
	return (this->name_);
}

std::string	Channel::getPassword(void) const {
	return (this->password_);
}
// SETTERS
void	Channel::setInviteOnly(bool inviteOnly) {
	this->inviteOnly_ = inviteOnly;
}

void	Channel::setRestrictedTopic(bool restrictedTopic) {
	this->restrictedTopic_ = restrictedTopic;
}

void	Channel::setId(int id) {
	this->id_ = id;
}

void	Channel::setTopic(std::string topic) {
	this->topic_ = topic;
}

void	Channel::setName(std::string name) {
	this->name_ = name;
}

void	Channel::setPassword(std::string password) {
	this->password_ = password;
}

void	Channel::addOperator(Client *opUser) {
	this->operators_.push_back(opUser);
}

void	Channel::removeOperator(Client *opUser) {
	for (std::vector<Client *>::iterator it = this->operators_.begin(); it != this->operators_.end(); it++) {
		if (*it == opUser) {
			this->operators_.erase(it);
			return ;
		}
	}
}

bool	Channel::addMember(Client *user) {
	for (std::list<Client *>::iterator it = this->members_.begin(); it != this->members_.end(); it++) {
		if ((*it)->getNick() == user->getNick()) {
			return (false);
		}
	}
	this->members_.push_back(user);
	return (true);
}

void	Channel::removeMember(Client *user) {
	for (std::list<Client *>::iterator it = this->members_.begin(); it != this->members_.end(); it++) {
		if ((*it)->getNick() == user->getNick()) {
			this->members_.erase(it);
			return ;
		}
	}
}

void	Channel::addInvited(Client *user) {
	this->invited_.push_back(user);
}

void	Channel::removeInvited(Client *user) {
	for (std::vector<Client *>::iterator it = this->invited_.begin(); it == this->invited_.end(); it++) {
		if (*it == user) {
			this->invited_.erase(it);
			return ;
		}
	}
}

void	Channel::setUserLimit(int limit) {
	this->userLimit_ = limit;
}

// *************************************** MEMBER FUNCTIONS **************************************************************//

bool	Channel::isMember(Client *user) {
	std::list<Client *>::iterator it;
	std::string	usrNick = user->getNick();
	for (it = this->members_.begin(); it != this->members_.end(); it++) {
		if ((*it)->getNick() == usrNick) {
			return (true);
		}
	}
	return (false);
}

/*
bool	Channel::isMember(std::string nickname) {
	std::list<Client *>::iterator it;
	for (it = this->members_.begin(); it != this->members_.end(); it++) {
		if ((*it)->getNick() == nickname) {
			return (true);
		}
	}
	return (false);
}
*/

bool	Channel::isOperator(Client *user) {
	std::vector<Client *>::iterator it;
	std::string	usrNick = user->getNick();
	for (it = this->operators_.begin(); it != this->operators_.end(); it++) {
		if ((*it)->getNick() == usrNick) {
			return (true);
		}
	}
	return (false);
}

bool	Channel::isInvited(Client *user) {
	std::vector<Client *>::iterator it;
	std::string	usrNick = user->getNick();
	for (it = this->invited_.begin(); it != this->invited_.end(); it++) {
		if ((*it)->getNick() == usrNick) {
			return (true);
		}
	}
	return (false);
}

bool	Channel::broadcast(Client *sender, std::string message) {
	if (this->members_.empty()) {
		return (false);
	}
	std::list<Client *>::iterator membr;
	for (membr = this->members_.begin(); membr != this->members_.end(); membr++) {
		if (!sender || (*membr)->getNick() != sender->getNick()) {
			(*membr)->sendMessage(message);
		}
	}
	return (true);
}

//returns false if it was the last client
//what happens when last operator leaves ? -> give operator rights to another member
bool	Channel::disconnectClient(Client *client, std::string reason) {
	DEBUG_LOG("disconnecting client " + client->getUsername() + " from " + this->name_);
	this->removeMember(client);
	this->removeOperator(client);
	if (this->getMembers().empty() && this->getOperators().empty()) {
		return (false);
	}
	//TODO broadcast message of deconnexion
	if (reason.length() == 0) {
		this->broadcast(NULL, PARTNOREASON(client->getNick(), client->getUsername(), this->name_));
	} else {
		this->broadcast(NULL, PART(client->getNick(), client->getUsername(), this->name_, reason));
	}
	return (true);
}

std::string	Channel::getNames(void) {
	std::string res = "";
	std::list<Client *>::iterator usr;
	for (usr = this->members_.begin(); usr != this->members_.end(); usr++) {
		if (usr != this->members_.begin()) {
			res += " ";
		}
		if (this->isOperator(*usr)) {
			res += "@";
			res += (*usr)->getNick();
		} else {
			res += (*usr)->getNick();
		}
	}
	return (res);
}

Client *Channel::findMember(std::string client_nick)
{
	std::list<Client *> members = this->getMembers();
	std::list<Client *>::iterator it;

	for (it = members.begin(); it != members.end(); it++)
	{
		if ((*it)->getNick() == client_nick)
			return (*it);
	}
	return NULL;
}
