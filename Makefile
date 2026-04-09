# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/04 12:41:15 by dtimofee          #+#    #+#              #
#    Updated: 2026/04/08 16:11:11 by tsemenov         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++

INC_DIRS = include include/config include/server include/handler
IFLAG = $(addprefix -I, $(INC_DIRS))

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 $(IFLAG)
TEST_CXXFLAGS = -Wall -Wextra -std=c++11 $(IFLAG)

NAME = webserv
SRC_DIR = src/
OBJ_DIR = obj/
SRC_FILES = globals.cpp config/ServerConfig.cpp config/RouteConfig.cpp
SRC_FILES += handler/Handler.cpp handler/HttpResponse.cpp
SRC_FILES += server/Server.cpp server/Client.cpp server/ServerHub.cpp
ALL_SRC_FILES = main.cpp $(SRC_FILES)
SRCS = $(addprefix $(SRC_DIR), $(ALL_SRC_FILES))
OBJS = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRCS))

# Test config
TEST_DIR = unit_tests/
TEST_OBJ_DIR = obj/tests/
TEST_NAME = run_tests
# All files except main.cpp

SRCS_NO_MAIN = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJS_NO_MAIN = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRCS_NO_MAIN))

TEST_FILES = tests_response.cpp
TEST_SRCS = $(addprefix $(TEST_DIR), $(TEST_FILES))
TEST_OBJS = $(patsubst $(TEST_DIR)%.cpp, $(TEST_OBJ_DIR)%.o, $(TEST_SRCS))


all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
# 	mkdir -p $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)%.o: $(TEST_DIR)%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(TEST_CXXFLAGS) -I$(TEST_DIR) -c $< -o $@

$(TEST_NAME): $(OBJS_NO_MAIN) $(TEST_OBJS)
	$(CXX) $(TEST_CXXFLAGS) -I$(TEST_DIR) -o $(TEST_NAME) $(OBJS_NO_MAIN) $(TEST_OBJS)

test: $(TEST_NAME)
	./$(TEST_NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(TEST_NAME)

re: fclean all

.PHONY: all clean fclean re test
