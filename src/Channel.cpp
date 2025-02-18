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
unsigned int		Channel::getId(void) {
	return (this->id_);
}

std::vector<int>&	Channel::getOperators(void) {
	return (this->operators_);
}

std::list<int>&		Channel::getMembers(void) {
	return (this->members_);
}


// SETTERS
void	Channel::setId(unsigned int id) {
	this->id_ = id;
}

void	Channel::addOperator(unsigned int operatorId) {
	this->operators_.push_back(operatorId);
}

void	Channel::addMember(unsigned int memberId) {
	this->members_.push_back(memberId);
}

void	Channel::removeOperator(unsigned int operatorId) {
	for (std::vector<int>::iterator it = this->operators_.begin(); it == this->operators_.end(); it++) {
		if (*it == operatorId) {
			this->operators_.erase(it);
		}
	}
}

void	Channel::removeMember(unsigned int memberId) {
	for (std::list<int>::iterator it = this->members_.begin(); it == this->members_.end(); it++) {
		if (*it == memberId) {
			this->members_.erase(it);
		}
	}
}