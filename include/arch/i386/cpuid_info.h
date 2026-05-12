#pragma once

#include <common.h>
#include <cpuid.h>
#include <stdbool.h>
#include <stddef.h>

#define CPUID_VENDOR 0
#define CPUID_FEATURES 1
#define CPUID_CACHE_INFO 1
#define CPUID_SERIAL 3

#define CPUID_PSE_FLAG (1 << 3)
#define CPUID_MSR_FLAG (1 << 5)
#define CPUID_PAE_FLAG (1 << 6)
#define CPUID_APIC_FLAG (1 << 9)
#define CPUID_SYSENTER_FLAG (1 << 11)
#define CPUID_PGE_FLAG (1 << 13)
#define CPUID_PAT_FLAG (1 << 16)
#define CPUID_PSE36_FLAG (1 << 17)

#define CPUID_TEST_HEAD(flag) bool has_cpu_##flag();
#define CPUID_TEST(flag)                                              \
    bool has_cpu_##flag() {                                           \
        u32 unused = 0, edx = 0;                                      \
        __get_cpuid(CPUID_FEATURES, &unused, &unused, &unused, &edx); \
        return (edx & (CPUID_##flag##_FLAG)) != 0;                    \
    }

void get_cpu_vendor(char* str);
u32 get_cpu_model();

CPUID_TEST_HEAD(PSE);
CPUID_TEST_HEAD(MSR);
CPUID_TEST_HEAD(PAE);
CPUID_TEST_HEAD(APIC);
CPUID_TEST_HEAD(SYSENTER);
CPUID_TEST_HEAD(PGE);
CPUID_TEST_HEAD(PAT);
CPUID_TEST_HEAD(PSE36);
