/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isr.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:16:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 23:17:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <asm/asm.h>
#include <stdio.h>
#include <system/irq.h>
#include <system/isr.h>
#include <system/kerrno.h>
#include <system/serial.h>

static InterruptHandler isr_interrupt_handlers[NB_INTERRUPT_HANDLERS] = {0};
static irqs_t irqs[ISR_MAX_COUNT] = {0};

const char *exception_messages[ISR_MAX_COUNT] = {
	"Division By Zero", "Debug", "Non Maskable Interrupt", "Breakpoint Exception",
	"Into Detected Overflow Exception", "Out of Bounds Exception", "Invalid Opcode Exception",
	"No Coprocessor Exception", "Double Fault Exception", "Coprocessor Segment Overrun Exception",
	"Bad TSS Exception", "Segment Not Present Exception", "Stack Fault Exception",
	"General Protection Fault Exception", "Page Fault Exception", "Unknown Interrupt Exception",
	"Coprocessor Fault Exception", "Alignment Check Exception", "Machine Check Exception",
	"Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
	"Reserved", "Reserved", "Reserved", "Reserved"};

/**
 * Interrupt Service Routine (ISR) functions.
 *
 * Each ISR function is responsible for handling a specific interrupt.
 * The ISR functions are stored in an array of function pointers.
 *
 * @note The ISR functions are defined in the file `isr_handlers.s`.
 *
 */
const void *isr_fn[ISR_MAX_COUNT] = {
	isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7,
	isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15,
	isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
	isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31};

/**
 * Registers an Interrupt Service Routine (ISR) handler.
 *
 * @param index The index of the ISR.
 * @param name The name of the ISR.
 * @param code The code to be executed when the ISR is triggered.
 * @param type The type of panic to be triggered if the ISR encounters an error.
 * @param exception The exception associated with the ISR.
 * @param zero A flag indicating if the ISR should be zeroed out.
 * @param has_code A flag indicating if the ISR has code associated with it.
 */
static void isr_register(uint8_t index, const char *name, isr_code_t code, panic_t type, const char *exception, bool zero, bool has_code) {
	irqs[index].name = name;
	irqs[index].code = code;
	irqs[index].type = type;
	irqs[index].exception = exception;
	irqs[index].zero = zero;
	irqs[index].has_code = has_code;
}

/**
 * @brief Handles the display interrupt frame.
 *
 * This function is responsible for handling the interrupt frame for display interrupts.
 * It takes a pointer to the interrupt registers as a parameter.
 * The interrupt frame contains the state of the CPU registers at the time of the interrupt.
 * This function does not save any caller-saved registers.
 *
 * @param r Pointer to the interrupt registers.
 */
