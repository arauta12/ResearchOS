#include <early_print.h>
#include <lib/conversion.h>

void kmain() {
    char arr[50] = {0};
    tty_init();
    tty_print_string(to_hex(0xdead, arr));

    while (1);
}