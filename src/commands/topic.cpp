#include "../../include/Command.hpp"
#include "../../include/Server.hpp"

std::vector<std::string> split(std::string str, std::string delim);
std::string str_join(std::vector<std::string> strs);
void static broadcast_RPLTOPIC(Channel *chan);
void static broadcast_UNSET_TOPIC(Channel *chan);

/**
 * TOPIC Command Handler
 *
 * Usage: TOPIC <channel> [<new_topic>]
 * Example: 
 *   TOPIC #chatroom
 *   TOPIC #chatroom Welcome to our channel!
 * 
 * Gets or sets the topic of a channel.
 * - <channel>: The target IRC channel.
 * - <new_topic> (optional): The new topic to set for the channel.
 *
 * If no <new_topic> is provided, the server responds with the current topic.
 * Changing the topic may require channel operator privileges.
 * Errors include:
 * - ERR_NOTONCHANNEL (442) if the user is not in the channel.
 * - ERR_CHANOPRIVSNEEDED (482) if the user lacks permission to change the topic.
 */

bool Command::topic(Client *client, Server *server, const std::string& line)
{
    std::vector<std::string> cmd_vec = split(line, " ");
    std::vector<std::string>::iterator it;
    Channel *chan;
    
    cmd_vec.erase(cmd_vec.begin() + 0);
    if (!isValidChannelName(cmd_vec[0]) || !(chan = server->getChannelByName(cmd_vec[0])))
    {
        client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), cmd_vec[0]));
        return false;
    }
    if (!client->joined(chan->getId()))
    {
        client->sendMessage(ERR_NOTONCHANNEL(client->getNick(), chan->getName()));
        return false;
    }
    cmd_vec.erase(cmd_vec.begin() + 0);
    if (cmd_vec[0] == ":")
    {
        chan->setTopic("");
        broadcast_UNSET_TOPIC(chan);
        return true ;
    }
    if (cmd_vec[0][0] == ':')
        cmd_vec[0].erase(cmd_vec[0].begin() + 0);
    chan->setTopic(str_join(cmd_vec));
    broadcast_RPLTOPIC(chan);
    return true;
}

std::vector<std::string> split(std::string str, std::string delim)
{
    std::vector<std::string> tokens;
    std::string token;
 
    while (str.find(delim) != str.npos)
    {
        token = str.substr(0, str.find(delim));
        tokens.push_back(token);
        str.erase(0, str.find(delim) + delim.length());
    }
    tokens.push_back(str);
    return tokens;
}

std::string str_join(std::vector<std::string> strs)
{
    std::vector<std::string>::iterator it; 
    std::string final_str;

    for (it = strs.begin(); it != strs.end(); it++)
    {
        final_str += *it;
        final_str.append(" ");
    }
    return final_str;
}

void static broadcast_RPLTOPIC(Channel *chan)
{
    std::list<Client *> chan_members = chan->getMembers();
    std::list<Client *>::iterator it;

    for (it = chan_members.begin(); it != chan_members.end(); it++)
        (*it)->sendMessage(RPL_TOPIC((*it)->getNick(), chan->getName(), chan->getTopic()));
}

void static broadcast_UNSET_TOPIC(Channel *chan)
{
    std::list<Client *> chan_members = chan->getMembers();
    std::list<Client *>::iterator it;

    for (it = chan_members.begin(); it != chan_members.end(); it++)
    {
        (*it)->sendMessage(UNSET_TOPIC((*it)->getNick(), (*it)->getUsername(), chan->getName()));
        std::cout << UNSET_TOPIC((*it)->getNick(), (*it)->getUsername(), chan->getName()) << "\n";
    }
}
