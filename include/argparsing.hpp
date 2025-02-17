#include <exception>
#include "../include/server.hpp"

t_args	parsing(int argc, char *argv[]);

class InvalidNumberOfArguments : public std::exception {
	public :
		const char * what() const throw();
};

class InvalidPortNumber : public std::exception {
	public :
		const char * what() const throw();
};
