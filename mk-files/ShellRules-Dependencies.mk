# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    ShellRules-Dependencies.mk                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/02 13:18:04 by vvaucoul          #+#    #+#              #
#    Updated: 2022/09/02 13:54:52 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SCRIPTS_DIR = scripts

#*******************************************************************************
#*                      SHELL RULES / KERNEL DEPENDENCIES                      *
#*******************************************************************************

CHECK_HIGHER_HALF_KERNEL = $(shell sh $(SCRIPTS_DIR)/isHigherHalfKernel.sh)
CHECK_XORRISO_INSTALL = $(shell sh $(SCRIPTS_DIR)/checkXorriso.sh)