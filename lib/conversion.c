#include <lib/conversion.h>
#include <lib/string.h>
#include <stdbool.h>

char itoh(int n) {
    if (n >= 0 && n <= 9) return INT_TO_C(n);

    switch (n) {
        case 0xA:
            return 'A';
        case 0xB:
            return 'B';
        case 0xC:
            return 'C';
        case 0xD:
            return 'D';
        case 0xE:
            return 'E';
        case 0xF:
            return 'F';
        default:
            return (char)n;
    }
}

char to_upper(char c) {
    if (c >= 'A' && c <= 'Z') return c;
    if (c >= 'a' && c <= 'z') return (char)(c - 32);

    return c;
}

char to_lower(char c) {
    if (c >= 'a' && c <= 'z') return c;
    if (c >= 'A' && c <= 'Z') return (char)(c + 32);

    return c;
}

char* to_hex_u8(u8 value, char* str) { return to_string(value, str, 16, 2); }

char* to_hex_u16(u16 value, char* str) { return to_string(value, str, 16, 4); }

char* to_hex_u32(u32 value, char* str) { return to_string(value, str, 16, 8); }

char* to_hex_u64(u64 value, char* str) { return to_string(value, str, 16, 16); }

char* to_hex(size_t value, char* str) {
    return to_string(value, str, 16, sizeof(size_t) * 2);
}

char* itoa(int value, char* str, int base) {
    if (base <= 1 || base > 16) return str;
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    size_t i = 0;
    if (value < 0) {
        if (base == 10) str[i++] = '-';
        value *= -1;
    }

    size_t oldPos = i;
    while (value > 0) {
        str[i++] = itoh(value % base);
        value /= base;
    }
    str[i] = '\0';

    for (size_t j = oldPos; j <= (i - 1 - oldPos) / 2; j++) {
        char cpy = str[j];
        str[j] = str[i - 1 - j + oldPos];
        str[i - 1 - j + oldPos] = cpy;
    }

    return str;
}

char* to_string(size_t value, char* str, int base, int width) {
    if (base <= 1 || width <= 0 || base > 16) return str;
    if (value == 0) {
        for (int i = 0; i < width; i++) str[i] = '0';
        str[width] = '\0';
        return str;
    }

    int i = 0;
    while (value > 0 && width > 0) {
        str[i++] = itoh(value % base);
        value /= base;
        width--;
    }

    while (width > 0) {
        str[i++] = '0';
        width--;
    }

    str[i] = '\0';

    for (int j = 0; j <= (i - 1) / 2; j++) {
        char cpy = str[j];
        str[j] = str[i - 1 - j];
        str[i - 1 - j] = cpy;
    }

    return str;
}

int atoi(const char* str) {
    int res = 0;
    size_t i = 0;
    while (isspace(str[i]) && str[i] != '\0') i++;

    if (str[i] == '\0') return res;

    bool is_neg = false;
    if (str[i] == '-') {
        is_neg = true;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    while (str[i] != '\0' && isnum(str[i])) {
        res *= 10;
        res += C_TO_INT(str[i++]);
    }

    return (is_neg) ? res * -1 : res;
}

long int atol(const char* str) {
    long int res = 0;
    size_t i = 0;
    while (isspace(str[i]) && str[i] != '\0') i++;

    if (str[i] == '\0') return res;

    bool is_neg = false;
    if (str[i] == '-') {
        is_neg = true;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    while (str[i] != '\0' && isnum(str[i])) {
        res *= 10;
        res += C_TO_INT(str[i++]);
    }

    return (is_neg) ? res * -1 : res;
}

long long int atoll(const char* str) {
    long long int res = 0;
    size_t i = 0;
    while (isspace(str[i]) && str[i] != '\0') i++;

    if (str[i] == '\0') return res;

    bool is_neg = false;
    if (str[i] == '-') {
        is_neg = true;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    while (str[i] != '\0' && isnum(str[i])) {
        res *= 10;
        res += C_TO_INT(str[i++]);
    }

    return (is_neg) ? res * -1 : res;
}

int abs(int n) { return (n < 0) ? n * -1 : n; }
