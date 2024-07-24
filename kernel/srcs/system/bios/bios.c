/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bios.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/18 19:57:17 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/23 20:36:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/bios.h>
#include <system/gdt.h>

#include <system/panic.h>

void (*exec_bios32_function)() = (void *)BIOS_DEFAULT_MEMORY;

extern void gdt_flush(uint32_t);
extern void idt_load(struct idt_ptr*);

static void __bios32_service(uint8_t interrupt, regs16_t *in, regs16_t *out) {
    // Sauvegarde des GDT et IDT actuelles
    gdt_ptr_t old_gdt_ptr;
    struct idt_ptr old_idt_ptr;
    __asm__ volatile ("sgdt %0" : "=m" (old_gdt_ptr));
    __asm__ volatile ("sidt %0" : "=m" (old_idt_ptr));

    // Copie des registres d'entrée dans la mémoire basse
    memcpy((void *)BIOS32_REGS16_INPUT, in, sizeof(regs16_t));
    
    // Configuration de l'interruption et des registres
    *((uint8_t *)BIOS32_INT_NUMBER) = interrupt;
    *((uint32_t *)BIOS32_REGS16_INPUT_PTR) = (uint32_t)BIOS32_REGS16_INPUT;
    *((uint32_t *)BIOS32_REGS16_OUTPUT_PTR) = (uint32_t)BIOS32_REGS16_OUTPUT;

    // Exécution du service BIOS
    exec_bios32_function();

    // Copie des registres de sortie depuis la mémoire basse
    memcpy(out, (void *)BIOS32_REGS16_OUTPUT, sizeof(regs16_t));

    // Restauration des GDT et IDT
    gdt_flush((uint32_t)old_gdt_ptr.base);
    idt_load(&old_idt_ptr);
}

static void __init_bios32() {
    // Initialisation des segments GDT pour le code et les données 16 bits
    gdt_add_entry(7, 0, 0xFFFFF, 0x9A, 0x0F); // Segment code 16 bits
    gdt_add_entry(8, 0, 0xFFFFF, 0x92, 0x0F); // Segment data 16 bits

    // Chargement de la nouvelle GDT
    gdt_ptr_t gp;
    gp.limit = (sizeof(gdt_entry_t) * (GDT_SIZE + 2)) - 1;
    gp.base = (uint32_t)&gdt;
    gdt_flush((uint32_t)&gp);

    // Configuration de l'IDT
    struct idt_ptr idtp;
    idtp.limit = 0x3FF;
    idtp.base = (uint32_t)&idt;
    idt_load(&idtp);
}

void init_bios32() {
    __init_bios32();
}

void int86(uint8_t interrupt, regs16_t *in, regs16_t *out) {
    __bios32_service(interrupt, in, out);
}