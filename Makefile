NAME =  webserver

SRC =		srcs/main.cpp \
			srcs/config.cpp \
			srcs/server.cpp \

INCLUDES =	include/config.hpp \
			include/include.hpp \
			include/server.hpp \


OBJ = $(SRC:.cpp=.o)
CC = c++
RM = rm -f
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

all: $(NAME)

$(NAME): $(OBJ) Makefile
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME) -I ./include

%.o: %.cpp $(INCLUDES)
	$(CC) $(CPPFLAGS) -c $< -o $@ -I ./include

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
