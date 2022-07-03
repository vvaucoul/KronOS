# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Colors.mk                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/22 13:47:15 by vvaucoul          #+#    #+#              #
#    Updated: 2022/06/22 13:47:26 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#*******************************************************************************
#*                                   COLORS                                    *
#*******************************************************************************

# This is a minimal set of ANSI/VT100 color codes
_END=$'\033[0m
_BOLD=$'\033[1m
_UNDER=$'\033[4m
_REV=$'\033[7m

# Colors
_GREY=$'\033[30m
_RED=$'\033[31m
_GREEN=$'\033[32m
_YELLOW=$'\033[33m
_BLUE=$'\033[34m
_PURPLE=$'\033[35m
_CYAN=$'\033[36m
_WHITE=$'\033[37m

# Inverted, i.e. colored backgrounds
_IGREY=$'\033[40m
_IRED=$'\033[41m
_IGREEN=$'\033[42m
_IYELLOW=$'\033[43m
_IBLUE=$'\033[44m
_IPURPLE=$'\033[45m
_ICYAN=$'\033[46m
_IWHITE=$'\033[47m

# Light colors
_LGREY=$'\033[90m
_LRED=$'\033[91m
_LGREEN=$'\033[92m
_LYELLOW=$'\033[93m
_LBLUE=$'\033[94m
_LPURPLE=$'\033[95m
_LCYAN=$'\033[96m
_LWHITE=$'\033[97m

# Others
_DIM=$'\033[2m
_CLEAR_LINE=$'\033[K
