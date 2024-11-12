NAME =  webserver

SRC =		srcs/main.cpp \
			srcs/config.cpp \
			srcs/server.cpp \
			srcs/location.cpp \
			srcs/Open_html_page.cpp \
			srcs/SetupSocket.cpp \
			srcs/parseBuffer.cpp \

INCLUDES =	include/config.hpp \
			include/include.hpp \
			include/server.hpp \
			include/location.hpp \
			include/Open_html_page.hpp \
			include/SetupSocket.hpp \
			srcs/parseBuffer.hpp \


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
