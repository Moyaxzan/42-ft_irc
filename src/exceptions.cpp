#include "../include/exceptions.hpp"

const char * InvalidNumberOfArguments::what() const throw() {
	return ("Invalid number of arguments.");
}
