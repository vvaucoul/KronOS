# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    tiny-kernels.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/19 19:09:22 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/04 15:25:10 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

TINY_DIR = tiny-kernels

#*******************************************************************************
#*                                  TINY HHK                                   *
#*******************************************************************************

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

#*******************************************************************************
#*                               TINY MULTIBOOT                                *
#*******************************************************************************

tiny-multiboot: fclean
	@printf "$(_LWHITE)- COMPILING TINY-Multiboot $(_END)$(_END)$(_DIM)----$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"
	@make -s -C $(TINY_DIR)/Multiboot all
	@printf "$(_LWHITE)- TINY-Multiboot COMPILED $(_END)$(_END)$(_DIM)-----$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"
	@printf "\n$(_LWHITE)- Now you use: '$(_LYELLOW)make tiny-Multiboot-run$(_END)$(_LWHITE)' or '$(_LYELLOW)make tiny-Multiboot-run-iso$(_END)$(_LWHITE)' to start the tiny-kernel !$(_END)\n"

tiny-multiboot-run: tiny-multiboot
	@printf "$(_LWHITE)Running $(_LYELLOW)Multiboot$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) !\n"
	@qemu-system-i386 -kernel $(TINY_DIR)/Multiboot/isodir/boot/multiboot.bin

tiny-multiboot-run-iso: tiny-multiboot
	@printf "$(_LWHITE)Running $(_LYELLOW)Multiboot$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)cdrom$(_LWHITE) !\n"
	@qemu-system-i386 -cdrom $(TINY_DIR)/Multiboot/multiboot.iso

tiny-multiboot-clear:
	@make -s -C $(TINY_DIR)/Multiboot fclean

#*******************************************************************************
#*                                 TINY MEMORY                                 *
#*******************************************************************************

tiny-memory: fclean
	@printf "$(_LWHITE)- COMPILING TINY-Memory $(_END)$(_END)$(_DIM)-$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"
	@make -s -C $(TINY_DIR)/Memory all
	@printf "$(_LWHITE)- TINY-Memory COMPILED $(_END)$(_END)$(_DIM)--$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"
	@printf "\n$(_LWHITE)- Now you use: '$(_LYELLOW)make tiny-memory-run$(_END)$(_LWHITE)' or '$(_LYELLOW)make tiny-memory-run-iso$(_END)$(_LWHITE)' to start the tiny-kernel !$(_END)\n"

tiny-memory-run: tiny-memory
	@printf "$(_LWHITE)Running $(_LYELLOW)Memory$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) !\n"
	@qemu-system-i386 -kernel $(TINY_DIR)/Memory/isodir/boot/memory.bin

tiny-memory-run-iso: tiny-memory
	@printf "$(_LWHITE)Running $(_LYELLOW)Memory$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)cdrom$(_LWHITE) !\n"
	@qemu-system-i386 -cdrom $(TINY_DIR)/Memory/memory.iso

tiny-memory-clear:
	@make -s -C $(TINY_DIR)/Memory fclean
