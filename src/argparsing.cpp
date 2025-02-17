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
	if (!(sstream && sstream.eof()) || args.port > 65535) {
		throw (InvalidPortNumber());
	}
	// check password ?
	args.portStr = std::string(argv[1]);
	args.password = std::string(argv[2]);
	return (args);
}
