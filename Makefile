# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/04 14:28:56 by jdhallen          #+#    #+#              #
#    Updated: 2025/06/27 14:23:34 by jdhallen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include make_source/source.mk

CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

OBJ_DIR = build
NAME = ircserv
SRCS = main.cpp \
		$(SRCS_SERV) $(SRCS_UTILS)


OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
