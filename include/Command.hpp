#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <iostream>
#include "Client.hpp"
#include "Server.hpp"
#include "debug.hpp"

std::vector<std::string>	split(std::string line, char delimiter);

class Command {
	private:
		Command();
		Command(const Command &other);
		Command &operator=(const Command &other);
		~Command();

	public:
		static bool			pass(Client *client, Server *server, std::string &line);
		static bool			nick(Client *client, Server *server, std::string &line);
		static bool			user(Server *server, Client *client, std::string &line);
		static void			cap(Server *server, Client *client, const std::string& line);
		static bool			ping(Server *server, Client *client, const std::string& line);
		static bool			mode(Server *server, Client *client, const std::string& line);
		static bool			join(Client *client, Server *server, std::string &line);
		static int			isValidTarget(std::string target, Client *client, Server *server);
		static bool			privMsg(Client *client, Server *server, const std::string& line);
		// static void quit(Client &client, Server &server, const std::vector<std::string> &args);
		static bool			invite(Client *client, Server *server, const std::string& line);
		static bool			kick(Client *client, Server *server, const std::string& line);
		static bool			topic(Client *client, Server *server, const std::string& line);
        static void			removeClientFromChannel(Channel *channel, Client *client);
		static std::string	part(Client *client, Server *server, const std::string & line);
		static bool			names(Client *client, Server *server, const std::string &line);
};

bool isValidChannelName(const std::string &name);

#define SERV_NAME ":localhost"

// :serverName CODE <client> <argument> :message
//****************************	 JOIN MACROS	***********************************//
#define JOINCONFIRMED(nick, user, channel) (":" + (nick) + "!" + (user) + "@127.0.0.1 JOIN :" + channel)
#define LISTNAMES(nick, channel, names) (SERV_NAME " 353 " + (nick) + " = " + (channel) + " :" + (names))
#define ENDOFNAMES(nick, channel) (SERV_NAME " 366 " + (nick) + " " + (channel) + " :End of /NAMES list.")

//****************************	  CAP MACROS	***********************************//
#define CAPLS_RESP() (SERV_NAME " CAP * LS :")
#define CAPREQ_RESP(requests) (SERV_NAME " CAP * NAK:" + (requests))

//****************************	BAD AUTH MACROS	***********************************//
#define ERR_NOTREGISTERED() (SERV_NAME " 451 * :Please authenticate first")
#define ERR_NOTREGISTEREDPASS() (SERV_NAME " 451 * :Please authenticate first with PASS command")
#define ERR_NOTREGISTEREDNICK() (SERV_NAME " 451 * :Please authenticate first with NICK command")
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
#define ERR_NEEDMOREPARAMS(nick, function) (SERV_NAME " 461 " + (nick) + " " + (function) + " :Not enough parameters")

//****************************		PART MACRO	***********************************//
#define PARTNOREASON(nick, user, channel) (":" + (nick) + "!" + (user) + "@127.0.0.1 PART " + (channel))
#define PART(nick, user, channel, reason) (":" + (nick) + "!" + (user) + "@127.0.0.1 PART " + (channel) + " :" + (reason))

//****************************		PING MACRO	***********************************//
#define PONG(target) ("PONG " + (target))

//****************************	 JOIN MACROS	***********************************//
#define JOINCONFIRMED(nick, user, channel) (":" + (nick) + "!" + (user) + "@127.0.0.1 JOIN :" + channel)
#define TOPICNOTSET(nick, channel) (SERV_NAME " 331 " + (nick) + " " + (channel) + " :No topic is set")
#define JOINTOPIC(nick, channel, topic) (SERV_NAME " 332 " + (nick) + " " + (channel) + " :" + topic)
#define LISTNAMES(nick, channel, names) (SERV_NAME " 353 " + (nick) + " = " + (channel) + " :" + (names))
#define ENDOFNAMES(nick, channel) (SERV_NAME " 366 " + (nick) + " " + (channel) + " :End of /NAMES list.")
#define ERR_CHANNELFULL(nick, channel) (SERV_NAME " 471 " + (nick) + " " + (channel) + " :Cannot join channel (+l)")
#define ERR_BADCHANNAME(nick, channel) (SERV_NAME " 479 " + (nick) + " " + (channel) + " :Invalid channel name")
#define ERR_INVITEONLYCHAN(nick, channel) (SERV_NAME " 473 " + (nick) + " " + (channel) + " :Cannot join channel (+i)")
#define ERR_BADCHANNELKEY(nick, channel) (SERV_NAME " 475 " + (nick) + " " + (channel) + " :Cannot join channel (+k)")

//****************************		PRIVMSG MACROS	***********************************//
#define ERR_NOTEXTTOSEND(nick) (SERV_NAME " 412 " + (nick) + " :No text to send")
#define ERR_INPUTTOOLONG(nick) (SERV_NAME " 417 " + (nick) + " :Input line is too long") // no an official code of the IRC protocol
#define ERR_NOSUCHNICK(nick, target) (SERV_NAME " 401 " + (nick) + " " + (target) + " :")
#define ERR_NOSUCHCHANNEL(nick, channel) (SERV_NAME " 403 " + (nick) + " " + (channel) + " :No such channel")
#define ERR_CANNOTSENDTOSELF(nick) (SERV_NAME " 431 " + (nick) + " :Cannot send a message to yourself")
#define ERR_TARGETNOTAUTH(nick, target) (SERV_NAME " 484 " + (nick) + " " + (target) + " :Cannot send message to user (not fully registered)")
#define ERR_TARGETDISCONNECTED(nick, target) (SERV_NAME " 401 " + (nick) + " " + (target) + " :Cannot send message to user (target disconnected unexpectedly)")
#define ERR_BUFFERFULL(nick, target) (SERV_NAME " 401 " + (nick) + " " + (target) + " :Cannot send message to user (buffer full)")
#define ERR_CANNOTSENDMSG(nick, target) (SERV_NAME " 401 " + (nick) + " " + (target) + " :Cannot send message to user (unknown send() funct error)")
#define ERR_CANNOTSENDTOCHAN(nick, channel) (SERV_NAME " 404 " + (nick) + " " + (channel) + " :Cannot send to channel")
#define PRIVMSG(nick, user, channel, message) (":" + (nick) + "!" + (user) + "@127.0.0.1 PRIVMSG " + (channel) + " :" + (message))

