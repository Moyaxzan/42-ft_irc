#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <vector>
# include <list>
# include <string>
# include "Client.hpp"

class Channel {
	private:
		int						id_;
		std::string				topic_;
		std::string				name_;
		std::string				password_;
		std::list<Client *>		members_;
		std::vector<Client *>	operators_;

	public:
		Channel(void);
		Channel(unsigned int id, std::string name);
		Channel(Channel& other);
		Channel& operator=(Channel& other);
		~Channel(void);

		//GETTERS
		unsigned int			getId(void) const ;
		std::vector<Client *>	getOperators(void) const ;
		std::list<Client *>		getMembers(void) const ;
		std::string				getTopic(void) const ;
		std::string				getName(void) const ;
		std::string				getPassword(void) const;

		// SETTERS
		void					setId(int id);
		void					setTopic(std::string topic);
		void					setName(std::string name);
		void					setPassword(std::string password);
		void					addOperator(Client *opUser);
		void					addMember(Client *user);
		void					removeOperator(Client *opUser);
		void					removeMember(Client *user);


		//MEMBER FUNCTIONS
		bool					broadcast(std::string message);
};



#endif
