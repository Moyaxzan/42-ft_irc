#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/debug.hpp"
#include <iostream>


Channel::Channel(void) {
}

Channel::Channel(unsigned int id, std::string name) {
	this->id_ = id;
	this->name_ = name;
	this->inviteOnly_ = false;
	this->password_ = "";
	this->topic_ = "";
}

Channel::Channel(Channel& other) {
	*this = other;
}

Channel& Channel::operator=(Channel& other) {
	this->inviteOnly_ = other.inviteOnly_;
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

unsigned int		Channel::getId(void) const {
	return (this->id_);
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
		if (*it == user) {
			return (false);
		}
	}
	this->members_.push_back(user);
	return (true);
}

void	Channel::removeMember(Client *user) {
	for (std::list<Client *>::iterator it = this->members_.begin(); it != this->members_.end(); it++) {
		if (*it == user) {
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

// *************************************** MEMBER FUNCTIONS **************************************************************//

bool	Channel::isOperator(Client *user) {
	std::vector<Client *>::iterator it;
	for (it = this->operators_.begin(); it != this->operators_.end(); it++) {
		if ((*it)->getNick() == user->getNick()) {
			return (true);
		}
	}
	return (false);
}

bool	Channel::isInvited(Client *user) {
	std::vector<Client *>::iterator it;
	for (it = this->invited_.begin(); it != this->invited_.end(); it++) {
		if ((*it)->getNick() == user->getNick()) {
			return (true);
		}
	}
	return (false);
}

bool	Channel::broadcast(std::string message) {
	for (
		std::list<Client *>::iterator it = this->members_.begin();
		it != this->members_.end();
		it++) {
		(*it)->sendMessage(message);
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
		DEBUG_LOG((*usr)->getNick());
		if (this->isOperator(*usr)) {
			res += "@";
			res += (*usr)->getUsername();
		} else {
			res += (*usr)->getUsername();
		}
	}
	return (res);
}
