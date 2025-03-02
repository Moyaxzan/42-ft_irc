#include "../include/Client.hpp"
#include <iostream>
#include <sys/socket.h>
#include <algorithm>

// *************************************** CONSTRUCTORS/DESTRUCTORS **************************************************************//

Client::Client(void) {
	this->passwdSet_ = false;
	this->nickSet_ = false;
	this->usernameSet_ = false;
	this->welcomeSent_ = false;
	// this->currentChannel_ les clients sont mis sur un channel de base ou pas ?
}

Client::Client(int id) {
	std::cout << "id = " << id << std::endl;
	this->id_ = id;
	this->passwdSet_ = false;
	this->nickSet_ = false;
	this->usernameSet_ = false;
	this->welcomeSent_ = false;
	// this->currentChannel_ les clients sont mis sur un channel de base ou pas ?
}

Client::Client(std::string nickname, unsigned int id, std::string username) {
	this->nickSet_ = true;
	this->usernameSet_ = true;
	this->welcomeSent_ = false;
	this->nickname_ = nickname;
	this->username_ = username;
	this->id_ = id;
}

Client::Client(const Client& other) {
	*this = other;
}

Client::~Client(void) {
}

// *************************************** OVERLOAD OPERATORS **************************************************************//
// should copy id ?
Client& Client::operator=(const Client& other) {
	this->passwdSet_ = other.passwdSet_;
	this->nickSet_ = other.nickSet_;
	this->usernameSet_ = other.usernameSet_;
	this->welcomeSent_ = other.welcomeSent_;
	this->nickname_ = other.nickname_;
	this->username_ = other.username_;
	this->currChannel_ = other.currChannel_;
	this->joinedChannels_ = other.joinedChannels_;
	return (*this);
}

// *************************************** GETTERS ************************************************************************//

bool	Client::isPasswdSet(void) const {
	return (this->passwdSet_);
}

bool	Client::isNickSet(void) const {
	return (this->nickSet_);
}

bool	Client::isUsernameSet(void) const {
	return (this->usernameSet_);
}

bool	Client::isWelcomeSent(void) const {
	return (this->welcomeSent_);
}

std::string	Client::getNick(void) const {
	return (this->nickname_);
}

std::string	Client::getUsername(void) const {
	return (this->username_);
}

unsigned int	Client::getCurrChannel(void) const {
	return (this->currChannel_);
}

std::list<unsigned int>	Client::getJoinedChannels(void) const {
	return (this->joinedChannels_);
}

// *************************************** SETTERS ************************************************************************//

void	Client::setId(int id) {
	this->id_ = id;
}

void	Client::setPasswdSet(bool passwdSet) {
	this->passwdSet_ = passwdSet;
}

// setNickSet and setUserSet useful ? it's done in setNick and setUser
void	Client::setNickSet(bool nickSet) {
	this->nickSet_ = nickSet;
}

void	Client::setUsernameSet(bool usernameSet) {
	this->usernameSet_ = usernameSet;
}

void	Client::setWelcomeSent(bool welcomeSent) {
	this->welcomeSent_ = welcomeSent;
}

void	Client::setNick(std::string nickname) {
	this->nickname_ = nickname;
	this->nickSet_ = true;
}

void	Client::setUser(std::string username) {
	this->username_ = username;
	this->usernameSet_ = true;
}

void	Client::setCurrChannel(unsigned int channel) {
	this->currChannel_ = channel;
}

// *************************************** MEMBER FUNCTIONS **************************************************************//

// add welcomeSent ?
bool	Client::isAuth(void) const {
	return (this->nickSet_ && this->usernameSet_ && this->passwdSet_);
}

bool	Client::joined(unsigned int channel) const {
	return (
		std::find(
			this->joinedChannels_.begin(),
			this->joinedChannels_.end(),
			channel)
		!= this->joinedChannels_.end());
}

void	Client::addJoinedChann(unsigned int channel) {
	this->joinedChannels_.push_back(channel);
}

void	Client::rmJoinedChann(unsigned int channel) {
	this->joinedChannels_.remove(channel);
}

void	Client::sendMessage(std::string message) const {
	message += "\r\n";
	if (send(this->id_, message.c_str(), message.size(), 0) == -1) {
		std::cout << "Couln't send message: \"" << message << "\" to " << this->id_ << std::endl;
		return ;
	}
	if (this->nickSet_)
		std::cout << "Message sent to client [" << this->nickname_ << "]: " << message << std::endl;
	else
		std::cout << "Message sent to client [" << this->id_ << "]: " << message << std::endl;
}
