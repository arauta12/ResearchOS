#include <string.h>

size_t strlen(const char* str) {
    size_t len = 0;
    while (*str != '\0') {
        len++;
        str++;
    }

    return len;
}

char* strcpy(char* destination, const char* source) {
    char* cpy = destination;
    while (*source != '\0') {
        *cpy = *source;
        cpy++;
        source++;
    }
    *cpy = '\0';

    return destination;
}

char* strncpy(char* destination, const char* source, size_t num) {
    size_t copied = 0;
    char* cpy = destination;

    while (*source != '\0' && copied < num) {
        *cpy = *source;
        cpy++;
        source++;
        copied++;
    }

    while (copied < num) {
        *cpy = '\0';
        cpy++;
        copied++;
    }

    *cpy = '\0';

    return destination;
}

char* strcat(char* destination, const char* source) {
    char* cpy = destination;

    while (*cpy != '\0')
        cpy++;

    while (*source != '\0') {
        *cpy = *source;
        cpy++;
        source++;
    }

    *cpy = '\0';

    return destination;
}

char* strncat(char* destination, const char* source, size_t num) {
    char* cpy = destination;

    while (*cpy != '\0')
        cpy++;

    size_t count = 0;
    while (*source != '\0' && count < num) {
        *cpy = *source;
        cpy++;
        source++;
        count++;
    }

    while (count < num) {
        *cpy = '\0';
        cpy++;
        count++;
    }

    *cpy = '\0';

    return destination;
}

int strcmp(const char* str1, const char* str2) {
    while (*str1 != '\0' || *str2 != '\0') {
        if (*str1 < *str2)
            return -1;

        if (*str1 > *str2)
            return 1;

        str1++;
        str2++;
    }

    return 0;
}

