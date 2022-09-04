/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_builtins.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 01:12:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/05 01:54:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh.h>
#include <system/gdt.h>
#include <multiboot/multiboot.h>
#include <system/sections.h>

KshBuiltins __ksh_builtins[__NB_BUILTINS_];
static uint8_t __current_index = 0;

static void __add_builtin(char *names[__BUILTINS_MAX_NAMES], void *fn)
{
    for (uint8_t i = 0; names[i] && i < __BUILTINS_MAX_NAMES; i++)
    {
        if (names[i][0] == 0)
            break;
        kmemcpy(__ksh_builtins[__current_index].names[i], names[i], kstrlen(names[i]));
    }
    __ksh_builtins[__current_index].function = fn;
    ++__current_index;
}

void __ksh_init_builtins(void)
{
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"clear", ""}, &ksh_clear);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"poweroff", "halt", "shutdown", ""}, &poweroff);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"reboot", ""}, &reboot);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"print-stack", "stack", ""}, &print_stack);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"print-gdt", "gdt", ""}, &print_gdt);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"gdt-test", ""}, &gdt_test);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"time", ""}, &timer_display_ktimer);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"mboot", ""}, &__display_multiboot_infos);
    __add_builtin((char *[__BUILTINS_MAX_NAMES]){"sections", ""}, &display_sections);
}

void __ksh_execute_builtins(const char *name)
{
    if (!name)
        return;
    else if (!name[0])
        return;

    for (uint8_t i = 0; i < __NB_BUILTINS_; i++)
    {
        for (uint8_t j = 0; j < __BUILTINS_MAX_NAMES; j++)
        {
            if (__ksh_builtins[i].names[j] == NULL)
                continue;
            if (kstrcmp(name, __ksh_builtins[i].names[j]) == 0)
            {
                __ksh_builtins[i].function();
                return;
            }
        }
    }
    kprintf("       Unknown command: %s\n", name);
}