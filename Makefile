include make_source/source.mk

NAME := ircserv
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98
CPPFLAGS := -Iinclude
SRC_DIR := src
OBJ_DIR := obj
SRCS := main.cpp $(SRCS_SERV) $(SRCS_UTILS)
OBJS := $(SRCS:.cpp=.o)
VPATH := $(SRC_DIR):$(OBJ_DIR)
OUTPUT_OPTION = -o $(OBJ_DIR)/$@

all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	mkdir -p $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(addprefix $(OBJ_DIR)/,$(OBJS)) -o $@

debug: CXXFLAGS += -D_GLIBCXX_DEBUG -g3 -DDEBUG=1
debug: re

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all debug clean fclean re
