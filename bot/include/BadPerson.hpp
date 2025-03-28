#ifndef BADPERSON_HPP
#define BADPERSON_HPP
#include <iostream>

class BadPerson
{
    private:
        std::string username_;
        int strikes_;

    public:
        BadPerson(void);
        BadPerson(BadPerson const & other);
        ~BadPerson();
        BadPerson & operator=(BadPerson const & other);

        BadPerson(std::string const & username);

        std::string const & getName(void);
        int const & getStrike(void);
        void incStrike(void);
};

#endif