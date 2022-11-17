# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/21 21:08:53 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/17 02:35:24 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME 			= libkfs.a

ifeq ($(CLANG_INSTALLED), false)
	CC 			= ccache gcc
else
	CC 			= ccache clang
endif

CXX 			= clang++
CXXFLAGS		= -Wall -Wextra -Werror -std=c++11 -Wall -Wextra -Werror -Wfatal-errors \
				-fno-builtin -fno-exceptions -fno-stack-protector \
				-nostdlib -nodefaultlibs \
				-ffreestanding -O2 \
				-fno-builtin -fno-rtti


ASM 			= nasm
INLCUDES_PATH 	= -I./includes/ \
				-I./libs

CFLAGS 			= -Wall -Wextra -Werror -Wfatal-errors \
				-fno-builtin -fno-exceptions -fno-stack-protector \
				-nostdlib -nodefaultlibs \
				-std=gnu99 -ffreestanding -O2
LDFLAGS			= 	-g3 -m32
ASM_FLAGS		=	-f elf32 
MK_INCLUDE_DIR	=	mk-files

SRCS 		=	$(shell find srcs -name '*.c') \
				$(shell find libs -name '*.c')
OBJS 		= 	$(SRCS:.c=.o)

SRCS_ASM	=	$(shell find srcs -name '*.s') \
				$(shell find libs -name '*.s')
OBJS_ASM	=	$(SRCS_ASM:.s=.o)

SRCS_CPP	=	$(shell find srcs -name '*.cpp') \
				$(shell find libs -name '*.cpp')
OBJS_CPP	=	$(SRCS_CPP:.cpp=.o)

DEPENDS		=	$(OBJS:.o=.d)
DEPENDS_ASM	=	$(OBJS_ASM:.o=.d)
DEPENDS_CPP	=	$(OBJS_CPP:.o=.d)

#*******************************************************************************
#*                                  INLCUDES                                   *
#*******************************************************************************

include ../$(MK_INCLUDE_DIR)/utils/Colors.mk

%.o: %.cpp
	@printf "$(_LWHITE)    $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LYELLOW) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $(INLCUDES_PATH) -c $< -o $@

%.o: %.c
	@printf "$(_LWHITE)    $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LCYAN) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(CC) $(LDFLAGS) $(CFLAGS) $(INLCUDES_PATH) -MD -c $< -o ${<:.c=.o}

%.o: %.s
	@printf "$(_LWHITE)    $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LPURPLE) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $<
	@$(ASM) $(ASM_FLAGS) $(INLCUDES_PATH) $< -o ${<:.s=.o}

all: $(NAME)

$(NAME): $(OBJS) $(OBJS_ASM) $(OBJS_CPP)
	@ar -rcs $(NAME) $(OBJS) $(OBJS_ASM) $(OBJS_CPP)

clean:
	@rm -f $(OBJS) $(OBJS_ASM) $(OBJS_CPP) $(DEPENDS) $(DEPENDS_ASM) $(DEPENDS_CPP)

fclean: clean
	@rm -f $(NAME)

re: fclean all

-include $(DEPENDS)
-include $(DEPENDS_ASM)
-include $(DEPENDS_CPP)

.PHONY: all clean fclean re