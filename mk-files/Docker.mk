# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Docker.mk                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/29 15:51:12 by vvaucoul          #+#    #+#              #
#    Updated: 2022/08/29 16:15:29 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#******************************************************************************#
#*                         DOCKER RULES IN MAKEFILE !                         *#
#******************************************************************************#

docker-icon:
	printf "                   $(_RED) ##       $(_CYAN) .         \n"
	printf "             $(_RED) ## ## ##      $(_CYAN) ==         \n"
	printf "           $(_RED)## ## ## ##      $(_CYAN)===         \n"
	printf "       /\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\\\___/ ===       \n"
	printf "  $(_BLUE)~~~ $(_CYAN){$(_BLUE)~~ ~~~~ ~~~ ~~~~ ~~ ~ $(_CYAN)/  ===- $(_BLUE)~~~$(_CYAN)\n"
	printf "       \\\______$(_WHITE) o $(_CYAN)         __/           \n"
	printf "         \\\    \\\        __/            \n"
	printf "          \\\____\\\______/               \n"
	printf " $(_END)                                         \n"

docker-run: ascii
	@printf "$(_LCYAN)- DOCKER $(_END)$(_DIM)----------------$(_END) $(_LYELLOW)[$(_LWHITE)⚠️ $(_LYELLOW)]$(_END) $(_LYELLOW)\n$(_END)"
	@cd Docker > /dev/null ; sh compil.sh > /dev/null 2>&1
	@mkdir -p isodir/boot
	@cp Docker/kfs.iso . && cp Docker/kernel.bin . && cp Docker/kernel.bin isodir/boot/kernel.bin
	@printf "$(_LWHITE)- KFS.iso $(_END)$(_DIM)---------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"
	@printf "$(_LWHITE)- KERNEL.bin $(_END)$(_DIM)------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"
	@printf "$(_LWHITE)- BOOT/KERNEL.bin $(_END)$(_DIM)-------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"
	@make -s -C . helper

docker-clear:
	@cd Docker; sh clear.sh > /dev/null 2>&1

docker-helper:
	@clear
	@make -s -C . docker-icon
	@printf "$(_LCYAN)$(_BOLD)docker-run: $(_END)Compile kernel through debian:latest image (fix grub cdrom booting)\n"
	@printf "$(_LCYAN)$(_BOLD)docker-clear: $(_END)Clear docker directories and clean docker project files\n"
	@printf "$(_LCYAN)$(_BOLD)docker-helper: $(_END)display this helper\n"

.PHONY: docker-run docker-clear