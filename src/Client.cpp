#include "../include/Client.hpp"

Client::Client(void) {
	this->isAuth_ = false;
}

Client::Client(std::string nickname, unsigned int id, std::string username) {
	this->isAuth_ = true;
	this->nickname_ = nickname;
	this->auth_.first = id;
	this->auth_.second = username;
}

Client::Client(Client& other) {
	*this = other;
}

Client& Client::operator=(Client& other) {
	this->isAuth_ = other.isAuth_;
	this->nickname_ = other.nickname_;
	this->currChannel_ = other.currChannel_;
	this->joinedChannels_ = other.joinedChannels_;
	// auth info ?
}

Client::~Client(void) {
}

bool	Client::isAuth(void) {
	return (this->isAuth_);
}

std::string	Client::getNick(void) {
	return (this->nickname_);
}

unsigned int	Client::getCurrChannel(void) {
	return (this->currChannel_);
}

std::list<unsigned int>	Client::getJoinedChannels(void) {
	return (this->joinedChannels_);
}

std::pair<unsigned int, std::string>	Client::getAuthInfos(void) {
	return (this->auth_);
}