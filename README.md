<!-- <center>
<img
    style="display: block; margin-left: none; margin-right: none; width: 75%"
    src="assets/KronOS-Logo.png"
img/>
</center> -->

# Kronos

--------------------------------

Kronos is a lightweight Unix-like kernel writen in C and ASM.

### Prerequisites

* <strong>GCC</strong> (https://gcc.gnu.org/)
* <strong>KVM</strong> (https://www.linux-kvm.org/)
* <strong>QEMU</strong> (https://qemu.org/)
  
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
