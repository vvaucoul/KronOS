/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_builtins.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 01:12:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 14:21:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh.h>
#include <shell/builtins/builtins.h>
#include <system/gdt.h>
#include <multiboot/multiboot.h>
#include <system/sections.h>
#include <system/cpu.h>

#include <cmds/ps.h>

#include <drivers/keyboard.h>

#include <workflows/workflows.h>

KshBuiltins __ksh_builtins[__NB_BUILTINS_];

static uint8_t __current_index = 0;

static void __ksh_help(void)
{
    printk("KSH builtins:\n");
    printk("- " _GREEN "help" _END ": display this help\n");
    printk("- " _GREEN "clear" _END ": clear the screen\n");
    printk("- " _GREEN "reboot" _END ": reboot kernel\n");
    printk("- " _GREEN "poweroff" _END "/" _GREEN "halt" _END "/" _GREEN "shutdown" _END ": shutdown kernel\n");
    printk("- " _GREEN "stack" _END "/" _GREEN "print-stack" _END ": display kernel stack (gdt)\n");
    printk("- " _GREEN "sections" _END ": display kernel sections\n");
    printk("- " _GREEN "mboot" _END "/" _GREEN "multiboot" _END ": display multiboot info\n");
    printk("- " _GREEN "kmmap" _END ": display kernel memory info\n");
    printk("- " _GREEN "setxkbmap" _END ": set keyboard layout\n");
    printk("- " _GREEN "cpuinfos" _END ": display cpu infos\n");
    printk("- " _GREEN "ps" _END ": display process infos\n");
}

static void __add_builtin(char *names[__BUILTINS_MAX_NAMES], void *fn)
{
    for (uint8_t i = 0; names[i] && i < __BUILTINS_MAX_NAMES; i++)
    {
        if (names[i][0] == 0)
            break;
        memcpy(__ksh_builtins[__current_index].names[i], names[i], strlen(names[i]));
    }
    __ksh_builtins[__current_index].function = fn;
    ++__current_index;
}

void __ksh_init_builtins(void)
{
    bzero(__ksh_builtins, sizeof(__ksh_builtins));

    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"clear", ""}, &ksh_clear);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"poweroff", "halt", "shutdown", ""}, &poweroff);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"reboot", ""}, &reboot);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"print-stack", "stack", ""}, &print_stack);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"print-gdt", "gdt", ""}, &print_gdt);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"gdt-test", ""}, &gdt_test);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"time", ""}, &timer_display_ktimer);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"mboot", "multiboot", ""}, &display_multiboot_infos);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"sections", ""}, &display_sections);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"help", ""}, &__ksh_help);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"kmmap", ""}, &display_kernel_memory_map);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"setxkbmap", ""}, &setxkbmap);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"cpuinfos", ""}, &get_cpu_informations);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"ps", ""}, &ps);
}

void __ksh_execute_builtins(const ksh_args_t *arg)
{
    if (arg == NULL)
        return;

    for (uint8_t i = 0; i < __NB_BUILTINS_; i++)
    {
        for (uint8_t j = 0; j < __BUILTINS_MAX_NAMES; j++)
        {
            if (__ksh_builtins[i].names[j] == NULL)
                continue;
            if (strcmp(arg->cmd, __ksh_builtins[i].names[j]) == 0)
            {
                __ksh_builtins[i].function(arg);
                return;
            }
        }
    }
    printk("       Unknown command: %s\n", arg->cmd);
}