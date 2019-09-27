#include "loop_divadd_10M.hpp"
#include <malloc.h>

EXPORT void loop_divadd_10M::init() {}

EXPORT uint32_t loop_divadd_10M::loop_divadd_10M_test()
{
/*     int* a;
    a = (int*)malloc(100);
    a[0] = 1;
    prints("the a[0] is:");
    printi(a[0]);
    free(a);
    return 0; */
    uint32_t i = 0, x = 100, y = 300, k = 500, n = 10000000;

    for (i; i < n; i++)
    {
        x /= y;
        x += k;
    }

    return x;
}
