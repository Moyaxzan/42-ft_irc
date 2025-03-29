#include "../include/argparsing.hpp"
#include "../include/Server.hpp"

const char * InvalidNumberOfArguments::what() const throw() {
	return ("Invalid number of arguments.");
}

const char * InvalidPortNumber::what() const throw() {
	return ("Invalid port number\nport must be an int between 1024 and 65535");
}

/*
const char * InvalidPortNumber::what() const throw() {
	return ("Invalid port number\nport must be an int between 1024 and 65535");
}
*/

const char * InvalidPassword::what() const throw() {
	return ("Invalid password\npassword can't be empty, please enter at least a character");
}

const char * SocketError::what(void) const throw() {
	return ("Socket Error has occured");
}

const char * BindError::what(void) const throw() {
	return ("Bind Error has occured\nCheck that your port is available");
}

const char * ListenError::what(void) const throw() {
	return ("Listen Error has occured");
}

const char * AcceptError::what(void) const throw() {
	return ("Accept Error has occured");
}

const char * SelectError::what(void) const throw() {
	return ("Select Error has occured");
}

