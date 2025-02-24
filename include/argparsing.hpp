#ifndef ARGPARSING_HPP

#define ARGPARSING_HPP

#include <exception>
#include <string>

typedef struct s_args {
	unsigned int port;
	std::string password;
	std::string portStr;
}	t_args;

t_args	parsing(int argc, char *argv[]);

class InvalidNumberOfArguments : public std::exception {
	public :
		const char * what() const throw();
};

class InvalidPortNumber : public std::exception {
	public :
		const char * what() const throw();
};

class InvalidPassword : public std::exception {
	public :
		const char * what() const throw();
};

#endif
