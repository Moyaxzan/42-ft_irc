#include "../include/Bot.hpp"

const char * ConnectionError::what() const throw()
{
    return ("Connection with the server failed.\n");
};