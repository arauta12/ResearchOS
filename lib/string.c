#include <string.h>

/**
 * Gives the length of the given string.
 * @param str String to get length of
 * @return Length of the string
 */
size_t strlen(const char* str) {
    size_t len = 0;
    while (*str != '\0') {
        len++;
        str++;
    }

    return len;
}

/**
 * Copies all characters from source string to destination string.
 * 
 * NOTE: Destination string must have enough space to fit the copied result!
 * @param dest String being copied to
 * @param src String to copy to dest
 * @return Pointer to new string (always just dest)
 */
char* strcpy(char* dest, const char* src) {
    char* cpy = dest;
    while (*src != '\0') {
        *cpy = *src;
        cpy++;
        src++;
    }
    *cpy = '\0';

    return dest;
}

/**
 * Copies first num characters from source string to destination string.
 * 
 * NOTE: Destination string must have enough space to fit the copied result! 
 * @param dest String being copied to
 * @param src String of character to copy to dest
 * @param num Number of characters to copy from src
 * @return Pointer to new string (always just dest)
 */
char* strncpy(char* dest, const char* src, size_t num) {
    size_t copied = 0;
    char* cpy = dest;

    while (*src != '\0' && copied < num) {
        *cpy = *src;
        cpy++;
        src++;
        copied++;
    }

    while (copied < num) {
        *cpy = '\0';
        cpy++;
        copied++;
    }

    *cpy = '\0';

    return dest;
}

/**
 * Appends the all characters from source string onto destination string.
 * 
 * NOTE: Destination string must have enough space to fit the concatenated result! 
 * @param dest String being changed by concatenating
 * @param src String to append to dest
 * @return Pointer to new string (always just dest)
 */
char* strcat(char* dest, const char* src) {
    char* cpy = dest;

    while (*cpy != '\0')
        cpy++;

    while (*src != '\0') {
        *cpy = *src;
        cpy++;
        src++;
    }

    *cpy = '\0';

    return dest;
}

/**
 * Appends the first num characters from source string onto destination string.
 * 
 * NOTE: Destination string must have enough space to fit the concatenated result! 
 * @param dest String being changed by concatenating
 * @param src String of character to append to dest
 * @param num Number of character to append
 * @return Pointer to new string (always just dest)
 */
char* strncat(char* dest, const char* src, size_t num) {
    char* cpy = dest;

    while (*cpy != '\0')
        cpy++;

    size_t count = 0;
    while (*src != '\0' && count < num) {
        *cpy = *src;
        cpy++;
        src++;
        count++;
    }

    while (count < num) {
        *cpy = '\0';
        cpy++;
        count++;
    }

    *cpy = '\0';

    return dest;
}

/**
 * Lexicographically compares the two strings. 0 represents the two string are the same, -1 means str1 < str2, 
 * 1 means str1 > str2.
 * 
 * Ex. strcmp("12", "13") = -1, strcmp("123", "12") = 1
 * @param str1 1st string (base to compare)
 * @param str2 2nd string (what str1 is being compared to)
 * @return -1 if str1 < str2, 1 if str1 > str2, 0 if the strings are identical
 */
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

/**
 * Lexicographically compares the first num character of the two strings. 
 * 0 represents the two string are the same, -1 means str1 < str2, 1 means str1 > str2.
 * 
 * Ex. strcmp("12", "13") = -1, strcmp("123", "12") = 1
 * @param str1 1st string (base to compare)
 * @param str2 2nd string (what str1 is being compared to)
 * @param num Number of character from start to check
 * @return -1 if str1 < str2, 1 if str1 > str2, 0 if the first num characters are identical
 */
int strncmp(const char* str1, const char* str2, size_t num) {
    size_t count = 0;

    while ((*str1 != '\0' || *str2 != '\0') && count < num) {
        if (*str1 < *str2)
            return -1;

        if (*str1 > *str2)
            return 1;

        str1++;
        str2++;
        count++;
    }

    return 0;
}