#if defined(__GNUC__)
static void isr_display_interrupt_frame(t_regs *r) {
#elif defined(__clang__)
static __attribute__((no_caller_saved_registers)) void isr_display_interrupt_frame(t_regs *r) {
#endif

	/**
	 * Function with attribute 'no_caller_saved_registers' should only call a function with attribute 'no_caller_saved_registers'
	 * Check: -mgeneral-regs-only
	 */

	printk(_END "\nInterrupt Frame:\n");
	printk("------------------------------------------------\n");
	printk("General Purpose Registers:\n");
	printk("  EAX: 0x%08x  EBX: 0x%08x  ECX: 0x%08x  EDX: 0x%08x\n", r->eax, r->ebx, r->ecx, r->edx);
	printk("  EDI: 0x%08x  ESI: 0x%08x  EBP: 0x%08x  ESP: 0x%08x\n", r->edi, r->esi, r->ebp, r->esp);
	printk("\n");
	printk("Segment Registers:\n");
	printk("  GS: 0x%08x  FS: 0x%08x  ES: 0x%08x  DS: 0x%08x\n", r->gs, r->fs, r->es, r->ds);
	printk("\n");
	printk("Control Registers:\n");
	printk("  EIP: 0x%08x  CS: 0x%08x  SS: 0x%08x  EFLAGS: 0x%08x\n", r->eip, r->cs, r->ss, r->eflags);
	printk("  User ESP: 0x%08x\n", r->useresp);
	printk("------------------------------------------------\n");
	printk(_END);

	qemu_printf(_END "\nInterrupt Frame:\n");
	qemu_printf("------------------------------------------------\n");
	qemu_printf("General Purpose Registers:\n");
	qemu_printf("  EAX: 0x%08x  EBX: 0x%08x  ECX: 0x%08x  EDX: 0x%08x\n", r->eax, r->ebx, r->ecx, r->edx);
	qemu_printf("  EDI: 0x%08x  ESI: 0x%08x  EBP: 0x%08x  ESP: 0x%08x\n", r->edi, r->esi, r->ebp, r->esp);
	qemu_printf("\n");
	qemu_printf("Segment Registers:\n");
	qemu_printf("  GS: 0x%08x  FS: 0x%08x  ES: 0x%08x  DS: 0x%08x\n", r->gs, r->fs, r->es, r->ds);
	qemu_printf("\n");
	qemu_printf("Control Registers:\n");
	qemu_printf("  EIP: 0x%08x  CS: 0x%08x  SS: 0x%08x  EFLAGS: 0x%08x\n", r->eip, r->cs, r->ss, r->eflags);
	qemu_printf("  User ESP: 0x%08x\n", r->useresp);
	qemu_printf("------------------------------------------------\n");
	qemu_printf(_END);
}

/**
 * @brief Sets up an Interrupt Service Routine (ISR) gate.
 *
 * This function is responsible for setting up an ISR gate for a specific interrupt index.
 * It takes the ISR function pointer, name, code, type, exception, zero, and has_code as parameters.
 *
 * @param index The interrupt index.
 * @param isr_fn The ISR function pointer.
 * @param name The name of the ISR gate.
 * @param code The ISR code.
 * @param type The panic type.
 * @param exception The exception name.
 * @param zero A boolean indicating if the ISR gate should be zeroed.
 * @param has_code A boolean indicating if the ISR gate has code.
 */
static void setup_isr_gate(int index, void *isr_fn, const char *name, isr_code_t code, panic_t type, const char *exception, bool zero, bool has_code) {
	idt_set_gate(index, (uint32_t)(uintptr_t)isr_fn, IDT_SELECTOR, IDT_FLAG_GATE);
	isr_register(index, name, code, type, exception, zero, has_code);
}

/**
 * @brief Installs the Interrupt Service Routine (ISR).
 *
 * This function is responsible for installing the Interrupt Service Routine (ISR).
 * It sets up the necessary configurations to handle interrupts.
 *
 * @note This function should be called before enabling interrupts.
 */
void isr_install(void) {
	for (int i = 0; i < ISR_MAX_COUNT; ++i) {
		setup_isr_gate(i, (void *)isr_fn[i], exception_messages[i], i, FAULT, exception_messages[i], false, false);
	}
}

/**
 * Registers an interrupt handler for a specific interrupt number.
 *
 * @param num The interrupt number to register the handler for.
 * @param handler The function pointer to the interrupt handler.
 * @return 0 if the registration was successful, -1 otherwise.
 */
int isr_register_interrupt_handler(int num, InterruptHandler handler) {
	if (num < 0 || num >= ISR_MAX_COUNT) {
		return (1);
	}
	idt_set_gate(num, (unsigned int)(uintptr_t)handler, IDT_SELECTOR, IDT_FLAG_GATE);
	isr_interrupt_handlers[num] = handler;

	return (0);
}

/**
 * @brief Handles faults in the system.
 *
 * This function is responsible for handling faults in the system. It takes a pointer to a `struct regs` as a parameter.
 * The `struct regs` contains the register values at the time of the fault.
 *
 * @param r Pointer to a `struct regs` containing the register values at the time of the fault.
 */
void fault_handler(struct regs *r) {
	uint8_t err_code = 0x0;
	r->int_no &= 0xFF; /* Extend 8-bit interrupts to 32-bit */

	if (r->int_no < ISR_MAX_COUNT) {
		isr_display_interrupt_frame(r);
		panic_t type = irqs[r->int_no].type;
		bool has_code = irqs[r->int_no].has_code;
		bool zero = irqs[r->int_no].zero;

		if (!zero && has_code)
			err_code = r->err_code;

		pic8259_send_eoi(r->int_no);
		__PANIC_INTERRUPT(irqs[r->int_no].name, r->int_no, type, err_code);

		if (isr_interrupt_handlers[r->int_no] != NULL)
			isr_interrupt_handlers[r->int_no](r);
	} else {
		isr_display_interrupt_frame(r);
		__PANIC_INTERRUPT("Unhandled Interrupt", r->int_no, ABORT, r->err_code);
	}
}
