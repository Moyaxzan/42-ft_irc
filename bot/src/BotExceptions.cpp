#include "../include/Bot.hpp"

const char * ConnectionError::what() const throw()
{
    return ("Connection with the server failed.\n");
};

const char * SocketBotError::what() const throw()
{
    return ("Bot socket opening failed.\n");
};

const char * SendError::what() const throw()
{
    return ("Send error.\n");
};

const char * FileOpenError::what() const throw()
{
    return ("Opening file failed\n");
}

const char * WrongPassword::what() const throw()
{
    return ("Connection failed : invalid password\n");
}