# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Sources-Boot.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/27 17:36:21 by vvaucoul          #+#    #+#              #
#    Updated: 2022/06/27 17:41:34 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#*******************************************************************************
#*                                    KBOOT                                    *
#*******************************************************************************

KBOOT_SRCS	=	$(BOOT).s
KBOOT_OBJS	=	$(KBOOT_SRCS:.s=.o)