## KronOS - A Kernel from Scratch

Welcome to the official GitHub page for KronOS, a custom kernel built from scratch. KronOS is designed to provide a unique insight into the workings of an operating system kernel, offering a hands-on experience in low-level system programming.

### About KronOS
KronOS is a minimalistic (linux like), kernel implemented from scratch.

### Getting Started with KronOS
To start exploring KronOS, you can visit the KronOS Repository on GitHub. Here you will find the source code, documentation, and build instructions.

### Features
KronOS is a work in progress and currently includes the following features:

- Basic Memory Management: Handling memory allocation and deallocation.
- Simple Scheduler: Basic process scheduling mechanism.
- Hardware Interactions: Communicating with hardware through device drivers.
- Interrupt Handling: Managing system interrupts.
- ...

### Build and Run

- To build and run KronOS, follow these steps:

1. Clone the repository: `git clone https://github.com/vvaucoul/KronOS.git`
2. Navigate to the project directory: `cd KronOS`
    - Edit .config file: `mk-files/.config`
4. Build the kernel: `make`
5. Run the kernel using a virtual machine like QEMU: `qemu-system-i386 -kernel isodir/boot/kronos.bin`

### Contributions
Contributions to KronOS are welcome! Whether you're fixing bugs, improving the documentation, or proposing new features, your help is appreciated. Please read our contribution guidelines before submitting your pull requests.

### Documentation
For more detailed information about KronOS, including its architecture, development environment setup, and contribution guidelines, please refer to our documentation.

### Support and Contact
If you have any questions or need support with KronOS, feel free to raise an issue in the GitHub repository or contact the maintainers.

### License
