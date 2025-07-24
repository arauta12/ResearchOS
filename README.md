# ResearchOS

## Goals of ResearchOS

-   It is a hobby Operating System, intended to help learn concepts and algorithms in Operating systems, such as Memory Management, File systems, Device drivers, etc.
-   It is also intended to help others start their journey into OS development by providing help docs, explanations and external links to guide them on their journey.

## ResearchOS details

-   ResearchOS is (expected to be) a multitasking Operating system that is based off of Unix and Linux, with an API (in the future) following POSIX standards.

-   It supports multiboot 2 and utilizes GRUB as a multiboot2 compliant bootloader supported on many machines.

-   Currently it runs on x86 machines (x86-64 too by backwards compatibility) with future support for other machine architectures.

-   ResearchOS also utilizes GCC native tools, such as the compiler, assembler (GAS) and linker (LD) to build the final result, as well as multiple Virtual Machines, such as QEMU, Bochs, and Virtual Box, to test iterations of ResearchOS.

## Running ResearchOS

### WSL

Running the commands to build ResearchOS works best on Linux systems (ie. WSL). Please install Linux (or WSL if on Windows) if not so already.

### GCC

Building ResearchOS also needs GCC support, so if your Linux system does not already have GCC, please install it (ie. `sudo apt install gcc -y` or `sudo apt install build-essential -y`);

### Virtual Machine

You can run ResearchOS on either of the VMs: QEMU, Bochs or VirtualBox. If do you not have any, please install at least one of them.

### Commands

1. To build ResearchOS, run: `make; make iso` on the project root directory.
2. If you are using QEMU, you can run `qemu-system-x86_64 -drive file=os.iso,format=raw` on the same directory as the .iso file.
3. If you are using Bochs, you can manually set up the configuration with the .iso file or use the default configuration provided in the config/ folder (by loading it using Bochs' menu).
4. If you are using VirtualBox, you can just specifiy the .iso file and start the VM.
