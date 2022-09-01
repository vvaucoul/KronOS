# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    QEMU-Runner.mk                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/29 16:45:20 by vvaucoul          #+#    #+#              #
#    Updated: 2022/09/01 17:19:23 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#******************************************************************************#
#*                         START KERNEL WITH KVM/QEMU                         *#
#******************************************************************************#

run: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) !\n"
	@qemu-system-i386 -smp 1 -kernel isodir/boot/$(BIN) -display gtk -vga std -full-screen 
 
run-sdl: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) !\n"
	@qemu-system-i386 -smp 1 -kernel isodir/boot/$(BIN) -display sdl -vga std -full-screen 

run-iso: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)cdrom$(_LWHITE) !\n"
	@qemu-system-i386 -smp 1 -cdrom $(NAME).iso -display gtk -boot d -vga std -full-screen

run-curses: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)cdrom$(_LWHITE) !\n"
	@qemu-system-i386 -smp 1 -cdrom $(NAME).iso -display curses -vga std -full-screen

run-debug: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) in $(_LRED)debug mode$(_LWHITE) !\n"
	@qemu-system-i386 -smp 1 -kernel isodir/boot/$(BIN) -s -S -display gtk -vga std -full-screen

.PHONY: run run-sdl run-iso run-curses run-debug