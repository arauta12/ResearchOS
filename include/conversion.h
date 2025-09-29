#ifndef CONVERSION_H
#define CONVERSION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

bool isDigit(char c);

char toLower(char c);
char toUpper(char c);

bool isLetter(char c);

int charToDigit(char c);
char digitToChar(int digit);
int stringToInt(const char* str);
char* intToString(char* dest, size_t num);
char hexToChar(int hexDigit);
char* intToHex(char* dest, size_t num);

#endif
