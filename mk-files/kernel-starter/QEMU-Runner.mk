# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    QEMU-Runner.mk                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/29 16:45:20 by vvaucoul          #+#    #+#              #
#    Updated: 2024/07/30 01:14:45 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

GLOBAL_QEMU_FLAGS	=	-smp 4 -m 4G -cpu kvm32 -machine type=pc -k en-us -rtc base=localtime

ifeq ($(CHECK_USE_KVM), false)
	QEMU			:=	qemu-system-i386
else
	QEMU			:=	kvm
endif

# QEMU_DISK_FLAGS		=	-drive file=$(HDD_PATH)/$(HDD_FILENAME),format=qcow2,if=ide,index=0,media=disk
QEMU_DISK_FLAGS		=	 -drive file=$(HDA_PATH)/$(HDA_FILENAME),format=raw,if=ide,index=0,media=disk \
						-drive file=$(HDB_PATH)/$(HDB_FILENAME),format=raw,if=ide,index=1,media=disk
INITRD_FLAGS 		=	-initrd $(INITRD_DIR)/$(INITRD)

#******************************************************************************#
#*                         START KERNEL WITH KVM/QEMU                         *#
#******************************************************************************#

run: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)$(QEMU)$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) !\n"
	@$(QEMU) $(GLOBAL_QEMU_FLAGS) -kernel isodir/boot/$(BIN) -full-screen -vga vmware
 
run-sdl: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)$(QEMU)$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) !\n"
	@$(QEMU) $(GLOBAL_QEMU_FLAGS) -kernel isodir/boot/$(BIN) -display sdl -vga vmware -full-screen 

run-iso: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)$(QEMU)$(_LWHITE) with $(_LYELLOW)cdrom$(_LWHITE) !\n"
	@$(QEMU) $(GLOBAL_QEMU_FLAGS) -cdrom $(NAME).iso -display gtk -boot d -vga vmware -full-screen

run-curses: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)$(QEMU)$(_LWHITE) with $(_LYELLOW)cdrom$(_LWHITE) !\n"
	@$(QEMU) $(GLOBAL_QEMU_FLAGS) -cdrom $(NAME).iso -display curses -vga vmware -full-screen

run-debug: $(NAME)
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)$(QEMU)$(_LWHITE) with $(_LYELLOW)kernel$(_LWHITE) in $(_LRED)debug mode$(_LWHITE) !\n"
	@x-terminal-emulator -e gdb -q -x scripts/gdb-commands.txt 
	@$(QEMU) $(GLOBAL_QEMU_FLAGS) -kernel isodir/boot/$(BIN) -s -S -serial file:serial.log -vga vmware

run-iso-disk: $(NAME) hda hdb ext2 initrd
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)$(QEMU)$(_LWHITE) with $(_LYELLOW)cdrom$(_LWHITE) and $(_LYELLOW)disk$(_LWHITE): $(_LYELLOW)$(DISK_NAME)$(_LWHITE) !\n"
	@$(QEMU) $(GLOBAL_QEMU_FLAGS) -cdrom $(NAME).iso -boot order=cd $(QEMU_DISK_FLAGS) -full-screen -vga vmware

run-disk: $(NAME) hda hdb ext2 initrd
	@printf "$(_LWHITE)Running $(_LYELLOW)KFS$(_LWHITE) with $(_LYELLOW)$(QEMU)$(_LWHITE) with disk: $(_LYELLOW)$(DISK_NAME)$(_LWHITE) !\n"
	@$(QEMU) $(GLOBAL_QEMU_FLAGS) -boot order=c -kernel isodir/boot/$(BIN) $(INITRD_FLAGS) $(QEMU_DISK_FLAGS) -full-screen -vga vmware

.PHONY: run run-sdl run-iso run-curses run-debug run-disk clean-disk