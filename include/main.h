#pragma once

#include <common.h>
#include <early_kprintf.h>

#define MAX_BUFF_SIZE 200
#define MAX_ARGS 10

#define PARSE_CMD(cmd) void parse_##cmd##_cmd(size_t num_args, char** args)

typedef struct {
    int x;
    int y;
} char_pos_st;

void early_terminal_kh(key_st key);
void clear_buffer();
void early_terminal();
void parse_in_cmd(size_t num_args, char** args);
void parse_out_cmd(size_t num_args, char** args);
void parse_command();
void kmain();
