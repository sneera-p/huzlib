#ifndef _SNEERA_SWAP_H
#define _SNEERA_SWAP_H

#include "type.h"
#include "unique.h"


/**
 * SWAP - swap the values of two variables
 * @a:		first variable
 * @b:		second variable
 *
 * Swaps the values of variables @a and @b using a temporary variable of the
 * appropriate type. The macro uses the compiler's typeof operator to determine
 * the type of @b and creates a temporary of that type.
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
   typecheck(typeof(b), a);            \
   typeof(b) __unique__(__tmp) = a;    \
   a = b;                              \
   b = __unique__(__tmp);              \
} while (0)


#endif /* _SNEERA_SWAP_H */
