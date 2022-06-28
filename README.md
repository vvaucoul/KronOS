<!-- <center>
<img
    style="display: block; margin-left: none; margin-right: none; width: 75%"
    src="assets/KronOS-Logo.png"
img/>
</center> -->

# Kronos

<strong>KronOS</strong> is a lightweight Unix-like kernel writen in C and ASM.
This is the result of the <strong>KFS</strong> projects of School 42.
The kernel is currently not finished. I will make improvements as I go along

---

### Prerequisites

* <strong>GCC</strong> (https://gcc.gnu.org/)
* <strong>KVM</strong> (https://www.linux-kvm.org/)
* <strong>QEMU</strong> (https://qemu.org/)

---

### KFS - SERIES

- <strong>KFS-1</strong> : Grub / Boot and Screen
- <strong>KFS-2</strong> : GDT & Stack
- <strong>KFS-3</strong> : Memory
- <strong>KFS-4</strong> : Interrupts
- <strong>KFS-5</strong> : Processes
- <strong>KFS-6</strong> : Filesystem
- <strong>KFS-7</strong> : Syscalls, Sockets & env
- <strong>KFS-8</strong> : Kernel Modules
- <strong>KFS-9</strong> : ELF Parser
- <strong>KFS-X</strong> : Complete Unix System

---

### Compilation

```bash
$ make
```

### Running

```bash
$ make run
```

### Features

- [x] Basic Kernel System
- [x] Boot via Grub
- [x] ASM Bootable base
- [x] Kernel Library with basics functions and types 
- [x] Keyboard Support
- [x] VGA Support
- [x] Scroll and Cursor support
- [x] Colors support

- [ ] A complete GDT system
- [ ] A complete Shell system

### GRUB 2

<p align="center">
<img src="https://user-images.githubusercontent.com/66129673/176182052-630d5111-d968-4858-b01f-0a9beee57b4a.png" width="80%">
</p>

### The KERNEL itself

<p align="center">
<img src="https://user-images.githubusercontent.com/66129673/176182166-39fc1c60-7502-4f3a-878e-e8c7e3babe74.png" width="80%">
</p>