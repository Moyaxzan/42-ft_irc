#include "../include/exceptions.hpp"
#include <iostream>
#include <string>
#include <sstream>

typedef struct s_args {
	unsigned int port;
	std::string password;
	std::string portStr;
}	t_args;

void printHelp() {
	std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
	std::cout << "  <port>      : The port number on which the IRC server listens for connections." << std::endl;
	std::cout << "  <password>  : The connection password required by clients." << std::endl;
	std::cout << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  --help      : Display this help message and exit." << std::endl;
	std::cout << std::endl;
	std::cout << "Example:" << std::endl;
	std::cout << "  ./ircserv 6667 mypassword" << std::endl;
}

t_args	parsing(int argc, char *argv[]) {
	if (argc != 3) {
		throw (InvalidNumberOfArguments());
	}
	t_args args;
	std::stringstream sstream;
	sstream << argv[1];
	sstream >> args.port;
	if (!(sstream && sstream.eof()) || args.port > 65535) {
		throw (InvalidPortNumber());
	}
	// check password ?
	args.portStr = std::string(argv[1]);
	args.password = std::string(argv[2]);
	return (args);
}

/* ecutable will be run as follows:
	./ircserv <port> <password> */
int	main(int argc, char *argv[]) {
	if (argc == 2 && std::string(argv[1]) == "--help") {
		printHelp();
		return (0);
	}
	try {
		t_args args = parsing(argc, argv);
		(void) args;
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "see --help for more informations..." << std::endl;
		return (1);
	}
	return (0);
}
