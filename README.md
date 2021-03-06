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
* <strong>CLANG</strong> (https://clang.llvm.org/)
* <strong>KVM</strong> (https://www.linux-kvm.org/)
* <strong>QEMU</strong> (https://qemu.org/)

---

### KFS - SERIES

- [x] <strong>KFS-1</strong> : Grub / Boot and Screen
- [x] <strong>KFS-2</strong> : GDT & Stack
- [ ] <strong>KFS-3</strong> : Memory
- [ ] <strong>KFS-4</strong> : Interrupts
- [ ] <strong>KFS-5</strong> : Processes
- [ ] <strong>KFS-6</strong> : Filesystem
- [ ] <strong>KFS-7</strong> : Syscalls, Sockets & env
- [ ] <strong>KFS-8</strong> : Kernel Modules
- [ ] <strong>KFS-9</strong> : ELF Parser
- [ ] <strong>KFS-X</strong> : Complete Unix System

##### Documentation

- <strong>[WIKI OSDEV](https://wiki.osdev.org/Expanded_Main_Page)</strong>
- <strong>[OSDEVER](http://www.osdever.net/bkerndev/Docs/title.htm)</strong>

---

### Compilation

```bash
$ make
```

### Running

```bash
$ make run
$ make run-iso
```

### Features

- [x] Basic Kernel System
- [x] Boot via Grub 2
- [x] ASM Bootable base
- [x] Kernel Library with basics functions and types 
- [x] Keyboard Support
- [x] VGA Support
- [x] Scroll and Cursor support
- [x] Colors support
- [x] A Complete GDT system

### GRUB 2

🚧 ... redaction pending ...🚧
<p align="center">
<kbd>
<img src="https://user-images.githubusercontent.com/66129673/176182052-630d5111-d968-4858-b01f-0a9beee57b4a.png" width="80%"></img>
</kbd>
</p>

### The KERNEL itself

🚧 ... redaction pending ...🚧
<p align="center">
<kbd>
<img src="https://user-images.githubusercontent.com/66129673/176182166-39fc1c60-7502-4f3a-878e-e8c7e3babe74.png" width="80%"></img>
</kbd>
</p>

### GDT

🚧 ... redaction pending ...🚧

- Kernel GDT
![image](https://user-images.githubusercontent.com/66129673/178467131-f02fd993-e9b6-497a-a2ce-cc615e5d0f5f.png)
![image](https://user-images.githubusercontent.com/66129673/178347667-2f4433ee-97e3-4cd6-9471-3bea19b7deb8.png)

- GDT - TEST
![image](https://user-images.githubusercontent.com/66129673/178347721-22271cd3-8d49-42c3-96c6-679c556e85e6.png)
![image](https://user-images.githubusercontent.com/66129673/178347760-4edac329-18af-428f-94be-5aa1255ceec6.png)

- STACK
![image](https://user-images.githubusercontent.com/66129673/178347873-5b58e8a6-f70f-45e0-b537-42edf01be889.png)

