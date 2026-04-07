# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/04 12:41:15 by dtimofee          #+#    #+#              #
#    Updated: 2026/03/31 21:30:45 by tsemenov         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++

INC_DIRS = include include/config include/server include/handler
IFLAG = $(addprefix -I, $(INC_DIRS))

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 $(IFLAG)

NAME = webserv
SRC_DIR = src/
OBJ_DIR = obj/
SRC_FILES = main.cpp config/ServerConfig.cpp config/RouteConfig.cpp handler/Handler.cpp handler/HttpResponse.cpp
SRC_FILES += server/Server.cpp
SRCS = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJS = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
# 	mkdir -p $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test:
	@echo "Testing..."
	@echo "Done!"

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re test
