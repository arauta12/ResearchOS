#pragma once
#include <common.h>
#include <stdbool.h>
#include <stddef.h>

#define C_TO_INT(c) ((c) - 48)
#define INT_TO_C(n) ((char)((n) + 48))

char itoh(int n);
int htoi(char c);
char to_upper(char c);
char to_lower(char c);
bool is_letter(u8 code);
bool is_digit(char c);

char* to_string(size_t value, char* str, int base, int width);
char* to_hex_u8(u8 value, char* str);
char* to_hex_u16(u16 value, char* str);
char* to_hex_u32(u32 value, char* str);
char* to_hex_u64(u64 value, char* str);
char* to_hex(size_t value, char* str);

char* itoa(int value, char* str, int base);
char* utoa(unsigned int value, char* str, int base);
size_t atoub(const char* str);
int atoi(const char* str);
long int atol(const char* str);
long long int atoll(const char* str);

int abs(int n);