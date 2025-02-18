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
		std::pair<unsigned int, std::string>	auth_;

	public:
		Client(void);
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
