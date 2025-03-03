#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include "Client.hpp"
# include "Server.hpp"

class Command {
	private:
		Command();
		Command(const Command &other);
		Command &operator=(const Command &other);
		~Command();

		static bool	isValidUsername(Client *client, std::string line, std::string &username);
		static bool	isValidNickname(Client *client, Server* server, std::string nickname);

		static bool	correctChars(std::string nickname, char caller);
		static bool	allUserElements(Client *client, std::string line, std::string &username);

	public:
		static bool pass(Client *client, Server *server, std::string &line);
		static bool nick(Client *client, Server *server, std::string &line);
		static bool user(Client *client, std::string &line);
		// static void join(Client *client, Server *server, std::string &line);
		// static void part(Client &client, Server &server, const std::vector<std::string> &args);
		// static void privMsg(Client &client, Server &server, const std::vector<std::string> &args);
		// static void quit(Client &client, Server &server, const std::vector<std::string> &args);
		static void cap(Client *client, const std::string& line);
};

#define SERV_NAME ":localhost"

//****************************	  CAP MACROS	***********************************//
#define CAPLS_RESP() (SERV_NAME " CAP * LS :")
#define CAPREQ_RESP(requests) (SERV_NAME " CAP * NAK:" + (requests))

//****************************	BAD AUTH MACROS	***********************************//
#define ERR_NOTREGISTEREDPASS()  (SERV_NAME " 451 * :Please authenticate first with PASS command")
#define ERR_NOTREGISTEREDNICK()  (SERV_NAME " 451 * :Please authenticate first with NICK command")
#define ERR_ALREADYREGISTREDPASS() (SERV_NAME " 462 * :You may not reregister")
#define ERR_ALREADYREGISTRED(nick) (SERV_NAME " 462 " + (nick) + " :You may not reregister")

//****************************	PASSWORD MACROS	***********************************//
#define CORRECTPASS() (SERV_NAME " NOTICE AUTH :Password accepted")
#define ERR_WRONGPASS() (SERV_NAME " NOTICE AUTH :Invalid password")

//****************************	NICKNAME MACROS	***********************************//
#define ERR_NONICKNAMEGIVEN() (SERV_NAME " 431 * :No nickname given")
#define ERR_NICKNAMEINUSE(nick) (SERV_NAME " 433 " + (nick) + " :Nickname is already in use")
#define ERR_ERRONEUSNICKNAME(nick) (SERV_NAME " 432 " + (nick) + " :Erroneous nickname")
#define NICKSET(nick) (SERV_NAME " NOTICE AUTH :Nickname set to " + (nick))

//****************************	USERNAME MACROS	***********************************//
#define ERR_ERRONEUSUSERNAME(nick) (SERV_NAME " 461 " + (nick) + " USER :Invalid username")
#define USERSET(user) (SERV_NAME " NOTICE AUTH :Username set to " + (user))
#define ERR_NEEDMOREPARAMS(nick) (SERV_NAME " 461 " + (nick) + " :Not enough parameters")

#endif
