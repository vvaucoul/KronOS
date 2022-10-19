# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    tiny-kernels.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/19 19:09:22 by vvaucoul          #+#    #+#              #
#    Updated: 2022/10/20 00:11:00 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

TINY_DIR = tiny-kernels

tiny-hhk: fclean
	@printf "$(_LWHITE)- COMPILING TINY-HHK $(_END)$(_END)$(_DIM)----$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"
	@make -s -C $(TINY_DIR)/HHK all
	@printf "$(_LWHITE)- TINY-HHK COMPILED $(_END)$(_END)$(_DIM)-----$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"
	@printf "\n$(_LWHITE)- Now you use: '$(_LYELLOW)make tiny-hhk-run$(_END)$(_LWHITE)' or '$(_LYELLOW)make tiny-hhk-run-iso$(_END)$(_LWHITE)' to start the tiny-kernel !$(_END)\n"

tiny-hhk-run: tiny-hhk
	@printf "$(_LWHITE)Running $(_LYELLOW)HHK$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) !\n"
	@qemu-system-i386 -kernel $(TINY_DIR)/HHK/isodir/boot/hhk.bin

tiny-hhk-run-iso: tiny-hhk
	@printf "$(_LWHITE)Running $(_LYELLOW)HHK$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)cdrom$(_LWHITE) !\n"
	@qemu-system-i386 -cdrom $(TINY_DIR)/HHK/hhk.iso

tiny-hhk-clear:
	@make -s -C $(TINY_DIR)/HHK fclean