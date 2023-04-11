/*
 * array.h - condensed version of the array handling header used by AwesomeWM.
 *
 * Copyright © 2023 Abigail Teague <ateague063@gmail.com>
 * Copyright © 2009 Julien Danjou <julien@danjou.info>
 * Copyright © 2008 Pierre Habouzit <madcoder@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define array_type(elem, name) \
    typedef struct {           \
        elem *tab;             \
        int   len, size;       \
    } name

#define p_alloc_nr(x) (((x) + 16) * 3 / 2)
#define DO_NOTHING(...)

#define foreach(var, array)                                                     \
    for (int __foreach_index_##var = 0; __foreach_index_##var < (array).len;    \
         __foreach_index_##var     = (array).len)                               \
        for (typeof((array).tab) var = &(array).tab[__foreach_index_##var]; \
             (__foreach_index_##var < (array).len) &&                       \
             (var = &(array).tab[__foreach_index_##var]);                   \
             ++__foreach_index_##var)

#define ARRAY_COMMON_FUNCS(elem, name, dtor)                                  \
    static inline name *name##_new() { return (name *)malloc(sizeof(name)); } \
                                                                              \
    static inline void name##_init(name *arr) {                               \
        memset(arr, 0, sizeof(name));                                         \
    }                                                                         \
                                                                              \
    static inline void name##_wipe(name *arr) {                               \
        for (int i = 0; i < arr->len; i++) {                                  \
            dtor(&arr->tab[i]);                                               \
        }                                                                     \
        free(arr->tab);                                                       \
        arr->tab = NULL;                                                      \
    }                                                                         \
                                                                              \
    static inline void name##_grow(name *arr, int newlen) {                   \
        if (newlen > arr->size) {                                             \
            if (p_alloc_nr(arr->size) < newlen) {                             \
                arr->size = newlen;                                           \
            } else {                                                          \
                arr->size = p_alloc_nr(arr->size);                            \
            }                                                                 \
            arr->tab =                                                        \
                (elem *)realloc(arr->tab, sizeof(*(arr->tab)) * arr->size);   \
        }                                                                     \
    }                                                                         \
                                                                              \
    static inline void name##_splice(                                         \
        name *arr, int pos, int len, elem items[], int count) {               \
        assert(pos >= 0 && len >= 0 && count >= 0);                           \
        assert(pos <= arr->len && pos + len <= arr->len);                     \
        if (len != count) {                                                   \
            name##_grow(arr, arr->len + count - len);                         \
            memmove(                                                          \
                arr->tab + pos + count, arr->tab + pos + len,                 \
                (arr->len - pos - len) * sizeof(*items));                     \
            arr->len += count - len;                                          \
        }                                                                     \
        memcpy(arr->tab + pos, items, count * sizeof(*items));                \
    }                                                                         \
                                                                              \
    static inline elem name##_take(name *arr, int pos) {                      \
        elem res = arr->tab[pos];                                             \
        name##_splice(arr, pos, 1, NULL, 0);                                  \
        return res;                                                           \
    }                                                                         \
                                                                              \
    static inline int name##_indexof(name *arr, elem *e) {                    \
        int idx = e - arr->tab;                                               \
        return (idx >= arr->len || idx < 0) ? -1 : idx;                       \
    }                                                                         \
                                                                              \
    static inline elem name##_remove(name *arr, elem *e) {                    \
        return name##_take(arr, name##_indexof(arr, e));                      \
    }

/** Non-ordered array functions */
#define array_def(elem, name, dtor)                       \
    ARRAY_COMMON_FUNCS(elem, name, dtor)                  \
                                                          \
    static inline void name##_push(name *arr, elem e) {   \
        name##_splice(arr, 0, 0, &e, 1);                  \
    }                                                     \
                                                          \
    static inline void name##_append(name *arr, elem e) { \
        name##_splice(arr, arr->len, 0, &e, 1);           \
    }

/** Binary ordered array functions */
#define binary_array_def(elem, name, dtor, cmp)                           \
    ARRAY_COMMON_FUNCS(elem, name, dtor)                                  \
                                                                          \
    static inline void name##_insert(name *arr, elem e) {                 \
        int l = 0, r = arr->len;                                          \
        while (l < r) {                                                   \
            int i   = (r + l) / 2;                                        \
            int res = cmp(&e, &arr->tab[i]);                              \
            if (res == 0) return; /* Already added, ignore */             \
            if (res < 0) r = i;                                           \
            else l = i + 1;                                               \
        }                                                                 \
        name##_splice(arr, r, 0, &e, 1);                                  \
    }                                                                     \
                                                                          \
    static inline elem *name##_lookup(name *arr, elem *e) {               \
        return (elem *)bsearch(e, arr->tab, arr->len, sizeof(elem), cmp); \
    }
