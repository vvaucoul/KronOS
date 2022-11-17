# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    QEMU-Runner.mk                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/29 16:45:20 by vvaucoul          #+#    #+#              #
#    Updated: 2022/10/20 13:52:46 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

GLOBAL_QEMU_FLAGS	=	-smp 1 -m 4G -cpu kvm32

DISK_NAME			=	$(NAME).img
DISK_SIZE			=	256M

#******************************************************************************#
#*                         START KERNEL WITH KVM/QEMU                         *#
#******************************************************************************#

run: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) !\n"
	@qemu-system-i386 $(GLOBAL_QEMU_FLAGS) -kernel isodir/boot/$(BIN) -display gtk -vga std -full-screen 
 
run-sdl: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) !\n"
	@qemu-system-i386 $(GLOBAL_QEMU_FLAGS) -kernel isodir/boot/$(BIN) -display sdl -vga std -full-screen 

run-iso: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)cdrom$(_LWHITE) !\n"
	@qemu-system-i386 $(GLOBAL_QEMU_FLAGS) -cdrom $(NAME).iso -display gtk -boot d -vga std -full-screen

run-curses: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)cdrom$(_LWHITE) !\n"
	@qemu-system-i386 $(GLOBAL_QEMU_FLAGS) -cdrom $(NAME).iso -display curses -vga std -full-screen

run-debug: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) in $(_LRED)debug mode$(_LWHITE) !\n"
	@qemu-system-i386 $(GLOBAL_QEMU_FLAGS) --enable-kvm -kernel isodir/boot/$(BIN) -s -S -display gtk -vga std -serial file:serial.log

run-disk: $(NAME) $(DISK_NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)qemu-system-i386$(_LWHITE) with disk: $(_LYELLOW)$(DISK_NAME)$(_LWHITE) !\n"
	@qemu-system-i386 $(GLOBAL_QEMU_FLAGS) -boot order=c -cdrom $(NAME).iso -drive file=$(DISK_NAME),format=raw -display gtk -vga std -full-screen

clean-disk:
	@rm -rf $(DISK_NAME)

$(DISK_NAME):
	@printf "\n- $(_LWHITE)Creating disk: $(_LYELLOW)$(DISK_NAME)$(_LWHITE) for $(_LYELLOW)KFS$(_LWHITE) !\n"
	@sh ./scripts/kvm-create-disk.sh $(DISK_NAME) $(DISK_SIZE) .

.PHONY: run run-sdl run-iso run-curses run-debug run-disk clean-disk