#include <iostream>
#include <string>
#include <csignal>
#include "../include/argparsing.hpp"
#include "../include/Server.hpp"
#include "../include/debug.hpp"

volatile sig_atomic_t g_stopSig = false;

static void printHelp() {
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

void signalHandler(int signal) {
	(void) signal;
	g_stopSig = true;
	DEBUG_LOG("\nsignal recieved");
}

/* executable will be run as follows:
	./ircserv <port> <password> */
int	main(int argc, char *argv[]) {
	if (argc == 2 && std::string(argv[1]) == "--help") {
		printHelp();
		return (0);
	}
	try {
		t_args args = parsing(argc, argv);
		Server serv(args);
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);
		serv.runServer();
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << std::endl << "see --help for more informations..." << std::endl;
		return (1);
	}
	return (0);
}

/*
--> CAP LS 302 : CAP = CAPability negociation = mécanisme qui permet aux clients et serveurs de négocier des fonctionnalités avancées
LS = List Server capabilities
302 = version du protocole de négociation des capacités

--> Waiting for CAP LS response : après avoir demandé la liste des capacités du serveur, le client attend un réponse du serveur du type :
:<server> CAP <client_fd> LS :capability1 capability2 ...
Sinon, le client reste bloqué

--> Que répondre ? :
Ignorer proprement les CAP, ne sont pas à gérer pour un IRC simple
Répondre en précisant que le serveur n'offre pas de capacité spéciale
*/