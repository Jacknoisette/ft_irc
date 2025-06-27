# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/04 14:28:56 by jdhallen          #+#    #+#              #
#    Updated: 2025/06/11 18:20:04 by jdhallen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include make_source/source.mk

NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -Iinclude
OBJ_DIR = obj
SRCS = main.cpp $(SRCS_SERV) $(SRCS_UTILS)
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

debug: CXXFLAGS += -D_GLIBCXX_DEBUG -g3
debug: re

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all debug clean fclean re
