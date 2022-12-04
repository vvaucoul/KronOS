/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   panic.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/09 12:27:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/04 13:17:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/panic.h>

static void __panic_kerrno(void)
{
    kprintf("\n");
    __DISPLAY_HEADER__();
    kprintf(COLOR_YELLOW"\nKerrno - Subsystem "COLOR_CYAN"[0]\n"COLOR_END);
    kprintf(COLOR_RED"Error:"COLOR_END" "COLOR_END""COLOR_CYAN"%d"COLOR_END"\n"COLOR_END, KerrnoCode());
    kprintf(COLOR_RED"Sector:"COLOR_END" "COLOR_END""COLOR_CYAN"%x"COLOR_END"\n"COLOR_END, KerrnoCodeSector());
    kprintf(COLOR_RED"Message:"COLOR_END" "COLOR_END""COLOR_CYAN"%s"COLOR_END""COLOR_END"\n" COLOR_END, KerrnoMessage());
    
    kprintf("\n");
    kprintf(COLOR_RED"File:"COLOR_END" "COLOR_END""COLOR_CYAN"%s"COLOR_END""COLOR_END"\n" COLOR_END, KerrnoFile());
    kprintf(COLOR_RED"Function:"COLOR_END" "COLOR_END""COLOR_CYAN"%s"COLOR_END""COLOR_END"\n" COLOR_END, KerrnoFunction());

    kprintf("\nYou must restart the system.\n");
    __PANIC_LOOP_HANDLER__();
}

static void __panic_multiple_kerrno(const char *str[], size_t count)
{
    size_t i = 0;
    __DISPLAY_HEADER__();
    kprintf(COLOR_YELLOW"\nKerrno - Subsystem "COLOR_CYAN"[0]\n"COLOR_END);
    kprintf(COLOR_RED"Error:"COLOR_END" "COLOR_END""COLOR_CYAN"%d"COLOR_END"\n"COLOR_END, KerrnoCode());
    kprintf(COLOR_RED"Sector:"COLOR_END" "COLOR_END""COLOR_CYAN"%x"COLOR_END"\n"COLOR_END, KerrnoCodeSector());
    kprintf(COLOR_RED"Message:"COLOR_END" "COLOR_END""COLOR_CYAN"%s"COLOR_END""COLOR_END"\n" COLOR_END, KerrnoMessage());
    
    kprintf("\n");
    kprintf(COLOR_RED"File:"COLOR_END" "COLOR_END""COLOR_CYAN"%s"COLOR_END""COLOR_END"\n" COLOR_END, KerrnoFile());
    kprintf(COLOR_RED"Function:"COLOR_END" "COLOR_END""COLOR_CYAN"%s"COLOR_END""COLOR_END"\n" COLOR_END, KerrnoFunction());
    while (str[i] && i < count)
    {
        kprintf(COLOR_YELLOW "%s" COLOR_END "\n", str[i]);
        i++;
    }
    kprintf("\nYou must restart the system.\n");
    __PANIC_LOOP_HANDLER__();
}

extern void kernel_panic(const char *str)
{
    if (__USE_KERRNO_HELPER__)
        __panic_kerrno();
 
    __DISPLAY_HEADER__();
    kprintf(COLOR_YELLOW "%s\n" COLOR_END, str);
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
        kprintf(COLOR_YELLOW "%s" COLOR_END "\n", str[i]);
        i++;
    }
    __PANIC_LOOP_HANDLER__();
}
