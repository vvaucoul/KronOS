/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   panic.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/09 12:27:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 13:56:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/panic.h>

static void __panic_kerrno(void)
{
    printk("\n");
    __DISPLAY_HEADER__();
    printk(_YELLOW"\nKerrno - Subsystem "_CYAN"[0]\n"_END);
    printk(_RED"Error:"_END" "_END""_CYAN"%d"_END"\n"_END, KerrnoCode());
    printk(_RED"Sector:"_END" "_END""_CYAN"%x"_END"\n"_END, KerrnoCodeSector());
    printk(_RED"Message:"_END" "_END""_CYAN"%s"_END""_END"\n" _END, KerrnoMessage());
    
    printk("\n");
    printk(_RED"File:"_END" "_END""_CYAN"%s"_END""_END"\n" _END, KerrnoFile());
    printk(_RED"Function:"_END" "_END""_CYAN"%s"_END""_END"\n" _END, KerrnoFunction());

    printk("\nYou must restart the system.\n");
    __PANIC_LOOP_HANDLER__();
}

static void __panic_multiple_kerrno(const char *str[], size_t count)
{
    size_t i = 0;
    __DISPLAY_HEADER__();
    printk(_YELLOW"\nKerrno - Subsystem "_CYAN"[0]\n"_END);
    printk(_RED"Error:"_END" "_END""_CYAN"%d"_END"\n"_END, KerrnoCode());
    printk(_RED"Sector:"_END" "_END""_CYAN"%x"_END"\n"_END, KerrnoCodeSector());
    printk(_RED"Message:"_END" "_END""_CYAN"%s"_END""_END"\n" _END, KerrnoMessage());
    
    printk("\n");
    printk(_RED"File:"_END" "_END""_CYAN"%s"_END""_END"\n" _END, KerrnoFile());
    printk(_RED"Function:"_END" "_END""_CYAN"%s"_END""_END"\n" _END, KerrnoFunction());
    while (str[i] && i < count)
    {
        printk(_YELLOW "%s" _END "\n", str[i]);
        i++;
    }
    printk("\nYou must restart the system.\n");
    __PANIC_LOOP_HANDLER__();
}

extern void kernel_panic(const char *str)
{
    if (__USE_KERRNO_HELPER__)
        __panic_kerrno();
 
    __DISPLAY_HEADER__();
    printk(_YELLOW "%s\n" _END, str);
    __PANIC_LOOP_HANDLER__();
}
extern void kernel_panic_multistr(const char *str[], size_t count)
{
    if (__USE_KERRNO_HELPER__)
        __panic_multiple_kerrno(str, count);
 
    size_t i = 0;
    __DISPLAY_HEADER__();
    while (str[i] && i < count)
    {
        printk(_YELLOW "%s" _END "\n", str[i]);
        i++;
    }
    __PANIC_LOOP_HANDLER__();
}
