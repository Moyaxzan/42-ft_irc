#ifndef DEBUG_HPP
# define DEBUG_HPP


# define RESET	"\e[0m"
# define GREEN	"\e[32m"
# define RED	"\e[31m"
# define YELLOW	"\e[33m"
# define BLUE	"\e[34m"
# define BROWN	"\e[38;5;130m"
# define CYAN    "\e[36m"
# define MAGENTA "\033[35m"

# ifndef DEBUG
#  define DEBUG 0
# endif

#define DEBUG_LOG(s) if (DEBUG) {\
	std::cout << GREEN << "DEBUG: " << s << RESET << std::endl; \
}

#endif
