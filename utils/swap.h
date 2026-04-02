#ifndef _HUZLIB_SWAP_H
#define _HUZLIB_SWAP_H

#include "types.h"


/* __swap_unique(a)
 * -------------------
 * internal cancatation utility used to
 * create unique tmp varaible name
 *
 * WARN:
 * This macro is the internal implementation and should not be used directly.
 */ 
#define __swap_concat_internal(a, b) a##b
#define __swap_concat(a, b) __swap_concat_internal(a, b)
#define __swap_unique(name) __swap_concat(name, __LINE__)

/*
 * SWAP(a, b)
 * ----------
 * Swaps the values of variables @a and @b using a temporary variable
 *
 * This macro works with any data type, including structures and unions, as
 * long as assignment is supported. It does not require the two variables to
 * be of exactly the same type, but they must be assignment-compatible.
 *
 * WARNING:
 * This macro evaluates its arguments multiple times. Do NOT pass
 * expressions with side effects such as i++, *p++, or function calls.
 * Doing so will result in undefined behavior.
 */
#define SWAP(a, b) do {                \
   typecheck_unqual(typeof(a), b);     \
   typeof(a) __swap_unique(__tmp) = a; \
   a = b;                              \
   b = __swap_unique(__tmp);           \
} while (0)


/*
 * swap(a, b)
 * ----------
 * Swaps the values of variables @a and @b using temporary variable pointers
 *
 * This macro works with any data type, including structures and unions, as
 * long as assignment is supported. It does not require the two variables to
 * be of exactly the same type, but they must be assignment-compatible.
 *
 * NOTE: This macro is sideeffect-free unlike it's cousin SWAP(a, b)
 */
#define swap(a, b) do {                                     \
   typecheck_unqual(typeof(a), b);                          \
   typeof(a) *__swap_unique(__a) = &(a);                    \
   typeof(b) *__swap_unique(__b) = &(b);                    \
   typeof(a) __swap_unique(__tmp) = *__swap_unique(__a);    \
   *__swap_unique(__a) = *__swap_unique(__b);               \
   *__swap_unique(__b) = __swap_unique(__tmp);              \
} while (0)


#endif /* _HUZLIB_SWAP_H */
