#include "../include/argparsing.hpp"
#include "../include/Server.hpp"
#include <sstream>

t_args	parsing(int argc, char *argv[]) {
	if (argc != 3) {
		throw (InvalidNumberOfArguments());
	}
	t_args args;
	std::stringstream sstream;
	sstream << argv[1];
	sstream >> args.port;
	if (!(sstream && sstream.eof()) || args.port < 1024 || args.port > 65535) {
		throw (InvalidPortNumber());
	}
	args.portStr = std::string(argv[1]);
	args.password = std::string(argv[2]);
	if (args.password.empty()) {
		throw (InvalidPassword());
	}
	return (args);
}

// ports below 1024 are left for processes with root rights
