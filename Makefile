# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/04 12:41:15 by dtimofee          #+#    #+#              #
<<<<<<< New base: Feature/config (#5)
#    Updated: 2026/03/31 12:51:06 by nefimov          ###   ########.fr        #
||||||| Common ancestor
#    Updated: 2026/03/26 17:29:23 by nefimov          ###   ########.fr        #
=======
#    Updated: 2026/03/30 12:50:19 by tsemenov         ###   ########.fr        #
>>>>>>> Current commit: Feat: min server setup
#                                                                              #
# **************************************************************************** #

CXX = c++

INC_DIRS = include include/config
IFLAG = $(addprefix -I, $(INC_DIRS))

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 $(IFLAG)

NAME = webserv
SRC_DIR = src/
OBJ_DIR = obj/
<<<<<<< New base: Feature/config (#5)
SRC_FILES = main.cpp config/ServerConfig.cpp config/RouteConfig.cpp
||||||| Common ancestor
SRC_FILES = main.cpp config/ServerConfig.cpp
=======
SRC_FILES = main.cpp config/ServerConfig.cpp
SRC_FILES += Server.cpp
>>>>>>> Current commit: Feat: min server setup
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
