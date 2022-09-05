# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Kernel-Maker.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/29 15:43:33 by vvaucoul          #+#    #+#              #
#    Updated: 2022/09/05 02:21:18 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

GRUB_CONFIG_DIR = config
LINKER_DIR = linkers

ifeq ($(CHECK_XORRISO_INSTALL), false)
	XORRISO_PATH = $(shell pwd)/$(XORRISO)/xorriso/xorriso
else
	XORRISO_PATH = /bin/xorriso
endif
	
$(BIN_DIR)/$(BIN):
	@mkdir -p $(BIN_DIR)
	@$(LD) $(LD_FLAGS) -T $(LINKER) -o $(BIN_DIR)/$(BIN) $(KBOOT_OBJS) $(KOBJS) $(KOBJS_ASM) $(LIBKFS_A) #> /dev/null 2>&1
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

iso: $(OBJS_ASM) $(OBJS)
	@rm -rf isodir
	@make -s -C . $(ISO)

bin: $(OBJS_ASM) $(OBJS)
	@rm -rf $(BIN_DIR)
	@make -s -C . $(BIN_DIR)/$(BIN)

.PHONY: iso bin $(ISO) $(BIN_DIR)/$(BIN)