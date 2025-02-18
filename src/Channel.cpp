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

