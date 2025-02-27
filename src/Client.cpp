#include "../include/Client.hpp"

// *************************************** CONSTRUCTORS/DESTRUCTORS **************************************************************//

Client::Client(void) {
	this->nickSet_ = false;
	this->usernameSet_ = false;
	this->passwdSet_ = false;
}

Client::Client(int id) {
	this->nickSet_ = false;
	this->usernameSet_ = false;
	this->passwdSet_ = false;
	this->id_ = id;
}

Client::Client(std::string nickname, unsigned int id, std::string username) {
	this->nickSet_ = true;
	this->usernameSet_ = true;
	this->nickname_ = nickname;
	this->username_ = username; // useful ?
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
	this->nickSet_ = other.nickSet_;
	this->usernameSet_ = other.usernameSet_;
	this->passwdSet_ = other.passwdSet_;
	this->nickname_ = other.nickname_;
	this->username_ = other.username_;
	this->currChannel_ = other.currChannel_;
	this->joinedChannels_ = other.joinedChannels_;
	return (*this);
}

// *************************************** GETTERS ************************************************************************//

bool	Client::isNickSet(void) const {
	return (this->nickSet_);
}

bool	Client::isUsernameSet(void) const {
	return (this->usernameSet_);
}

bool	Client::isPasswdSet(void) const {
	return (this->passwdSet_);
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

void	Client::setNickSet(bool nickSet) {
	this->nickSet_ = nickSet;
}

void	Client::setUsernameSet(bool usernameSet) {
	this->usernameSet_ = usernameSet;
}

void	Client::setPasswdSet(bool passwdSet) {
	this->passwdSet_ = passwdSet;
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


