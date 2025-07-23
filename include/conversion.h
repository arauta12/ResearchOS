#ifndef CONVERSION_H
#define CONVERSION_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

bool isDigit(char c);

int charToDigit(char);
char digitToChar(int);
int stringToInt(const char*);
char* intToString(char*, int);
char hexToChar(int);
char* intToHex(char*, int);

#endif
