/*
+--------------------+------------+------------+------------+-------+
| Error              | Code       | Type       | Exception  | ERR C |
+--------------------+------------+------------+------------+-------+
| Divide-by-zero     | 0 (0x0)    | Fault      | #DE        | No    |
| Debug              | 1 (0x1)    | Fault/Trap | #DB        | No    |
| Non-maskable       | 2 (0x2)    | Interrupt  | -          | No    |
| Breakpoint         | 3 (0x3)    | Trap       | #BP        | No    |
| Overflow           | 4 (0x4)    | Trap       | #OF        | No    |
| Bound Range Exceed | 5 (0x5)    | Fault      | #BR        | No    |
| Invalid Opcode     | 6 (0x6)    | Fault      | #UD        | No    |
| Device Not         | 7 (0x7)    | Fault      | #NM        | No    |
| Double Fault       | 8 (0x8)    | Abort      | #DF        | Yes(Z)|
| Coprocessor        | 9 (0x9)    | Fault      | -          | No    |
| Invalid TSS        | 10 (0xA)   | Fault      | #TS        | Yes   |
| Segment Not        | 11 (0xB)   | Fault      | #NP        | Yes   |
| Stack-Segment      | 12 (0xC)   | Fault      | #SS        | Yes   |
| General Protection | 13 (0xD)   | Fault      | #GP        | Yes   |
| Page Fault         | 14 (0xE)   | Fault      | #PF        | Yes   |
| Reserved           | 15 (0xF)   | -          | -          | No    |
| x87 Floating-Point | 16 (0x10)  | Fault      | #MF        | No    |
| Alignment Check    | 17 (0x11)  | Fault      | #AC        | Yes   |
| Machine Check      | 18 (0x12)  | Abort      | #MC        | No    |
| SIMD Floating-Point| 19 (0x13)  | Fault      | #XM/#XF    | No    |
| Virtualization     | 20 (0x14)  | Fault      | #VE        | No    |
| Control Protection | 21 (0x15)  | Fault      | #CP        | Yes   |
| Reserved           | 22-27 (0x16| -          | -          | No    |
| Hypervisor         | 28 (0x1C)  | Fault      | #HV        | No    |
| VMM Communication  | 29 (0x1D)  | Fault      | #VC        | Yes   |
| Security           | 30 (0x1E)  | Fault      | #SX        | Yes   |
| Reserved           | 31 (0x1F)  | -          | -          | No    |
| Triple Fault       | -          | -          | -          | No    |
| FPU Error Interrupt| IRQ 13     | Interrupt  | #FERR      | No    |
+--------------------+------------+------------+------------+-------+
*/

/* Interrupt Service Routine */

/*
	P: 1bit, DPL: 2bits, S: 1bit, TYPE: 4bits

	- P: Present
	- DPL: Descriptor Privilege Level
	- S: Storage Segment
	- TYPE: Gate Type

	Interrupt Gate (0xE) : 32bit Interrupt Handler
	Trap Gate (0xF) : 32bit Interrupt Handler, with IF=1
	Task Gate (0x5) : 32bit TSS Selector
	Call Gate (0xC) : 32bit Interrupt Handler, with IF=0
*/