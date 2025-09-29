#include <conversion.h>

/**
 * Determines if the ASCII character is a decimal digit (0-9).
 *
 * Ex. isDigit('1') = true, isDigit('a') = false
 *
 * @param c The character to check if it is a digit
 * @return True if it is a digit, false otherwise
 */
bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

/**
 * Converts the ASCII character to its lowercase version.
 *
 * If it is already lowercase (or doesn't support lowercase), it remains the same.
 *
 * Ex. toLower('A') = 'a', toLower('a') = 'a', toLower('2') = '2'
 *
 * @param c The character to turn to lowercase (if possible)
 * @return The lowercase character
 */
char toLower(char c) {
    if (c >= 'a' && c <= 'z')
        return c;

    if (c >= 'A' && c <= 'Z')
        return c + 0x20;

    return c;
}

/**
 * Converts the ASCII character to its uppercase version.
 *
 * If it is already uppercase (or doesn't support uppercase), it remains the same.
 *
 * Ex. toUpper('a') = 'A', toUpper('A') = 'A', toUpper('2') = '2'
 * @param c The character to turn to uppercase (if possible)
 * @return The uppercase character
 */
char toUpper(char c) {
    if (c >= 'A' && c <= 'Z')
        return c;

    if (c >= 'a' && c <= 'z')
        return c - 0x20;

    return c;
}

/**
 * Determines if the ASCII character is a letter a-z or A-Z.
 *
 * Ex. isLetter('a') = true, isLetter('1') = false
 * @param c The ASCII character to check
 * @return True if is a letter, false otherwise
 */
bool isLetter(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

/**
 * Converts the ASCII character to its integer (digit) equivalent
 *
 * Returns -1 if not possible
 *
 * Ex. charToDigit('1') = 1, charToDigit('p') = -1
 * @param c The ASCII character to convert to a integer digit
 * @return Integer (digit) equivalent of c, or -1 if conversion not possible
 */
int charToDigit(char c) {
    if (c >= '0' && c <= '9')
        return (int) c - 48;

    return -1;
}

/**
 * Converts the integer digit to an ASCII character
 *
 * Return null byte if conversion is not possible (ie. not a digit).
 *
 * Ex. digitToChar(9) = '9', digitToChar(23) = '\0'
 * @param digit The integer digit to convert to a character
 * @return The character conversion of the digit, or '\0' if not possible
 */
char digitToChar(int digit) {
    if (digit >= 0 && digit <= 9)
        return (char) (digit + 48);

    return '\0';
}

/**
 * Converts the given string to its integer equivalent
 *
 * Ex. stringToInt("123") = 123, stringToInt("-99") = -99
 * @param str constant pointer to the string to convert
 * @return The integer equivalent of the string
 */
int stringToInt(const char* str) {
    int num = 0;
    const char* ptr = str;
    bool negative = false;
    if (*ptr == '-') {
        negative = true;
        ptr++;
    }

    while (*ptr != '\0') {
        if (*ptr == '-' || !isDigit(*ptr))
            return (negative) ? -1 * num : num;

        num = num * 10 + charToDigit(*ptr);
        ptr++;
    }

    return (negative) ? -1 * num : num;
}

/**
 * Converts the integer into a string, copied to destination.
 *
 * NOTE: Make sure destination has sufficient space to store the string.
 *
 * Ex. intToString(ptr, 123) = "123", intToString(ptr, -10) = "-10"
 * @param destination String to copy final integer string to
 * @param num Integer to convert into a string (integer string)
 * @return Char pointer to new string (same pointer as destination)
 */
char* intToString(char* dest, size_t num) {
    if (num == 0) {
        dest[0] = '0';
        return dest;
    }

    int endIndex = 0;

    if (num < 0) {
        dest[0] = '-';
        endIndex = 1;
        num *= -1;
    }

    size_t cpyNum = num;
    int numDigits = 0;
    while (cpyNum > 0) {
        numDigits++;
        cpyNum /= 10;
    }

    int i = endIndex + numDigits - 1;
    dest[i + 1] = '\0';
    while (num > 0) {
        dest[i] = digitToChar(num % 10);
        num /= 10;
        i--;
    }

    return dest;
}

/**
 * Converts the hex digit (as an integer) to an ASCII character ('0'-'9', 'a'-'f').
 *
 * Ex. hexToChar(10) = 'a', hexToChar(8) = '8'
 * @param hexDigit Integer hex digit to convert
 * @return The char representation of the int hex digit
 */
char hexToChar(int hexDigit) {
    if (hexDigit >= 0 && hexDigit <= 9) {
        return digitToChar(hexDigit);
    } else if (hexDigit >= 10 && hexDigit <= 15) {
        return (char) (hexDigit + 87);
    } else {
        return '\0';
    }
}

/**
 * Converts the number into a hex string, with "0x" prefix
 *
 * NOTE: Make sure destination has sufficient space to store the string.
 *
 * Ex. intToHex(ptr, 0x123) = "0x123"
 * @param destination String to copy final hex string to
 * @param num The (hex) number to use
 * @return Char pointer to new string (same pointer as destination)
 */
char* intToHex(char* dest, size_t num) {
    if (num == 0) {
        dest[0] = '0';
        dest[1] = '\0';
        return dest;
    }

    size_t cpyNum = num;
    int numDigits = 0;
    while (cpyNum > 0) {
        numDigits++;
        cpyNum /= 16;
    }

    char* prefix = "0x";
    strcpy(dest, prefix);

    int i = numDigits + 1;
    dest[i + 1] = '\0';
    while (num > 0) {
        dest[i] = hexToChar(num % 16);
        i--;
        num /= 16;
    }

    return dest;
}
