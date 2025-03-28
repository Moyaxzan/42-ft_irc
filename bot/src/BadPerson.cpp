#include "../include/BadPerson.hpp"

BadPerson::BadPerson(void) : username_("Default"), strikes_(1)
{
}

BadPerson::BadPerson(BadPerson const & other)
{
    *this = other;
}

BadPerson::~BadPerson(void)
{
}

BadPerson &BadPerson::operator=(BadPerson const & other)
{
    this->username_ = other.username_;
    this->strikes_ = other.strikes_;
    return *this;
}

BadPerson::BadPerson(std::string const & username) : username_(username), strikes_(1)
{
}

std::string const & BadPerson::getName(void)
{
    return this->username_;
}

int const & BadPerson::getStrike(void)
{
    return this->strikes_;
}

void BadPerson::incStrike(void)
{
    this->strikes_ += 1;
}