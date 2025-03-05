#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <vector>
# include <list>

class Channel {
	private:
		int		id_;
		std::vector<int>	operators_;
		std::list<int>		members_;
		std::string			topic_;
		std::string			name_;

	public:
		Channel(void);
		Channel(unsigned int id);
		Channel(Channel& other);
		Channel& operator=(Channel& other);
		~Channel(void);

		//GETTERS
		unsigned int		getId(void) const ;
		std::vector<int>	getOperators(void) const ;
		std::list<int>		getMembers(void) const ;
		std::string			getTopic(void) const ;
		std::string			getName(void) const ;

		// SETTERS
		void				setId(int id);
		void				setTopic(std::string topic);
		void				setName(std::string name);
		void				addOperator(int operatorId);
		void				addMember(int memberId);
		void				removeOperator(int operatorId);
		void				removeMember(int memberId);
};

#endif
