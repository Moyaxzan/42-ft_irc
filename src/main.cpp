#include <iostream>
#include <string>

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


/* ecutable will be run as follows:
	./ircserv <port> <password> */
int	main(int argc, char *argv[]) {
	if (argc == 2 && std::string(argv[1]) == "--help") {
		printHelp();
		return (0);
	}
	if (argc != 3) {
		std::cerr << "Invalide number of arguments." << std::endl;
		std::cerr << "see --help for more informations..." << std::endl;
	}
}
