; TLB : Translation lookaside buffer
; The Translation Lookaside Buffer (TLB) is a cache of memory page translations employed in many systems with memory paging capability. When the processor needs to translate a given virtual address into a physical address, the TLB is consulted first. On x86 systems, TLB misses are handled transparently by hardware. Only if the page directory/table entry is not present in-core will the operating system be notified (by the means of a page fault exception.)

section .text
align 4
global flush_tlb:

flush_tlb:
mov eax, [esp + 4]
invlpg [eax]
ret