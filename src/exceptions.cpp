#include "../include/exceptions.hpp"

const char * InvalidNumberOfArguments::what() const throw() {
	return ("Invalid number of arguments.");
}

const char * InvalidPortNumber::what() const throw() {
	return ("Invalid port number\nport must be an int between 0 and 65535");
}
