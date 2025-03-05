#include "../include/Channel.hpp"


Channel::Channel(void) {
}

Channel::Channel(unsigned int id) {
	this->id_ = id;
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

std::vector<int>	Channel::getOperators(void) const {
	return (this->operators_);
}

std::list<int>		Channel::getMembers(void) const {
	return (this->members_);
}

std::string	Channel::getTopic(void) const {
	return (this->topic_);
}

std::string	Channel::getName(void) const {
	return (this->name_);
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

void	Channel::addOperator(int operatorId) {
	this->operators_.push_back(operatorId);
}

void	Channel::addMember(int memberId) {
	this->members_.push_back(memberId);
}

void	Channel::removeOperator(int operatorId) {
	for (std::vector<int>::iterator it = this->operators_.begin(); it == this->operators_.end(); it++) {
		if (*it == operatorId) {
			this->operators_.erase(it);
			return ;
		}
	}
}

void	Channel::removeMember(int memberId) {
	for (std::list<int>::iterator it = this->members_.begin(); it == this->members_.end(); it++) {
		if (*it == memberId) {
			this->members_.erase(it);
			return ;
		}
	}
}
