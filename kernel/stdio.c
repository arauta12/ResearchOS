#include <stdio.h>

int kprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    size_t numChars = 0;
    while (*format != '\0') {
        if (*format != '%') {
            numChars += kputchar(*format);
        } else {
            char formatChar = *(format + 1);
            switch (formatChar) {
                case '\0':
                    numChars += kputchar(*format);
                    break;
                case 'd':
                    int num = va_arg(args, int);
                    char intString[12]; 

                    intToString(intString, num);
                    printStringTTY(intString);
                    
                    numChars += strlen(intString);
                    format++;
                    break;
                case 'x':
                case 'X':
                    int hex = va_arg(args, int);
                    char hexString[12];

                    intToHex(hexString, hex);
                    printStringTTY(hexString);
                    
                    numChars += strlen(hexString);
                    format++;
                    break;
                case 'c':
                    char c = va_arg(args, int);

                    numChars += kputchar(c);
                    format++;
                    break;
                case 's':
                    const char* str = va_arg(args, const char*);

                    printStringTTY(str);
                    numChars += strlen(str);

                    format++;
                    break;
                default:
                    numChars += kputchar(*format);
            }
        }

        format++;
    }

    va_end(args);
    return numChars;
}

int kputchar(int character) {
    char c = (char)character;
    printCharTTY(c);
    return 1;
}
