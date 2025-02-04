#include <exception>

class InvalidNumberOfArguments : public std::exception {
	public :
		const char * what() const throw();
};
