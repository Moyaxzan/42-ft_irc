#include "../include/Channel.hpp"
#include "../include/Client.hpp"


Channel::Channel(void) {
}

Channel::Channel(unsigned int id, std::string name) {
	this->id_ = id;
	this->name_ = name;
}

Channel::Channel(Channel& other) {
	*this = other;
}

Channel& Channel::operator=(Channel& other) {
	this->id_ = other.id_;
	this->operators_ = other.operators_;
	this->members_ = other.members_;
	return (*this);
}

Channel::~Channel(void) {
}

// GETTERS
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

void	Channel::addMember(Client *user) {
	this->members_.push_back(user);
}

void	Channel::removeOperator(Client *opUser) {
	for (std::vector<Client *>::iterator it = this->operators_.begin(); it == this->operators_.end(); it++) {
		if (*it == opUser) {
			this->operators_.erase(it);
			return ;
		}
	}
}

void	Channel::removeMember(Client *user) {
	for (std::list<Client *>::iterator it = this->members_.begin(); it == this->members_.end(); it++) {
		if (*it == user) {
			this->members_.erase(it);
			return ;
		}
	}
}

// *************************************** MEMBER FUNCTIONS **************************************************************//

bool	Channel::broadcast(std::string message) {
	for (
		std::list<Client *>::iterator it = this->members_.begin();
		it != this->members_.end();
		it++) {
		(*it)->sendMessage(message);
	}
	return (true);
}
