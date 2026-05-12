#include <early_kprintf.h>

int kputchar(char c) {
    char str[] = {c, '\0'};
    return tty_print_string(str);
}

int kputs(const char* str) { return tty_print_string(str); }

int kprintf(const char* format, ...) {
    int num_chars = 0;
    char buff[100] = {'\0'};
    va_list args;
    va_start(args, format);

    size_t i = 0;
    char c = format[i];
    while (c != '\0') {
        if (c != '%') {
            num_chars += kputchar(c);
            c = format[++i];
            continue;
        }

        i++;
        c = format[i];
        if (c == '\0') break;

        // Format option
        switch (c) {
            case 'D':
            case 'd':
            case 'I':
            case 'i':
                int int_val = va_arg(args, int);
                num_chars += tty_print_string(itoa(int_val, buff, 10));
                break;
            case 'U':
            case 'u':
                unsigned int ui_val = va_arg(args, unsigned int);
                num_chars += tty_print_string(utoa(ui_val, buff, 10));
                break;
            case 'X':
            case 'x':
                unsigned int hex_val = va_arg(args, unsigned int);
                num_chars += tty_print_string(utoa(hex_val, buff, 16));
                break;
            case 'C':
            case 'c':
                char arg_c = va_arg(args, int);
                num_chars += kputchar(arg_c);
                break;
            case 'S':
            case 's':
                char* str = va_arg(args, char*);
                num_chars += tty_print_string(str);
                break;
            case 'P':
            case 'p':
                void* ptrVal = va_arg(args, void*);
                num_chars += tty_print_string(to_hex((size_t)ptrVal, buff));
                break;
            default:
                num_chars += kputchar('%');
                break;
        }

        i++;
        c = format[i];
    }

    return num_chars;
}

int kerror(const char* format, ...) {
    int num_chars = 0;
    char buff[100] = {'\0'};

    va_list args;
    va_start(args, format);

    COLORS prev_fg = tty_get_fg();
    tty_set_fg(RED);

    size_t i = 0;
    char c = format[i];
    num_chars = 1;
    while (c != '\0') {
        if (c != '%') {
            num_chars += kputchar(c);
            c = format[++i];
            continue;
        }

        i++;
        c = format[i];
        if (c == '\0') break;

        // Format option
        switch (c) {
            case 'D':
            case 'd':
            case 'I':
            case 'i':
                int int_val = va_arg(args, int);
                num_chars += tty_print_string(itoa(int_val, buff, 10));
                break;
            case 'U':
            case 'u':
                unsigned int ui_val = va_arg(args, unsigned int);
                num_chars += tty_print_string(utoa(ui_val, buff, 10));
                break;
            case 'X':
            case 'x':
                unsigned int hex_val = va_arg(args, unsigned int);
                num_chars += tty_print_string(utoa(hex_val, buff, 16));
                break;
            case 'C':
            case 'c':
                char arg_c = va_arg(args, int);
                num_chars += kputchar(arg_c);
                break;
            case 'S':
            case 's':
                char* str = va_arg(args, char*);
                num_chars += tty_print_string(str);
                break;
            case 'P':
            case 'p':
                void* ptrVal = va_arg(args, void*);
                num_chars += tty_print_string(to_hex((size_t)ptrVal, buff));
                break;
            default:
                num_chars += kputchar('%');
                break;
        }

        i++;
        c = format[i];
    }

    tty_get_fg(prev_fg);

    return num_chars;
}
