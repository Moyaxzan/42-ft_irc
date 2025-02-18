#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <list>
# include <string>
# include <string>

class Client {
	private:
		bool									isAuth_;
		std::string								nickname_;
		unsigned int							currChannel_;
		std::list<unsigned int>					joinedChannels_;
		std::pair<unsigned int, std::string>	auth_;	// <id, username>

	public:
		Client(void);
		Client(std::string nickname, unsigned int id, std::string username);
		Client(Client& other);
		Client& operator=(Client& other);
		~Client(void);

		bool									isAuth(void);
		std::string								getNick(void);
		unsigned int							getCurrChannel(void);
		std::list<unsigned int>					getJoinedChannels(void);
		std::pair<unsigned int, std::string>	getAuthInfos(void);
};

#endif
