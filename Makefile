OBJS_DIR	=	.objs
DEPS_DIR	=	.deps
SRCS_DIR	=	srcs
HEADER_DIR	=	include

NAME		=	 webserver
OBJS	=	$(patsubst $(SRCS_DIR)%.cpp, $(OBJS_DIR)%.o, $(SRCS))
DEPS	=	$(OBJS:.o=.d)
#------------------------------------------------------------------------#

#---------------------------------Sources---------------------------------#
SRCS	=		$(SRCS_DIR)/main.cpp \
				$(SRCS_DIR)/config.cpp \
				$(SRCS_DIR)/server.cpp \
				$(SRCS_DIR)/location.cpp \
				$(SRCS_DIR)/Open_html_page.cpp \
				$(SRCS_DIR)/SetupSocket.cpp \
				$(SRCS_DIR)/parseBuffer.cpp \
				$(SRCS_DIR)/HandleClients.cpp \
				$(SRCS_DIR)/delete.cpp \
				$(SRCS_DIR)/httpRequest.cpp \
				$(SRCS_DIR)/CgiHandler.cpp \
				$(SRCS_DIR)/autoIndex.cpp \
				$(SRCS_DIR)/utils/fileToString.cpp \
				$(SRCS_DIR)/parseRequest.cpp \
				$(SRCS_DIR)/utils/debugPrints.cpp \
				$(SRCS_DIR)/ClientUploadState.cpp \
				$(SRCS_DIR)/Client.cpp \
				$(SRCS_DIR)/HttpRequestParser.cpp \
			
#------------------------------------------------------------------------#

#---------------------------------Compilation & Linking---------------------------------#
CC		=	c++
RM		=	rm -f

CFLAGS		=	-g3 -std=c++98 -Wall -Werror -Wextra

INCLUDES	=	-I $(HEADER_DIR) -MMD -MP

#------------------------------------------------------------------------#


#---------------------------------Pretty---------------------------------#
YELLOW	=	\033[1;33m
GREEN	=	\033[1;32m
RESET	=	\033[0m
UP		=	"\033[A"
CUT		=	"\033[K"
#------------------------------------------------------------------------#


all: $(NAME)
-include $(DEPS)

FORCE:

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(@D)
	@echo "$(YELLOW)Compiling [$<]$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 
	@printf $(UP)$(CUT)

$(NAME): $(OBJS) $(LIBFT) Makefile
	@echo "$(YELLOW)Compiling [$<]$(RESET)"
	@$(CC) $(OBJS) $(CFLAGS) $(INCLUDES) -o $@ 
	@printf $(UP)$(CUT)
	@echo "$(GREEN)$(NAME) compiled!$(RESET)"

clean:
	@echo "$(YELLOW)cleaning files$(RESET)"
	@$(RM) $(OBJS) rm -rf $(OBJS_DIR)
	@printf $(UP)$(CUT)
	@echo "$(GREEN)$(NAME) files deleted !$(RESET)"

fclean:	clean
	@echo "$(YELLOW)cleaning files$(RESET)"
	@$(RM) $(NAME)
	@printf $(UP)$(CUT)
	@echo "$(GREEN)$(NAME) executable deleted !$(RESET)"

re:	fclean $(NAME)

.PHONY:	all clean fclean re bonus FORCE
