#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <list>
# include <string>
# include <string>

class Client {
	private:

		int										id_;
		bool									nickSet_;
		bool									usernameSet_;
		bool									passwdSet_;
		bool									welcomeSent_;
		std::string								nickname_;
		std::string								username_;
		unsigned int							currChannel_;
		std::list<unsigned int>					joinedChannels_;

	public:
		Client(void);
		Client(int id);
		Client(std::string nickname, unsigned int id, std::string username);
		Client(const Client& other);
		Client& operator=(const Client& other);
		~Client(void);

		bool									isNickSet(void) const;
		bool									isUsernameSet(void) const;
		bool									isPasswdSet(void) const;
		bool									isWelcomeSent(void) const;
		std::string								getNick(void) const;
		std::string								getUsername(void) const;
		unsigned int							getCurrChannel(void) const;
		std::list<unsigned int>					getJoinedChannels(void) const;

		void									setId(int id);
		void									setNickSet(bool nickSet);
		void									setUsernameSet(bool usernameSet);
		void									setPasswdSet(bool passwdSet);
		void									setWelcomeSent(bool welcomeSent);
		void									setNick(std::string nickname);
		void									setUser(std::string username);
		void									setCurrChannel(unsigned int channel);

		bool									isAuth(void) const;
		bool									joined(unsigned int channel) const;		// return if user joined channel
		void									addJoinedChann(unsigned int channel);	// add channel to the joined channel list
		void									rmJoinedChann(unsigned int channel);	// removes channel from the joined channel list
};

#endif
