####

NAME = Bot

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -fsanitize=address

DEBUG_FLAGS = -g3 -DDEBUG=1

CXX = c++

<<<<<<< HEAD:Makefile
SRCS =	./src/argparsing.cpp \
		./src/Channel.cpp \
		./src/Client.cpp \
		./src/Command.cpp \
		./src/exceptions.cpp \
		./src/main.cpp \
		./src/Server.cpp \
		./src/commands/cap.cpp \
		./src/commands/join.cpp \
		./src/commands/mode.cpp \
		./src/commands/names.cpp \
		./src/commands/nick.cpp \
		./src/commands/pass.cpp \
		./src/commands/part.cpp \
		./src/commands/ping.cpp \
		./src/commands/privMsg.cpp \
		./src/commands/topic.cpp \
		./src/commands/user.cpp \
		./src/commands/invite.cpp \
		./src/commands/kick.cpp
		
		
=======
SRCS =	./src/Bot.cpp \
		./src/Gun.cpp \
		./src/BadPerson.cpp \
		./src/BotExceptions.cpp \
		./src/main.cpp \
		./src/global.cpp
>>>>>>> bot:bot/Makefile

OBJS_DIR = .objs/

####

OBJ = $(SRCS:.cpp=.o)

OBJS = $(addprefix $(OBJS_DIR), $(OBJ))

DEPS = $(OBJS:.o=.d)

####

$(NAME) 		:	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: re

all				:	$(NAME)

$(OBJS_DIR)%.o	:	%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean			:
	rm -rf $(OBJS_DIR)

fclean			:	clean
	rm -rf $(NAME)

re				:	fclean all

.PHONY			:	all clean fclean re
