# ResearchOS
## Purpose of ResearchOS
ResearchOS is a OS that is custom coded to implement many of the important aspects of an OS and guide OS developers on how it can be done. It's purpose is not to be practical (to an extent) but to demonstrate OS concepts through its code.
## Installation Setup
### NASM (Assembler) 
1. Install NASM from https://www.nasm.us/
2. Place the NASM folder as an environment variable in your PC
3. Run 'nasm -v' in a command line to see if NASM installed properly
### Linux
This OS build utilizes Linux commands to build the binary files. If you are on a Linux system, great! Otherwise, please install WSL (Windows Subsystem for Linux) by referring to: https://learn.microsoft.com/en-us/windows/wsl/install.
### QEMU (or Bochs)
This is the virtual machine used to host ResearchOS. Instructions to install this can be found here: https://www.qemu.org/. Make sure to add an environment variable for the path to its folder.

If you prefer to install Bochs as your VM, you can install it from here: https://bochs.sourceforge.io/. Since this is an app, no environment variable is needed.

## Setting up the OS
After cloning the repository into a folder. Do the following in your Linux system command line:

1) Run 'make'
2) Run 'sh scripts/run.sh' to start the VM.
