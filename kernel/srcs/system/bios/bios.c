/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bios.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/18 19:57:17 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/29 15:56:43 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/bios.h>
#include <system/gdt.h>

#include <system/panic.h>

void (*exec_bios32_function)() = (void *)BIOS_DEFAULT_MEMORY;

static void __bios32_service(uint8_t interrupt, regs16_t *in, regs16_t *out)
{
    void *bios32_service = (void *)BIOS_DEFAULT_MEMORY;

    memcpy(&bios32_gdt_entries, gdt, sizeof(t_gdt_entry));
    gp.base = (uint32_t)REBASE_ADDRESS((&bios32_gdt_entries));
    memcpy(&bios32_gdt_ptr, &gp, sizeof(t_gdt_ptr));
    memcpy(&bios32_idt_ptr, &idtp, sizeof(struct idt_ptr));
    memcpy(&bios32_in_reg16_ptr, in, sizeof(regs16_t));
    void *in_reg16_address = REBASE_ADDRESS(&bios32_in_reg16_ptr);
    memcpy(&bios32_int_number_ptr, &interrupt, sizeof(uint8_t));


    uint32_t size = (uint32_t)BIOS32_END - (uint32_t)BIOS32_START;
    memcpy(bios32_service, BIOS32_START, size);
    if (size > PAGE_SIZE)
        __PANIC("BIOS32 Service is too big !");
    printk("Executing BIOS32 Service at address %p\n", bios32_service);

    kpause();
    exec_bios32_function();

    in_reg16_address = REBASE_ADDRESS(&bios32_out_reg16_ptr);
    memcpy(out, in_reg16_address, sizeof(regs16_t));

    gdt_flush((uint32_t)(&gp));
    idt_load(&idtp);
}

static void __init_bios32()
{
    /* Bios 16 bit code segment */
    gdt_add_entry(7, 0, 0xFFFFFFFF, 0x9A, 0x0F);

    /* Bios 16 bit data segment */
    gdt_add_entry(8, 0, 0xFFFFFFFF, 0x92, 0x0F);

    gp.limit = (sizeof(&gdt[0]) * (__GDT_SIZE + 2)) - 1;
    gp.base = __GDT_ADDR;

    idtp.limit = 0x3ff;
    idtp.base = (unsigned int)&idt;
}

void init_bios32()
{
    __init_bios32();
}

void int86(uint8_t interrupt, regs16_t *in, regs16_t *out)
{
    __bios32_service(interrupt, in, out);
}