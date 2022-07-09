/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 14:40:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/07/09 12:13:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh.h>
#include <system/gdt.h>

size_t ksh_max_line = VGA_HEIGHT - __HEADER_HEIGHT__;
size_t ksh_min_line = __HEADER_HEIGHT__;
size_t ksh_current_line = __HEADER_HEIGHT__;
size_t ksh_current_max_line = __HEADER_HEIGHT__;

uint16_t *ksh_buffer;

void ksh_execute_command(void)
{
    /* SIMPLE COMMAND EXECUTOR */
    char *command = ksh_line_buffer;

    if (kstrcmp(command, "clear") == 0)
    {
        KSH_CLR_TERM_SH();
        CLEAR_SCREEN();
        ksh_current_line = __HEADER_HEIGHT__;
        ksh_current_max_line = __HEADER_HEIGHT__;
        ksh_min_line = __HEADER_HEIGHT__;
    }
    else if (kstrcmp(command, "halt") == 0 || kstrcmp(command, "poweroff") == 0 || kstrcmp(command, "shutdown") == 0)
        poweroff();
    else if (kstrcmp(command, "reboot") == 0)
        reboot();
    else if (kstrcmp(command, "print-stack") == 0 || kstrcmp(command, "stack") == 0)
        print_stack();
    else if (kstrcmp(command, "print-gdt") == 0 || kstrcmp(command, "gdt") == 0)
        print_gdt();
    else if (kstrcmp(command, "gdt-test") == 0)
        gdt_test();
    else if (kstrcmp(command, "time") == 0)
        timer_display_ktimer();
    else if (command[0] != '\0')
        kprintf("       Unknown command: %s\n", command);

    ksh_buffer_clear();
    terminal_column = 0;
    DISPLAY_PROMPT();
    terminal_column = __PROMPT_ASCII_LEN__;
    UPDATE_CURSOR();
}

void kronos_shell(void)
{
    ksh_init();
    DISPLAY_PROMPT();
    UPDATE_CURSOR();

    while (1)
    {
        /* Remplacer le systeme par:
            - a chaque touche appuyer, set variable char c dans le fichier keyboard .h et lire la variable dans le while 1
            remetre ensuite la variable à 0,
    */
        // kprintf("Line: %s\n", line);
    }
}

#undef __PROMPT__
#undef __PROMPT_LEN__
#undef __PROMPT_ASCII_LEN__
#undef __HEADER_WIDTH__
#undef __HEADER_HEIGHT__

#undef KSH_CLR_TERM_SH
#undef KSH_UPDATE_TERM_SH