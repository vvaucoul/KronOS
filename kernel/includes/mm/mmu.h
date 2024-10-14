/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:29:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/02 11:18:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MMU_H
#define MMU_H

#include <asm/asm.h>
#include <kernel.h>
#include <stdint.h>
#include <system/isr.h>
#include <system/pit.h>

#define PAGE_ENTRIES 0x400 // 1024

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

typedef struct s_page {
	uint32_t present : 1;  // Page present in memory
	uint32_t rw : 1;	   // Read-only if clear, readwrite if set
	uint32_t user : 1;	   // Supervisor level only if clear
	uint32_t accessed : 1; // Has the page been accessed since last refresh?
	uint32_t dirty : 1;	   // Has the page been written to since last refresh?
	uint32_t nx : 1;	   // No execute: If set, instruction fetches are not allowed
	uint32_t unused : 6;   // Amalgamation of unused and reserved bits
	uint32_t frame : 20;   // Frame address (shifted right 12 bits)
} page_t;

typedef struct s_page_table {
	page_t pages[PAGE_ENTRIES]; // Array of page entries
} page_table_t;

typedef struct s_page_directory {
	page_table_t *tables[PAGE_ENTRIES];	   // Array of pointers to page tables
	uint32_t tablesPhysical[PAGE_ENTRIES]; // Array of physical addresses of page tables
	uint32_t physicalAddr;				   // Physical address of the page directory
} page_directory_t;

int mmu_init(void);

page_directory_t *mmu_get_current_directory(void);
page_directory_t *mmu_get_kernel_directory(void);
void mmu_set_current_directory(page_directory_t *dir);

page_t *mmu_get_page(uint32_t address, page_directory_t *dir);
page_t *mmu_create_page(uint32_t address, page_directory_t *dir);

void mmu_destroy_page_directory(page_directory_t *dir);
void mmu_destroy_page(uint32_t address, page_directory_t *dir);

page_directory_t *mmu_clone_page_directory(page_directory_t *src);

uint32_t mmu_get_physical_address(uint32_t virtual_address);
uint32_t mmu_get_virtual_address(uint32_t physical_address);

void mmu_switch_page_directory(page_directory_t *dir);

void load_page_directory(void *page_directory);
void switch_page_directory(void *page_directory);
void copy_page_physical(uint32_t, uint32_t);
void enable_paging(void *page_directory);

void mmu_page_fault_handler(struct regs *r);
int mmu_is_paging_enabled();

void mmu_flush_tlb();
void mmu_flush_tlb_entry(uint32_t address);

void mmu_set_nx_bit(uint32_t address, int enable);
void mmu_protect_region(uint32_t address, uint32_t size, int permissions);

#endif /* !MMU_H */