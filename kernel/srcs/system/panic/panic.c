/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   panic.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/09 12:27:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/08 22:17:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/panic.h>

static void __panic_kerrno(void)
{
    printk("\n");
    __DISPLAY_HEADER__();
    printk(_YELLOW "\nKerrno - Subsystem "_CYAN
                   "[0]\n"_END);
    printk(_RED "Error:"_END
                " "_END
                ""_CYAN
                "%d"_END
                "\n"_END,
           KerrnoCode());
    printk(_RED "Sector:"_END
                " "_END
                ""_CYAN
                "%x"_END
                "\n"_END,
           KerrnoCodeSector());
    printk(_RED "Message:"_END
                " "_END
                ""_CYAN
                "%s"_END
                ""_END
                "\n" _END,
           KerrnoMessage());

    printk("\n");
    printk(_RED "File:"_END
                " "_END
                ""_CYAN
                "%s"_END
                ""_END
                "\n" _END,
           KerrnoFile());
    printk(_RED "Function:"_END
                " "_END
                ""_CYAN
                "%s"_END
                ""_END
                "\n" _END,
           KerrnoFunction());

    printk("\nYou must restart the system.\n");
    __PANIC_LOOP_HANDLER__();
}

static void __panic_multiple_kerrno(const char *str[], size_t count)
{
    size_t i = 0;
    __DISPLAY_HEADER__();
    printk(_YELLOW "\nKerrno - Subsystem "_CYAN
                   "[0]\n"_END);
    printk(_RED "Error:"_END
                " "_END
                ""_CYAN
                "%d"_END
                "\n"_END,
           KerrnoCode());
    printk(_RED "Sector:"_END
                " "_END
                ""_CYAN
                "%x"_END
                "\n"_END,
           KerrnoCodeSector());
    printk(_RED "Message:"_END
                " "_END
                ""_CYAN
                "%s"_END
                ""_END
                "\n" _END,
           KerrnoMessage());

    printk("\n");
    printk(_RED "File:"_END
                " "_END
                ""_CYAN
                "%s"_END
                ""_END
                "\n" _END,
           KerrnoFile());
    printk(_RED "Function:"_END
                " "_END
                ""_CYAN
                "%s"_END
                ""_END
                "\n" _END,
           KerrnoFunction());
    while (str[i] && i < count)
    {
        printk(_YELLOW "%s" _END "\n", str[i]);
        i++;
    }
    printk("\nYou must restart the system.\n");
    __PANIC_LOOP_HANDLER__();
}

void kernel_panic(const char *str)
{
    if (__USE_KERRNO_HELPER__)
        __panic_kerrno();

    __DISPLAY_HEADER__();
    printk(_YELLOW "%s\n" _END, str);
    __PANIC_LOOP_HANDLER__();
}
void kernel_panic_multistr(const char *str[], size_t count)
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

void kernel_fault(const char *str)
{
    __DISPLAY_HEADER_FAULT__();
    printk(_YELLOW "%s\n" _END, str);
}

void kernel_trap(const char *str)
{
    __DISPLAY_HEADER_TRAP__();
    printk(_YELLOW "%s\n" _END, str);
}

void kernel_panic_interrupt(const char *str, uint32_t index, panic_t fault, uint32_t code)
{
    if (__USE_KERRNO_HELPER__)
        __panic_kerrno();

    switch (fault)
    {
    case ABORT:
        __DISPLAY_HEADER__();
        printk(_RED "%s\n" _END, str);
        break;
    case FAULT:
        __DISPLAY_HEADER_FAULT__();
        printk(_END "%s\n" _END, str);
        break;
    case TRAP:
        __DISPLAY_HEADER_TRAP__();
        printk(_CYAN "%s\n" _END, str);
        break;
    case INTERRUPT:
        __DISPLAY_HEADER_INTERRUPT__();
        printk(_YELLOW "%s\n" _END, str);
        break;
    }
    if (fault == ABORT)
        __PANIC_LOOP_HANDLER__();
}
