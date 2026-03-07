#pragma once
#include <stdbool.h>
#include <stddef.h>

bool isalpha(int c);
bool isalnum(int c);
bool isspace(int c);
bool isnum(int c);

size_t strlen(const char* str);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t num);

char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t num);

int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t num);
