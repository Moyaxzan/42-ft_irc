####

NAME = Bot

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD

DEBUG_FLAGS = -g -DDEBUG=1

CXX = c++

SRCS =	./src/Bot.cpp \
		./src/Gun.cpp \
		./src/BotExceptions.cpp \
		./src/main.cpp

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
