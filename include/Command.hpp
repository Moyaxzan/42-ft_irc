#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include "Client.hpp"
# include "Server.hpp"
# include "debug.hpp"

class Command {
	private:
		Command();
		Command(const Command &other);
		Command &operator=(const Command &other);
		~Command();

	public:
		static bool pass(Client *client, Server *server, std::string &line);
		static bool nick(Client *client, Server *server, std::string &line);
		static bool user(Client *client, std::string &line);
		static void cap(Client *client, const std::string& line);
		static bool ping(Client *client, const std::string& line);
		static bool mode(Client *client, const std::string& line);
		// static void join(Client *client, Server *server, std::string &line);
		// static void part(Client &client, Server &server, const std::vector<std::string> &args);
		static bool privMsg(Client *client, Server *server, const std::string& line);
		// static void quit(Client &client, Server &server, const std::vector<std::string> &args);
};

#define SERV_NAME ":localhost"

// :serverName CODE <client> <argument> :message
//****************************	  CAP MACROS	***********************************//
#define CAPLS_RESP() (SERV_NAME " CAP * LS :")
#define CAPREQ_RESP(requests) (SERV_NAME " CAP * NAK:" + (requests))

//****************************	BAD AUTH MACROS	***********************************//
#define ERR_NOTREGISTERED() (SERV_NAME " 451 * :Please authenticate first")
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
// ERR_NOSUCHNICK (401) <nickname> :No such nick/channel

//****************************	USERNAME MACROS	***********************************//
#define ERR_ERRONEUSUSERNAME(nick) (SERV_NAME " 461 " + (nick) + " USER :Invalid username")
#define USERSET(user) (SERV_NAME " NOTICE AUTH :Username set to " + (user))
#define ERR_NEEDMOREPARAMS(nick) (SERV_NAME " 461 " + (nick) + " :Not enough parameters")

//****************************		PING MACRO	***********************************//
#define PONG(target) ("PONG " + (target))

//****************************		PRIVMSG MACRO	***********************************//
#define ERR_NOTEXTTOSEND(nick) (SERV_NAME " 412 " + (nick) + " :No text to send")
#define ERR_INPUTTOOLONG(nick) ( SERV_NAME " 417 " + (nick) + " :Input line is too long") // no an official code of the IRC protocol
#define ERR_NOSUCHNICK(nick) (SERV_NAME " 401 " + (nick) + " :No such nick")
#define ERR_NOSUCHCHANNEL(nick) (SERV_NAME " 401 " + (nick) + " :No such channel")
#define ERR_CANNOTSENDTOSELF(nick) (SERV_NAME " 431 " + (nick) + " :Cannot send a message to yourself")
#define ERR_TARGETNOTAUTH(nick, target) (SERV_NAME " 484 " + (nick) + " " + (target) + " :Cannot send message to user (not fully registered)")
#define ERR_TARGETDISCONNECTED(nick, target) (SERV_NAME " 401 " + (nick) + " " + (target) + " :Cannot send message to user (target disconnected unexpectedly)")
#define ERR_BUFFERFULL(nick, target) (SERV_NAME " 401 " + (nick) + " " + (target) + " :Cannot send message to user (buffer full)")
#define ERR_CANNOTSENDMSG(nick, target) (SERV_NAME " 401 " + (nick) + " " + (target) + " :Cannot send message to user (unknown send() funct error)")

# define CACTUS "\
" RESET "     .   " GREEN "  _ " RESET "   +    .  " BROWN " ______" RESET "   .          .\n\
" RESET "  (      " GREEN "/|\\" RESET "      .    " BROWN "|      \\" RESET "      .   +\n\
" RESET "      . " GREEN "|||||     " BROWN "_    | |   | | ||" RESET "         .\n\
" RESET " .      " GREEN "|||||    " BROWN "| |  _| | | | |_||" RESET "    .\n\
    " GREEN "/\\  " GREEN "||||| .  " BROWN "| | |   | |      |" RESET "       .\n\
" BROWN " __" GREEN "|||| |||||" BROWN "____| |_|_____________\\__________\n\
" BROWN " . " GREEN "|||| |||||  /\\" BROWN "   _____      _____  .   .\n\
   " GREEN "|||| ||||| ||||" BROWN "   .   .  .         ________\n\
" BROWN "  . " GREEN "\\|`-'|||| ||||" BROWN "    __________       .    .\n\
     " GREEN "\\__ |||| ||||" BROWN "      .          .     .\n\
" BROWN "  __    " GREEN "||||`-'|||" BROWN "  .       .    __________\n\
" BROWN " .    . " GREEN "|||| ___/" BROWN "  ___________             .\n\
" BROWN "    . _ " GREEN "|||||" BROWN " . _               .   _________\n\
" BROWN " _   ___" GREEN "|||||" BROWN "__  ____    __  .          _\n\
" BROWN "      _ " GREEN "`---'" BROWN "    .   ___  .   _   .   .    .\n\
" BROWN " _  ^      .  -    .    \n" RESET


#endif
