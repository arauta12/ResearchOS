#include <arch/i386/cpuid_info.h>
#include <lib/string.h>

void get_cpu_vendor(char* str) {
    uint32_t unused = 0, ebx = 0, ecx = 0, edx = 0;
    bool success = __get_cpuid(CPUID_VENDOR, &unused, &ebx, &ecx, &edx);
    str[0] = '\0';
    if (!success) return;

    char* ptr = (char*)&ebx;
    strncat(str, ptr, 4);

    ptr = (char*)&edx;
    strncat(str, ptr, 4);

    ptr = (char*)&ecx;
    strncat(str, ptr, 4);
}

uint32_t get_cpu_model() {
    uint32_t unused = 0, eax = 0;
    bool success = __get_cpuid(CPUID_FEATURES, &eax, &unused, &unused, &unused);
    if (!success) return 0;

    return eax;
}

CPUID_TEST(PSE);
CPUID_TEST(MSR);
CPUID_TEST(PAE);
CPUID_TEST(APIC);
CPUID_TEST(SYSENTER);
CPUID_TEST(PGE);
CPUID_TEST(PAT);
CPUID_TEST(PSE36);
