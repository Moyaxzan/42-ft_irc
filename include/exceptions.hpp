#include <exception>

class InvalidNumberOfArguments : public std::exception {
	public :
		const char * what() const throw();
};

class InvalidPortNumber : public std::exception {
	public :
		const char * what() const throw();
};
