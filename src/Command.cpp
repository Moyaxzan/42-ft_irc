#include "../include/Command.hpp"
#include "../include/debug.hpp"
#include <sstream>
#include <iostream>

//***************	CONSTRUCTORS / DESTRUCTORS	*********//
Command::Command(void) {
}
Command::Command(const Command &other) {
	(void) other;
}

Command &Command::operator=(const Command &other) {
	(void) other;
	return (*this);
}

Command::~Command() {
}
