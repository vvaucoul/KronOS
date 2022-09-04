/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 14:40:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/05 01:47:54 by vvaucoul         ###   ########.fr       */
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

uint32_t *ksh_buffer = (uint32_t *)(0x00002000 + KERNEL_MEMORY_START);

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
    __ksh_execute_builtins(__formated_command);
    /*
        if (kstrcmp(__formated_command, "clear") == 0)
        {
            KSH_CLR_TERM_SH();
            CLEAR_SCREEN();
            ksh_current_line = __HEADER_HEIGHT__;
            ksh_current_max_line = __HEADER_HEIGHT__;
            ksh_min_line = __HEADER_HEIGHT__;
        }
        else if (kstrcmp(__formated_command, "halt") == 0 || kstrcmp(__formated_command, "poweroff") == 0 || kstrcmp(__formated_command, "shutdown") == 0)
            poweroff();
        else if (kstrcmp(__formated_command, "reboot") == 0)
            reboot();
        else if (kstrcmp(__formated_command, "print-stack") == 0 || kstrcmp(__formated_command, "stack") == 0)
            print_stack();
        else if (kstrcmp(__formated_command, "print-gdt") == 0 || kstrcmp(__formated_command, "gdt") == 0)
            print_gdt();
        else if (kstrcmp(__formated_command, "gdt-test") == 0)
            gdt_test();
        else if (kstrcmp(__formated_command, "time") == 0)
            timer_display_ktimer();
        else if (kstrcmp(__formated_command, "mboot") == 0)
            __display_multiboot_infos();
        else if (kstrcmp(__formated_command, "sections") == 0)
            display_sections();
        else if (__formated_command[0] != '\0')
            kprintf("       Unknown command: %s\n", __formated_command);
    */
    ksh_buffer_clear();
    terminal_column = 0;
    DISPLAY_PROMPT();
    terminal_column = __PROMPT_ASCII_LEN__;
    UPDATE_CURSOR();
}

void kronos_shell(void)
{
    ksh_init();
    __ksh_init_builtins();
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