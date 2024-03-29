# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Sources-Workflows.mk                               :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/28 13:42:51 by vvaucoul          #+#    #+#              #
#    Updated: 2022/09/28 13:43:47 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#*******************************************************************************
#*                                  WORKFLOWS                                  *
#*******************************************************************************

WSRCS		=	$(shell find kernel/workflows -name '*.c')
WOBJS		=	$(WSRCS:.c=.o)