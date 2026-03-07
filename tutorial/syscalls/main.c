// #include "demo.h"
#include <unistd.h>

#define FD_STDOUT 1

int main() {
    const char* buff = "HELLO THERE\n";
    int bytesRead = write(1, buff, 13);
    return 0;
}