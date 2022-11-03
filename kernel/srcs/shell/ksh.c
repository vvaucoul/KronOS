/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 14:40:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/28 13:14:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh.h>
#include <shell/ksh_builtins.h>
#include <system/gdt.h>
#include <multiboot/multiboot.h>
#include <system/sections.h>

size_t ksh_max_line = VGA_HEIGHT - __HEADER_HEIGHT__;
size_t ksh_min_line = __HEADER_HEIGHT__;
size_t ksh_current_line = __HEADER_HEIGHT__;
size_t ksh_current_max_line = __HEADER_HEIGHT__;

uint32_t *ksh_buffer = (uint32_t *)(0x00002000 + __HIGHER_HALF_KERNEL__ ? KERNEL_MEMORY_START + 0x0000B000 : 0x0);

void ksh_clear(void)
{
    KSH_CLR_TERM_SH();
    CLEAR_SCREEN();
    ksh_current_line = __HEADER_HEIGHT__;
    ksh_current_max_line = __HEADER_HEIGHT__;
    ksh_min_line = __HEADER_HEIGHT__;
}

void ksh_execute_command(void)
{
    /* SIMPLE COMMAND EXECUTOR */
    char __formated_command[128];
    kstrclr(__formated_command, ksh_line_buffer);
    if (__formated_command[0] != 0)
    {
        __ksh_execute_builtins(__formated_command);
        ksh_add_line_history(__formated_command);
    }
    ksh_buffer_clear();
    terminal_column = 0;
    DISPLAY_PROMPT();
    terminal_column = __PROMPT_ASCII_LEN__;
    UPDATE_CURSOR();
}

void kronos_shell(void)
{
    // kprintf("KSHBuffer ADDR : 0x%x\n", ksh_buffer);
    ksh_init();
    __ksh_init_builtins();
    kprintf("Welcome to " COLOR_RED "KSH" COLOR_END " !\n");
    DISPLAY_PROMPT();
    UPDATE_CURSOR();

    while (1)
        ;
}

#undef __PROMPT__
#undef __PROMPT_LEN__
#undef __PROMPT_ASCII_LEN__
#undef __HEADER_WIDTH__
#undef __HEADER_HEIGHT__

#undef KSH_CLR_TERM_SH
#undef KSH_UPDATE_TERM_SH