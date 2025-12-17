# ResearchOS

## Goals of ResearchOS

-   It is a hobby Operating System, intended to help learn concepts and algorithms in Operating systems, such as Process & Memory Management, File systems, Device drivers, etc.
-   It is also intended to help others start their journey into OS development by providing help docs, explanations and external links to guide them on their journey.

## ResearchOS details

-   ResearchOS is a multitasking Operating system that is based off of Unix and Linux, with an syscall API following POSIX standards.

-   It supports multiboot 2 and utilizes GRUB as a multiboot2 compliant bootloader, supported on many machines.

-   Currently it runs on x86 machines with future support for x86-64 machines.

-   ResearchOS utilizes GCC native tools, such as the GCC compiler, assembler (GAS), and linker (LD) to build the final kernel executable. Uses a variety of virtual machines, such as QEMU, Bochs, and Virtual Box, to test new version of the ResearchOS kernel.

## Running ResearchOS

### WSL

Running the commands to build ResearchOS works best on Linux systems (ie. WSL). Please install Linux (or WSL if on Windows) if not so already.

### GCC

Building ResearchOS also needs GCC support, so if your Linux system does not already have GCC, please install it (ie. `sudo apt install gcc -y` or `sudo apt install build-essential -y`);

### Virtual Machine

You can run ResearchOS on either of the VMs: QEMU, Bochs or VirtualBox. If do you not have any, please install at least one of them.

### Commands

1. To build ResearchOS, run: `./install.sh` on the project root directory.
2. If you are using QEMU, you can run `qemu-system-i386 -drive file=os.img,format=raw` on the same directory as the .img file.
3. If you are using Bochs, you can manually set up the configuration with the .img file or use the default configuration provided in the config/ folder (by loading it using Bochs' menu).



