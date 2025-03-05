#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>

/*
** ---------------------------- JOIN COMMAND ----------------------------
**
** Usage: JOIN <channel> [<key>]
**
** The JOIN command allows a user to enter a specific channel.
** If a key is required for the channel, it must be provided.
** Users cannot join invite-only channels unless invited,
** and cannot join full channels.
**
** Example:
**   JOIN #saloon
**   JOIN #bank heist123
**
** Expected Server Response:
**	:nick!user@host JOIN :#channel_name       					; Confirmation of joining
**	:server_name 332 nick #channel_name :Topic of the channel   ; Topic (if set)
**	:server_name 331 nick #channel_name :No topic is set        ; If no topic exists
**	:server_name 353 nick = #channel_name :@OpUser User1 User2  ; List of users
**	:server_name 366 nick #channel_name :End of /NAMES list.    ; End of user list
**	:server_name 324 nick #channel_name +nt                     ; Channel modes (optional)
**
*/

bool Command::join(Client *client, Server *server, std::string &line) {

	(void) client;
	(void) server;
	(void) line;
	return (true);
}
