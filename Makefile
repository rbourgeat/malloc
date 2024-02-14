# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/14 15:21:40 by rbourgea          #+#    #+#              #
#    Updated: 2024/02/14 16:30:15 by rbourgea         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC				= gcc
CFLAGS 			= -Wall -Wextra -Werror
IFLAGS 			= -I./inc

ifeq ($(HOSTTYPE),)
	HOSTTYPE	= $(shell uname -m)_$(shell uname -s)
endif

SRC_DIR 		= src
OBJ_DIR 		= obj
SUB_DIR			= utils
DIRS			= $(addprefix $(OBJ_DIR)/, $(SUB_DIR))

NAME			= libft_malloc_$(HOSTTYPE).so
LINK_NAME		= libft_malloc.so
SRC				= malloc.c \
					heap.c \
					print.c \
					iter_heap.c
SUB_SRC			= ft_putnbr_base.c \
					ft_strlen.c \
					ft_memcpy.c
SRC				+= $(addprefix utils/, $(SUB_SRC))
OBJ				= $(SRC:%.c=$(OBJ_DIR)/%.o)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) -shared -o $@ $(OBJ)
	@echo "$(CL_LINE)🔨 $@"
	@(ln -sf $@ $(LINK_NAME) 2> /dev/null \
		&& echo "✅ $(LINK_NAME)") \
		|| echo "🔴 $(LINK_NAME)"

all: $(NAME)

$(OBJ_DIR):
	@mkdir $@ $(DIRS)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.c | $(OBJ_DIR)
	@printf "$(CL_LINE)[$(COMP)] $< ...\r"
	@$(CC) $(CFLAGS) $(IFLAGS) -fPIC -c $< -o $@

clean:
	@rm -Rf $(OBJ_DIR)
	@echo "🗑️  $(OBJ_DIR)"

fclean: clean
	@rm -Rf $(LINK_NAME) $(NAME)
	@echo "🗑️  $(LINK_NAME)"
	@echo "🗑️  $(NAME)"

re: fclean all

.PHONY: all clean fclean re run
