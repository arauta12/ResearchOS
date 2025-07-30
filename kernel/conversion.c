#include <conversion.h>

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

char toLower(char c) {
    if (c >= 'a' && c <= 'z')
        return c;

    if (c >= 'A' && c <= 'Z')
        return c + 0x20;

    return c;
}

char toUpper(char c) {
    if (c >= 'A' && c <= 'Z')
        return c;

    if (c >= 'a' && c <= 'z')
        return c - 0x20;

    return c;
}

bool isAsciiChar(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int charToDigit(char c) {
    if (c >= '0' && c <= '9')
        return (int)c - 48;

    return -1;
}

char digitToChar(int digit) {
    if (digit >=0 && digit <= 9) 
        return (char)(digit + 48);

    return '\0';
}

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
            return (negative)? -1 * num : num;

        num = num * 10 + charToDigit(*ptr);
        ptr++;
    }

    return (negative)? -1 * num : num;
}

char* intToString(char* destination ,int num) {
    if (num == 0) {
        destination[0] = '0';
        return destination;
    }

    int endIndex = 0;

    if (num < 0) {
        destination[0] = '-';
        endIndex = 1;
        num *= -1;
    }

    int cpyNum = num;
    int numDigits = 0;
    while (cpyNum > 0) {
        numDigits++;
        cpyNum /= 10;
    }

    int i = endIndex + numDigits - 1;
    destination[i + 1] = '\0';
    while (num > 0) {
        destination[i] = digitToChar(num % 10);
        num /= 10;
        i--;
    }

    return destination;
}

char hexToChar(int hexDigit) {
    if (hexDigit >=0 && hexDigit <= 9) {
        return digitToChar(hexDigit);
    } else if (hexDigit >= 10 && hexDigit <= 15) {
        return (char)(hexDigit + 87);
    } else {
        return '\0';
    }
}

char* intToHex(char* destination, int num) {
    if (num < 0) {
        destination[0] = '\0';
        return destination;
    }

    if (num == 0) {
        destination[0] = '0';
        destination[1] = '\0';
        return destination;
    }

    int cpyNum = num;
    int numDigits = 0;
    while (cpyNum > 0) {
        numDigits++;
        cpyNum /= 16;
    }

    char* prefix = "0x";
    strcpy(destination, prefix);

    int i = numDigits + 1;
    destination[i + 1] = '\0';
    while (num > 0) {
        destination[i] = hexToChar(num % 16);
        i--;
        num /= 16;
    }

    return destination;
}
