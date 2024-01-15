# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Kernel-Maker.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/29 15:43:33 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/13 14:10:56 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

GRUB_CONFIG_DIR = kernel/arch/x86/config
LINKER_DIR = kernel/arch/x86/linkers

ifeq ($(CHECK_XORRISO_INSTALL), false)
	XORRISO_PATH = $(shell pwd)/dependencies/$(XORRISO)/xorriso/xorriso
else
	XORRISO_PATH = /bin/xorriso
endif

$(BIN_DIR)/$(BIN):
	@mkdir -p $(BIN_DIR)
	@$(LD) $(LD_FLAGS) -T $(LINKER) -o $(BIN_DIR)/$(BIN) $(KBOOT_OBJS) $(KOBJS) $(KOBJSXX)  $(KOBJS_ASM) $(WOBJS) $(LIBKFS) #> /dev/null 2>&1
	@printf "$(_LWHITE)    $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LYELLOW) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" "KERNEL / LINKER / BOOT" 
	@printf "$(_LWHITE)- KERNEL BIN $(_END)$(_DIM)------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)$(_DIM) -> ISO CREATION $(_END) \n"
	@make -s -C . check

$(ISO):
	@mkdir -p isodir/boot/grub
	@cp $(BIN_DIR)/kernel.bin isodir/boot/kernel.bin
	@cp $(GRUB_CONFIG_DIR)/grub.cfg isodir/boot/grub/grub.cfg
	@printf "$(_LWHITE)    $(_DIM)- Xorriso Path: $(_END)$(_DIM)-----$(_END)$(_LYELLOW) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $(XORRISO_PATH)
	@printf "$(_LWHITE)    $(_DIM)- Generating: $(_END)$(_DIM)-------$(_END)$(_LYELLOW) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" "$(ISO)" 
	@grub-mkrescue -o $(ISO) isodir \
	--xorriso=$(XORRISO_PATH) > /dev/null 2>&1
	@printf "$(_LWHITE)- ISO $(_END)$(_DIM)-------------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

iso: $(NAME)
	@rm -rf isodir
	@make -s -C . $(ISO)

bin: $(NAME)
	@rm -rf $(BIN_DIR)
	@make -s -C . $(BIN_DIR)/$(BIN)

# $(INITRD_DIR)/$(INITRD):
# 	@printf "$(_LWHITE)    $(_DIM)- Generating: $(_END)$(_DIM)-------$(_END)$(_LYELLOW) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" "$(DISK_NAME)"
# 	@cd ./utils/vfs/ext2/ && gcc -o make_initrd vfs_ext2_generator.c
# 	@cd ./utils/vfs/ext2/ && ./make_initrd test.txt test > /dev/null 2>&1
# 	@cd ./utils/vfs/ext2/ && cp $(INITRD) ../../../$(INITRD_DIR)/$(INITRD)

# initrd:
# 	@printf "$(_LWHITE)    $(_DIM)- Generating: $(_END)$(_DIM)-------$(_END)$(_LYELLOW) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" "$(INITRD)"
# 	@mkdir -p $(INITRD_DIR)
# 	@make -s -C . $(INITRD_DIR)/$(INITRD)

# $(DISK_PATH)/$(DISK_NAME): 
# 	@cd ./utils/vfs/ && sh create_image.sh > /dev/null 2>&1

# vfs:
# 	@mkdir -p $(DISK_PATH)
# 	@make -s -C . $(DISK_PATH)/$(DISK_NAME)

initrd:
	@printf "$(_LWHITE)    $(_DIM)- Generating: $(_END)$(_DIM)-------$(_END)$(_LYELLOW) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" "$(INITRD)"
	@cd ./utils/vfs/initrd/ && sh create_initrd.sh && cd - > /dev/null 2>&1
	@printf "$(_LWHITE)- INITRD $(_END)$(_DIM)----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

ata:
	@printf "$(_LWHITE)    $(_DIM)- Generating: $(_END)$(_DIM)-------$(_END)$(_LYELLOW) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" "$(HDD_FILENAME)"
	@qemu-img create -f raw $(HDD_PATH)/$(HDD_FILENAME) $(HDD_SIZE) > .hdd_output.log
	@printf "$(_LWHITE)    $(_DIM)- Log: $(_END)$(_DIM)--------------$(_END)$(_LYELLOW) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" ".hdd_output.log"

.PHONY: iso bin $(ISO) $(BIN_DIR)/$(BIN) vsf initrd