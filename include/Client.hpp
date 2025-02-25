#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <list>
# include <string>
# include <string>

class Client {
	private:
		bool									isAuth_; // equivalent in server class
		std::string								nickname_;
		std::string								username_; // useful since it's in auth_ ?
		unsigned int							currChannel_;
		std::list<unsigned int>					joinedChannels_;
		std::pair<unsigned int, std::string>	auth_;	// <id, username>

	public:
		Client(void);
		Client(std::string nickname, unsigned int id, std::string username);
		Client(const Client& other);
		Client& operator=(const Client& other);
		~Client(void);

		bool									isAuth(void);
		std::string								getNick(void);
		void									setNick(std::string nickname);
		//std::string								getUser(void);
		//void									setUser(std::string username);
		unsigned int							getCurrChannel(void);
		std::list<unsigned int>					getJoinedChannels(void);
		std::pair<unsigned int, std::string>	getAuthInfos(void);
};

#endif
