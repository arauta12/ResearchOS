#include <lib/string.h>

bool isalpha(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

bool isalnum(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9');
}

bool isspace(int c) {
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' ||
            c == '\f');
}

bool isnum(int c) { return (c >= '0' && c <= '9'); }

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') len++;
    return len;
}

char* strcpy(char* dest, const char* src) {
    size_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

char* strncpy(char* dest, const char* src, size_t num) {
    size_t i = 0;
    while (i < num && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    while (i < num) dest[i++] = '\0';

    return dest;
}

char* strcat(char* dest, const char* src) {
    size_t destPos = strlen(dest), i = 0;
    while (src[i] != '\0') dest[destPos++] = src[i++];
    dest[destPos] = '\0';
    return dest;
}

char* strncat(char* dest, const char* src, size_t num) {
    size_t destPos = strlen(dest), i = 0;
    while (i < num && src[i] != '\0') dest[destPos++] = src[i++];
    dest[destPos] = '\0';
    return dest;
}

int strcmp(const char* str1, const char* str2) {
    size_t i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] < str2[i]) return -1;
        if (str1[i] > str2[i]) return 1;
        i++;
    }

    if (str1[i] < str2[i]) return -1;
    if (str1[i] > str2[i]) return 1;
    return 0;
}

int strncmp(const char* str1, const char* str2, size_t num) {
    size_t i = 0;
    while (str1[i] != '\0' && str2[i] != '\0' && i < num) {
        if (str1[i] < str2[i]) return -1;
        if (str1[i] > str2[i]) return 1;
        i++;
    }

    if (i == num) return 0;
    if (str1[i] < str2[i]) return -1;
    if (str1[i] > str2[i]) return 1;
    return 0;
}

void* memcpy(void* dest, const void* src, size_t count) {
    u8* destArr = (u8*)dest;
    const u8* srcArr = (const u8*)src;

    size_t i = 0;
    for (; i < count; i++) destArr[i] = srcArr[i];

    return dest;
}
