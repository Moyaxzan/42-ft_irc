#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <vector>
# include <list>
# include <string>
# include "Client.hpp"

class Channel {
	private:
		bool					inviteOnly_;
		bool					restrictedTopic_; // new
		unsigned int			userLimit_; // new
		int						id_;
		std::string				topic_;
		std::string				name_;
		std::string				password_;
		std::list<Client *>		members_;
		std::vector<Client *>	operators_;
		std::vector<Client *>	invited_;

	public:
		Channel(void);
		Channel(unsigned int id, std::string name);
		Channel(Channel& other);
		Channel& operator=(Channel& other);
		~Channel(void);

		//GETTERS
		bool					isInviteOnly(void) const;
		bool					isTopicRestricted(void) const;
		unsigned int			getId(void) const ;
		std::vector<Client *>	getOperators(void) const ;
		std::list<Client *>		getMembers(void) const ;
		std::string				getTopic(void) const ;
		std::string				getName(void) const ;
		std::string				getPassword(void) const;

		// SETTERS
		void					setInviteOnly(bool inviteOnly);
		void					setRestrictedTopic(bool restrictedTopic);
		void					setId(int id);
		void					setTopic(std::string topic);
		void					setName(std::string name);
		void					setPassword(std::string password);
		void					addOperator(Client *opUser);
		void					removeOperator(Client *opUser);
		bool					addMember(Client *user);
		void					removeMember(Client *user);
		void					addInvited(Client *user);
		void					removeInvited(Client *user);


		//MEMBER FUNCTIONS
		bool					isOperator(Client *user);
		bool					isInvited(Client *user);
		bool					broadcast(Client *sender, std::string message);
		bool					disconnectClient(Client *client, std::string reason);
		std::string				getNames(void);
};



#endif
