# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    ShellRules-Dependencies.mk                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/02 13:18:04 by vvaucoul          #+#    #+#              #
#    Updated: 2023/02/11 13:59:42 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SCRIPTS_DIR = scripts
CONFIG_FILE = mk-files/.config

#*******************************************************************************
#*                      SHELL RULES / KERNEL DEPENDENCIES                      *
#*******************************************************************************

CHECK_HIGHER_HALF_KERNEL	:= $(shell sh $(SCRIPTS_DIR)/isHigherHalfKernel.sh)
CHECK_XORRISO_INSTALL		:= $(shell sh $(SCRIPTS_DIR)/checkXorriso.sh)
CHECK_CLANG_INSTALL			:= $(shell sh $(SCRIPTS_DIR)/checkClang.sh)
CHECK_CCACHE_INSTALL		:= $(shell sh $(SCRIPTS_DIR)/checkCcache.sh)
CHECK_HEPHAISTOS_INSTALL	:= $(shell sh $(SCRIPTS_DIR)/checkHephaistos.sh)

#*******************************************************************************
#*                           CONFIG FILE / MAKEFILE                            *
#*******************************************************************************

CHECK_USE_KVM				:= $(shell cat $(CONFIG_FILE) | grep "USE_KVM" | cut -d '=' -f 2)