//**************************** MODE MACRO ***********************************//
#define ERR_UNKNOWNMODE(nick, mode) (SERV_NAME " 472 " + (nick) + " " + (mode) + " :is unknown mode")
#define ENDOFBANLIST(nick, channel) (SERV_NAME " 368 " + (nick) + " " + (channel) + " :End of Channel Ban List")
#define ERR_NEEDMOREPARAMSCHAN(channel, command) (SERV_NAME " 461 " + (channel) + " " + (command) + " :Not enough parameters")
//**************************** TOPIC MACRO ***********************************//
#define UNSET_TOPIC(nick, user, channel) (":" + (nick) + "!" + (user) + "@127.0.0.1 TOPIC " + channel + " :")
#define RPL_NOTOPIC(nick, channel) (SERV_NAME " NOTICE " + (channel) + " :No topic is set")
#define RPL_TOPIC(nick, channel, topic) (SERV_NAME " 332 " + (nick) + " " + (channel) + " :" + topic)

//****************************		CHANNEL MACROS	***********************************//
#define ERR_NOTONCHANNEL(nick, channel) (SERV_NAME " 442 " + (nick) + " " + (channel) + " :You're not on that channel")
#define ERR_BADCHANNAME(nick, channel) (SERV_NAME " 479 " + (nick) + " " + (channel) + " :Invalid channel name")
#define ERR_BADCHANLIMIT(nick, channel) (SERV_NAME " 479 " + (nick) + " " + (channel) + " :Bad channel limit")
#define ERR_CHANOPNEEDED(nick, channel) (SERV_NAME " 482 " + (nick) + " " + (channel) + " :: You're the last operator in town buddy, we can't let you abandon your duties just like that!")
#define RPL_YOUREOPER(nick) (SERV_NAME " " + (nick) + " :You are now an IRC operator")
#define NOTICE_OPER(nick, channel) (SERV_NAME " NOTICE " + (channel) + " :Well now, <" + GREEN + nick + RESET + ">. You’ve been deputized. Keep the outlaws in check, and don’t go startin’ no trouble yourself. 🤠")
#define NOTICE_UNOPER(nick, channel) (SERV_NAME " NOTICE " + (channel) + " :The sun sets on <" + GREEN + nick + RESET + ">’s time as Sheriff. The badge is off, the dust settles… and the law moves on. Hope they don’t turn outlaw. 🌵")
#define NOTICE_ALREADYOP(nick, channel) (SERV_NAME " NOTICE " + (channel) + " :Son, you tryna promote <" + GREEN + nick + RESET + ">? They’re already the Sheriff ‘round here! Might wanna check your whiskey before makin’ decisions. 🥃")
#define NOTICE_NOTOP(nick, channel) (SERV_NAME " NOTICE " + (channel) + " :Well now, partner... you tryin’ to strip <" + GREEN + nick + RESET + "> of a badge they never had? That’s like takin’ boots off a barefoot man. Ain’t much to remove. 👢")
#define RPL_AUTOOP(channel, nick) (SERV_NAME " MODE " + (channel) + " +o " + (nick))

//****************************		INVITE MACROS	***********************************//
#define ERR_NOSUCHCHANNEL(nick, channel) (SERV_NAME " 403 " + (nick) + " " + (channel) + " :No such channel")
#define ERR_NOTONCHANNEL(nick, channel) (SERV_NAME " 442 " + (nick) + " " + (channel) + " :You're not on that channel")
#define ERR_CHANOPRIVSNEEDED(nick, channel) (SERV_NAME " 482 " + (nick) + " " + (channel) + " :You're not a channel operator")
#define ERR_USERONCHANNEL(nick, target, channel) (SERV_NAME " 443 " + (nick) + " " + (target) + " " + (channel) + " :Is already on channel")
#define RPL_INVITING(invitorNick, invitorUser, channel, nick) (":" + (invitorNick) + "!" + (invitorUser) + "@127.0.0.1 INVITE " + (nick) + " " + (channel))
#define INVITECONFIRMED(nick, channel, target) (SERV_NAME " 341 " + (nick) + " " + (target) + " " + (channel))

//****************************		KICK MACROS	***********************************//
#define ERR_USERNOTINCHANNEL(nick, target, channel) (SERV_NAME " 441 " + (nick) + " " + (target) + " " + (channel) + " :Target is not in channel")
#define BROADKICK(nick, user, channel, target, reason) (":" + (nick) + "!" + (user) +  "@127.0.0.1 KICK " + (channel) + " " + (target) + (reason))
#define NOTIFYKICK(nick, channel, kicker, reason)  (SERV_NAME " NOTICE " + (nick) + " " + (channel) + " :You have been kicked by " + (kicker) + (reason))

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
