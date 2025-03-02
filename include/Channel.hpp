#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <vector>
# include <list>

class Channel {
	private:
		unsigned int		id_;
		std::vector<int>	operators_;
		std::list<int>		members_;

	public:
		Channel(void);
		Channel(unsigned int id);
		Channel(Channel& other);
		Channel& operator=(Channel& other);
		~Channel(void);


		unsigned int		getId(void);
		std::vector<int>&	getOperators(void);
		std::list<int>&		getMembers(void);

		void	setId(unsigned int id);
		void	addOperator(unsigned int operatorId);
		void	addMember(unsigned int memberId);

		void	removeOperator(unsigned int operatorId);
		void	removeMember(unsigned int memberId);
};

#endif
