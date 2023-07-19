/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 14:40:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/06/02 19:09:44 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh.h>
#include <shell/ksh_args.h>
#include <shell/ksh_builtins.h>

#include <memory/kheap.h>

#include <multitasking/process.h>

uint32_t ksh_max_line = VGA_HEIGHT - __HEADER_HEIGHT__;
uint32_t ksh_min_line = __HEADER_HEIGHT__;
uint32_t ksh_current_line = __HEADER_HEIGHT__;
uint32_t ksh_current_max_line = __HEADER_HEIGHT__;

uint32_t *ksh_buffer = (uint32_t *)0x0000B000;

void ksh_clear(void) {
    KSH_CLR_TERM_SH();
    CLEAR_SCREEN();
    ksh_current_line = __HEADER_HEIGHT__;
    ksh_current_max_line = __HEADER_HEIGHT__;
    ksh_min_line = __HEADER_HEIGHT__;
}

void ksh_execute_command(void) {
    /* SIMPLE COMMAND EXECUTOR */
    char __formated_command[128];

    bzero(__formated_command, 128);
    strclr(__formated_command, ksh_line_buffer);
    if (__formated_command[0] != 0) {
        const ksh_args_t *args = ksh_parse_args(__formated_command);
        __ksh_execute_builtins(args);
        ksh_add_line_history(__formated_command);
        ksh_free_args(args);
    }
    ksh_buffer_clear();
    terminal_column = 0;
    DISPLAY_PROMPT();
    terminal_column = __PROMPT_ASCII_LEN__;
    UPDATE_CURSOR();
}

void kronos_shell(void) {
    ksh_init();
    __ksh_init_builtins();
    printk("\nCreate Shell at "_GREEN
           "[0x%x]"_END
           " with PID: "_GREEN
           "[%d]"_END
           "\n",
           ksh_buffer, getpid());
    printk("Welcome to " _RED "KSH" _END " !\n");
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