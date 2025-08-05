#ifndef CONVERSION_H
#define CONVERSION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

bool isDigit(char);

char toLower(char);
char toUpper(char);

bool isLetter(char);

int charToDigit(char);
char digitToChar(int);
int stringToInt(const char*);
char* intToString(char*, int);
char hexToChar(int);
char* intToHex(char*, size_t);

#endif
