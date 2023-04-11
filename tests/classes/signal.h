#include <luaclasslib.h>
#include <stdlib.h>
#include "array.h"

array_type(const void *, reflist);

static inline int ref_cmp(const void *a, const void *b) {
    const void **x = (const void **)a, **y = (const void **)b;
    return *x > *y ? 1 : (*x < *y ? -1 : 0);
}

binary_array_def(const void *, reflist, DO_NOTHING, ref_cmp);

#define SIGNAL_HEADER reflist slots;

extern luaC_Class signal_class;
