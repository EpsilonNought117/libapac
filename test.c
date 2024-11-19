#define LIBAPAC_IMPLEMENTATION
#include "libapac.h"

#define LIMB_COUNT 2

int main(void)
{
    set_memory_func_ptrs(&malloc, &realloc, &free);

    u64 a[LIMB_COUNT], b[LIMB_COUNT], c[LIMB_COUNT * 2];

    for (u64 i = 0; i < LIMB_COUNT; i++)
    {
        a[i] = ULLONG_MAX;
        b[i] = ULLONG_MAX;
    }

    for (u64 i = 0; i < LIMB_COUNT * 2; i++)
    {
        c[i] = 0;
    }

    apz_mul_karatsuba_x64(c, a, b, LIMB_COUNT, NULL);

    printf("\nA (little endian order): \n");
    for (u64 i = 0; i < LIMB_COUNT; i++)
    {
        printf("%llu\n", a[i]);
    }

    printf("\nB (little endian order): \n");
    for (u64 i = 0; i < LIMB_COUNT; i++)
    {
        printf("%llu\n", b[i]);
    }

    printf("\nC (little endian order): \n");
    for (u64 i = 0; i < LIMB_COUNT * 2; i++)
    {
        printf("%llu\n", c[i]);
    }

    return 0;
}