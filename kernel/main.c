#include <arch/i386/ps2_keyboard.h>
#include <early_print.h>
#include <io.h>
#include <lib/conversion.h>
#include <lib/string.h>
#include <main.h>
#include <multiboot2_tbl.h>

static char buff[MAX_BUFF_SIZE] = {0};
static char_pos_st pos[MAX_BUFF_SIZE] = {0};
static size_t len = 0;
static bool returned = false;

void early_terminal_kh(key_st key) {
    if (key.pressedDown && key.cmd == NOT_CMD) {
        char c = key.data;
        if (c == '\n') {
            returned = true;
            return;
        }

        if (c == '\b') {
            if (len == 0) return;

            // clear last char, move tty pos to last char, update buffer info
            len--;
            set_pos(pos[len].x, pos[len].y);
            tty_put_char('\0');
            buff[len] = 0;
            return;
        }

        if (len == MAX_BUFF_SIZE) return;

        buff[len] = c;
        pos[len].x = get_x_pos();
        pos[len].y = get_y_pos();
        kputchar(c);
        len++;
    }
}

void clear_buffer() {
    for (size_t i = 0; i < len; i++) {
        pos[i].x = pos[i].y = 0;
        buff[i] = '\0';
    }
    len = 0;
}

void early_terminal() {
    register_key_handler(early_terminal_kh);
    while (true) {
        kprintf("root> ");

        while (!returned);
        kputchar('\n');
        parse_command();
        clear_buffer();
        returned = false;
    }
}

PARSE_CMD(in) {
    for (size_t i = 1; i < num_args; i++) {
        if (strcmp(args[i], "--help") == 0 || strcmp(args[i], "-h") == 0) {
            kprintf("in -s [1,2,4] port\n");
            return;
        }
    }

    if (num_args != 4) {
        kprintf("Insufficient args!\n");
        return;
    }

    int len;
    u16 port;
    if (strcmp(args[1], "-s") != 0) {
        port = (u16)atoub(args[1]);
        len = atoi(args[3]);
    } else {
        len = atoi(args[2]);
        port = (u16)atoub(args[3]);
    }

    switch (len) {
        case 1:
            kprintf("Port 0x%x: 0x%x\n", port, inb(port));
            break;
        case 2:
            kprintf("Port 0x%x: 0x%x\n", port, inw(port));
            break;
        case 4:
            kprintf("Port 0x%x: 0x%x\n", port, inl(port));
            break;
        default:
            kprintf("Invalid port size!\n");
    }
}

PARSE_CMD(out) {
    for (size_t i = 1; i < num_args; i++) {
        if (strcmp(args[i], "--help") == 0 || strcmp(args[i], "-h") == 0) {
            kprintf("out -s [1,2,4] port data\n");
            return;
        }
    }

    if (num_args != 5) {
        kprintf("Insufficient args!\n");
        return;
    }

    bool has_port = false;
    size_t i = 1;
    int len;
    u16 port;
    size_t data;
    while (i < num_args) {
        if (strcmp(args[i], "-s") == 0) {
            len = atoi(args[++i]);
            i++;
            continue;
        }

        if (!has_port) {
            port = (u16)atoub(args[i++]);
            has_port = true;
        } else {
            data = atoub(args[i++]);
        }
    }

    switch (len) {
        case 1:
            kprintf("Port 0x%x <- 0x%x\n", port, (u8)data);
            outb(port, (u8)data);
            break;
        case 2:
            kprintf("Port 0x%x <- 0x%x\n", port, (u16)data);
            outb(port, (u16)data);
            break;
        case 4:
            kprintf("Port 0x%x <- 0x%x\n", port, (u32)data);
            outb(port, (u32)data);
            break;
        default:
            kprintf("Invalid port size!\n");
    }
}

