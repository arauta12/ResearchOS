#include <stdio.h>

/**
 * Prints log leveled message to default log screen (TTY) based on formatted string
 *
 * Levels are 0-7, 0 being the highest & 7 the lowest
 *
 * Ex. kerror(KERN_INFO, "string: %s", "not now") = 15;
 * @param errorLevel Integer representing one of the levels mentioned above
 * @param format Format specifier on how to print the string
 * @param va_args Variable number of arguments corresponding the to the format
 * @return The number of characters printed to the screen
 */
int kerror(int errorLevel, const char* format, ...) {
    if (errorLevel < 0 || errorLevel > 7)
        return 0;

    uint8_t oldForeground = getDefaultForeground();
    uint8_t oldBackground = getDefaultBackground();

    uint8_t foreground = oldForeground;
    uint8_t background = oldBackground;

    size_t numChars = 0;
    switch (errorLevel) {
        case KERN_EMERG:
            printFullColorString("(EMERG) ", WHITE, RED);
            foreground = WHITE;
            background = RED;
            numChars = 8;
            break;
        case KERN_ALERT:
            printColorString("(ALERT) ", RED);
            foreground = RED;
            numChars = 8;
            break;
        case KERN_CRIT:
            printColorString("(CRIT) ", RED);
            foreground = RED;
            numChars = 7;
            break;
        case KERN_ERR:
            printColorString("(ERR) ", RED);
            foreground = RED;
            numChars = 6;
            break;
        case KERN_WARNING:
            printColorString("(WARNING) ", YELLOW);
            foreground = YELLOW;
            numChars = 10;
            break;
        case KERN_NOTICE:
            printString("(NOTICE) ");
            numChars = 9;
            break;
        case KERN_INFO:
            printString("(INFO) ");
            numChars = 7;
            break;
        default:
            printColorString("(DEBUG) ", GRAY);
            foreground = GRAY;
            numChars = 8;
    }

    va_list args;
    va_start(args, format);

    setDefaultForeground(foreground);
    setDefaultBackground(background);

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
                    char intString[12] = {0};

                    intToString(intString, num);
                    printString(intString);

                    numChars += strlen(intString);
                    format++;
                    break;
                case 'x':
                case 'X':
                    int hex = va_arg(args, int);
                    char hexString[12];

                    intToHex(hexString, hex);
                    printString(hexString);

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

                    printString(str);
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

    setDefaultForeground(oldForeground);
    setDefaultBackground(oldBackground);

    return numChars;
}

/**
 * Print message to default log screen (TTY) based on formatted string
 *
 * Ex. kprintf("string: %s", "not now") = 15;
 * @param format Format specifier on how to print the string
 * @param va_args Variable number of arguments corresponding the to the format
 * @return The number of characters printed to the screen
 */
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
                    size_t num = va_arg(args, size_t);
                    char intString[12] = {0};

                    intToString(intString, num);
                    printString(intString);

                    numChars += strlen(intString);
                    format++;
                    break;
                case 'x':
                case 'X':
                    size_t hex = va_arg(args, size_t);
                    char hexString[12];

                    intToHex(hexString, hex);
                    printString(hexString);

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

                    printString(str);
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

/**
 * Prints a single character to the screen (TTY)
 *
 * Ex. kputchar('c') = 1
 * @param character Character to print to the screen
 * @return Number of characters printed to the screen
 */
int kputchar(int character) {
    char c = (char) character;
    printChar(c);
    return 1;
}
