# Syscalls

[Linux Syscall Table](https://github.com/torvalds/linux/blob/master/arch/x86/entry/syscalls/syscall_64.tbl)

## x64 System V Calling Convention
- Args (1-6): `rdi`, `rsi`, `rdx`, `r8`, `r9`
- Return value: `rax`
- Caller save: `rax`, `rdi`, `rsi`, `r8`, `r9`, `r10`, `r11`
- Callee save:  `rbx`, `rsp`, `rbp`, `r12`, `r13`, `r14`, `r15`

## Syscall ABI for x86-64 (System V ABI)
- Syscall number: `rax` register
- Args (1-6): `rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9`

## Syscall ABI for x86 (System V ABI)
- Syscall number: `eax` register
- Args (1-6): `ebx`, `ecx`, `edx,` `esi`, `edi`, `ebp`