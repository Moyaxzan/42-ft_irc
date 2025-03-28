#include "../../include/Command.hpp"
#include "../../include/debug.hpp"
#include <iostream>
#include <sstream>

/**
 * CAP Command Handler
 * 
 * CAP = CAPability negociation
 *
 * Usage: CAP <subcommand> [:<capabilities>]
 * Example: CAP LS 302
 * 
 * Handles capability negotiation between client and server.
 * - LS: Lists Server available capabilities.
 * - REQ: Requests specific capabilities.
 * - ACK: Acknowledges capabilities.
 * - NAK: Rejects requested capabilities.
 * - END: Ends capability negotiation.
 *
 * Typically, the server responds with ":<server> CAP * LS :<capabilities>",
 * but if capabilities aren't supported, it can be ignored.
 */

void Command::cap(Server *server, Client *client, const std::string& line) {
	DEBUG_LOG("Handling CAP command: " + line);

	std::istringstream iss(line);
	std::string command, subcommand;

	iss >> command >> subcommand; // "CAP" -> command; "LS", "REQ", "END" -> subcommand

	if (subcommand == "LS") {
		// Announce supported capabilities : (empty for now ?)
		client->sendMessage(server, CAPLS_RESP());
	} else if (subcommand == "REQ") { // capability request
		client->sendMessage(server, CAPREQ_RESP(line.substr(8))); // Extracts requested capabilities
	} else if (subcommand == "END") {
		DEBUG_LOG("CAP END received, proceeding with authentication.");
	} else {
		DEBUG_LOG("Unknown CAP subcommand: " + subcommand);
	}
}
