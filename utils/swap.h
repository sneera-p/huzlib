#ifndef _SNEERA_SWAP_H
#define _SNEERA_SWAP_H

#include "types.h"


/* __swap_concat(a, b)
 * -------------------
 * internal cancatation utility used to
 * create unique tmp varaible name in SWAP
 *
 * WARN:
 * This macro is the internal implementation and should not be used directly.
 */ 
#define __swap_concat_internal(a, b) a##b
#define __swap_concat(a, b) __swap_concat_internal(a, b)


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
#define SWAP(a, b) do {                            \
   typecheck_unqual(typeof(a), b);                 \
   typeof(a) __swap_concat(__tmp, __LINE__) = a;   \
   a = b;                                          \
   b = __swap_concat(__tmp, __LINE__);             \
} while (0)


#endif /* _SNEERA_SWAP_H */