PARSE_CMD(x) {
    for (size_t i = 1; i < num_args; i++) {
        if (strcmp(args[i], "--help") == 0 || strcmp(args[i], "-h") == 0) {
            kprintf("x /(num)[d,x,b,c] address\n");
            return;
        }
    }

    if (num_args != 3) {
        kprintf("Insufficient args!\n");
        return;
    }

    size_t num_bytes = 1;
    u8* addr = NULL;
    char format = '\0';

    for (size_t i = 1; i < num_args; i++) {
        if (strncmp(args[i], "/", 1) == 0) {
            if (strlen(args[i]) <= 1) {
                kprintf("Missing byte count and/or format!\n");
                return;
            }

            char* format_ptr = args[i] + 1;
            if (is_digit(args[1][1])) {
                num_bytes = (size_t)atoll(args[1] + 1);
                while (is_digit(*format_ptr)) format_ptr++;
            }

            if (*format_ptr == '\0') {
                kprintf("No format provided!\n");
                return;
            }

            format = *format_ptr;
        } else {
            addr = (u8*)atoub(args[i]);
        }
    }

    char temp_buff[9] = {0};
    switch (format) {
        case 'd':
        case 'D':
            kprintf("0x%x bytes at address %p:\n", num_bytes, addr);
            for (size_t i = 0; i < num_bytes; i++) {
                kprintf("%d ", addr[i]);
            }
            break;
        case 'x':
        case 'X':
            kprintf("0x%x bytes at address %p:\n", num_bytes, addr);
            for (size_t i = 0; i < num_bytes; i++) {
                kprintf("%x ", addr[i]);
            }
            break;
        case 'b':
        case 'B':
            kprintf("0x%x bytes at address %p:\n", num_bytes, addr);
            for (size_t i = 0; i < num_bytes; i++) {
                kprintf("%s ", utoa(addr[i], temp_buff, 2));
            }
            break;
        case 'c':
        case 'C':
            kprintf("0x%x bytes at address %p:\n", num_bytes, addr);
            for (size_t i = 0; i < num_bytes; i++) {
                kprintf("%c ", addr[i]);
            }
            break;
        default:
            kprintf("Invalid format!\n");
            return;
    }

    kputchar('\n');
}

void parse_command() {
    if (len == 0) return;

    // Parse args
    char* args[MAX_ARGS] = {NULL};
    char* ptr = buff;
    size_t i = 0;
    while (i < MAX_ARGS) {
        // skip whitespace
        while (*ptr != '\0' && isspace(*ptr)) ptr++;
        if (*ptr == '\0') break;

        // set new arg
        args[i++] = ptr;

        // parse current arg
        while (*ptr != '\0' && !isspace(*ptr)) ptr++;
        if (*ptr == '\0') break;
        *ptr = '\0';
        ptr++;
    }

    // Find correct parsing for function
    if (strcmp(args[0], "clear") == 0) {
        tty_clear_screen();
    } else if (strcmp(args[0], "in") == 0) {
        parse_in_cmd(i, args);
    } else if (strcmp(args[0], "out") == 0) {
        parse_out_cmd(i, args);
    } else if (strcmp(args[0], "x") == 0) {
        parse_x_cmd(i, args);
    } else if (strcmp(args[0], "regs") == 0) {
        // no args
    } else if (strcmp(args[0], "cpuid") == 0) {
        // "cpuid flag"
    } else if (strcmp(args[0], "memmap") == 0) {
        // no args
    } else if (strcmp(args[0], "fb_info") == 0) {
        // no args
    } else if (strcmp(args[0], "help") == 0) {
        kprintf(
            "Commands:\nclear, in, out, x, regs, cpuid, memmap, fb_"
            "info, help\n");
    } else {
        kprintf("Unknown command!\n");
    }
}

void kmain() {
    tty_clear_screen();

    kprintf("  _____                               _      ____   _____ \n");
    kprintf(" |  __ \\                             | |    / __ \\ / ____|\n");
    kprintf(" | |__) |___  ___  ___  __ _ _ __ ___| |__ | |  | | (___  \n");
    kprintf(
        " |  _  // _ \\/ __|/ _ \\/ _` | '__/ __| '_ \\| |  | |\\___ \\ \n");
    kprintf(" | | \\ \\  __/\\__ \\  __/ (_| | | | (__| | | | |__| |____) |\n");
    kprintf(
        " |_|  \\_\\___||___/\\___|\\__,_|_|  \\___|_| |_|\\____/|_____/ \n\n");

    early_terminal();
}
