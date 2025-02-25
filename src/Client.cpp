#include "../include/Client.hpp"

Client::Client(void) {
	this->isAuth_ = false;
}

Client::Client(std::string nickname, unsigned int id, std::string username) {
	this->isAuth_ = true;
	this->nickname_ = nickname;
	this->username_ = username; // useful ?
	this->auth_.first = id;
	this->auth_.second = username;
}

Client::Client(const Client& other) {
	*this = other;
}

Client& Client::operator=(const Client& other) {
	this->isAuth_ = other.isAuth_;
	this->nickname_ = other.nickname_;
	this->username_ = other.username_;
	this->currChannel_ = other.currChannel_;
	this->joinedChannels_ = other.joinedChannels_;
	// auth info ?
	return (*this);
}

Client::~Client(void) {
}

bool	Client::isAuth(void) {
	return (this->isAuth_);
}

std::string	Client::getNick(void) {
	return (this->nickname_);
}

void	Client::setNick(std::string nickname) {
	this->nickname_ = nickname;
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