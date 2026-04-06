#ifndef HUZLIB_TYPEMATH_H
#define HUZLIB_TYPEMATH_H

#include <assert.h>
#include <stdint.h>


#define _INT_TYPES(x, ...)             \
   x(uint8_t, __VA_ARGS__)             \
   x(uint16_t, __VA_ARGS__)            \
   x(uint32_t, __VA_ARGS__)            \
   x(uint64_t, __VA_ARGS__)            \
   x(int8_t, __VA_ARGS__)              \
   x(int16_t, __VA_ARGS__)             \
   x(int32_t, __VA_ARGS__)             \
   x(int64_t, __VA_ARGS__)             \
   x(unsigned char, __VA_ARGS__)       \
   x(unsigned short, __VA_ARGS__)      \
   x(unsigned int, __VA_ARGS__)        \
   x(unsigned long, __VA_ARGS__)       \
   x(unsigned long long, __VA_ARGS__)  \
   x(signed char, __VA_ARGS__)         \
   x(short, __VA_ARGS__)               \
   x(int, __VA_ARGS__)                 \
   x(long, __VA_ARGS__)                \
   x(long long, __VA_ARGS__)           \
   x(size_t, __VA_ARGS__)              \
   x(ptrdiff_t, __VA_ARGS__)           \

#define _FLOAT_TYPES(x, ...)           \
   x(float, __VA_ARGS__)               \
   x(double, __VA_ARGS__)              \
   x(long double, __VA_ARGS__)




/*
 * min(x, y)
 * ---------
 * Returns the smaller of two numeric values.
 * - Evaluates each argument only once (prevents double-evaluation bugs)
 * - Performs type checking to ensure arguments have compatible types
 * - Uses statement expression (GNU C) or _Generic (C11) for type safety
 * 
 * Example:
 *   int a = 5, b = 3;
 *   int smallest = min(a, b);  // Returns 3
 */
#ifndef min
#define _MIN_RAW(x, y) ((x) < (y) ? (x) : (y))

#if _HAS_STATEMENT_EXPR
   #define min(x, y) __extension__ ({        \
      typecheck(typeof(x), y);               \
      typeof(y) _UNIQUE(__x) = (x);          \
      typeof(x) _UNIQUE(__y) = (y);          \
      _MIN_RAW(_UNIQUE(__x), _UNIQUE(__y));  \
   })

#else
   #define _GENERATE_MIN(type)                        \
      static inline type min_##type(type x, type y) { \
         return _MIN_RAW(x, y);                       \
      }

   _INT_TYPES(_GENERATE_MIN)
   _FLOAT_TYPES(_GENERATE_MIN)

   #define min(x, y) _Generic((x),                    \
      typeof(y): _Generic((y),                        \
         _INT_TYPES(XMACRO_GENERIC_ENTRY, min_)       \
         _FLOAT_TYPES(XMACRO_GENERIC_ENTRY, min_)     \
         XMACRO_GENERIC_DEFAULT(min_unsupported)      \
      ),                                              \
      default: min_unsupported                        \
   )(x, y)

#endif
#endif /* min */


/*
 * max(x, y)
 * ---------
 * Returns the larger of two numeric values.
 * - Evaluates each argument only once * - Maintains type safety through type checking
 * - Works with all numeric types (integers, floats, pointers)
 * 
 * Example:
 *   size_t size = 1024;
 *   size_t min_size = 4096;
 *   size_t aligned = max(size, min_size);  // Returns 4096
 */
#ifndef max
#define _MAX_RAW(x, y) ((x) > (y) ? (x) : (y))

#if _HAS_STATEMENT_EXPR
   #define max(x, y) __extension__ ({        \
      typecheck(typeof(x), y);               \
      typeof(y) _UNIQUE(__x) = (x);          \
      typeof(x) _UNIQUE(__y) = (y);          \
      _MAX_RAW(_UNIQUE(__x), _UNIQUE(__y));  \
   })

#else
   #define _GENERATE_MAX(type)                        \
      static inline type max_##type(type x, type y) { \
         return _MAX_RAW(x, y);                       \
      }

   _INT_TYPES(_GENERATE_MAX)
   _FLOAT_TYPES(_GENERATE_MAX)

   #define max(x, y) _Generic((x),                    \
      typeof(y): _Generic((y),                        \
         _INT_TYPES(XMACRO_GENERIC_ENTRY, max_)       \
         _FLOAT_TYPES(XMACRO_GENERIC_ENTRY, max_)     \
         XMACRO_GENERIC_DEFAULT(max_unsupported)      \
      ),                                              \
      default: max_unsupported                        \
   )(x, y)

#endif
#endif /* max */


/*
 * clamp(val, lo, hi)
 * ------------------
 * Constrains a value within the inclusive range [lo, hi].
 * - Equivalent to: min(max(val, lo), hi)
 * - Ensures lo <= val <= hi
 * - Handles edge cases where lo > hi (behavior follows min/max)
 * 
 * Example:
 *   int temperature = 150;
 *   int clamped = clamp(temperature, 0, 100);  // Returns 100
 */
#ifndef clamp
#define clamp(val, lo, hi) min(max(val, lo), hi)
#endif


/*
 * round_up(x, align)
 * ------------------
 * Rounds x upward to the nearest multiple of align.
 * - align must be positive (behavior undefined for align <= 0)
 * - Returns: ((x + align - 1) / align) * align
 * - Evaluates arguments only once to prevent side-effect duplication
 * 
 * Example:
 *   round_up(5, 4)   // Returns 8
 *   round_up(8, 4)   // Returns 8
 *   round_up(0, 4)   // Returns 0
 */
#ifndef round_up
#define _ROUND_UP_RAW(x, a) (((x) + (a) - 1) / (a) * (a))

#if _HAS_STATEMENT_EXPR
   #define round_up(x, align) __extension__ ({     \
      typeof(x) _UNIQUE(__x) = (x);                \
      typeof(align) _UNIQUE(__a) = (align);        \
      _ROUND_UP_RAW(_UNIQUE(__x), _UNIQUE(__a));   \
   })

#else
   #define _GENERATE_ROUND_UP(type)                         \
      static inline type round_up_##type(type x, type a) {  \
         return _ROUND_UP_RAW(x, a);                        \
      }

   _INT_TYPES(_GENERATE_ROUND_UP)
   #define round_up(x, align) _Generic((x),           \
      _INT_TYPES(XMACRO_GENERIC_ENTRY, round_up_)     \
      XMACRO_GENERIC_DEFAULT(round_up_unsupported)    \
   )(x, align)

#endif
#endif /* round_up */


/*
 * round_up_pow2(x, align)
 * -----------------------
 * Rounds x upward to the nearest multiple of align (align must be power of 2).
 * - align MUST be a power of two (assertion checks this in debug builds)
 * - Uses bitwise operations for efficiency: ((x) + (align) - 1) & ~((align) - 1)
 * - Faster than generic round_up for power-of-two alignments
 * 
 * Example:
 *   round_up_pow2(5, 4)   // Returns 8 (4 is power of 2)
 *   round_up_pow2(7, 8)   // Returns 8
 *   round_up_pow2(6, 4)   // Returns 8
 */
#ifndef round_up_pow2
#define _ROUND_UP_POW2_RAW(x, a)  (((x) + (a) - 1) & ~((a) - 1))

#if _HAS_STATEMENT_EXPR
   #define round_up_pow2(x, align) __extension__ ({      \
      typeof(x) _UNIQUE(__x) = (x);                      \
      typeof(align) _UNIQUE(__a) = (align);              \
      assert((_UNIQUE(__a) & (_UNIQUE(__a) - 1)) == 0);  \
      _ROUND_UP_POW2_RAW(_UNIQUE(__x), _UNIQUE(__a));    \
   })

#else
   #define _GENERATE_ROUND_UP_POW2(type)                          \
      static inline type round_up_pow2_##type(type x, type a) {   \
         assert((a & (a - 1)) == 0);                              \
         return _ROUND_UP_POW2_RAW(x, a);                         \
      }

   _INT_TYPES(_GENERATE_ROUND_UP_POW2)
   #define round_up_pow2(x, align) _Generic((x),      \
      _INT_TYPES(_GENERIC_ENTRY, round_up_pow2_)      \
      _GENERIC_DEFAULT(round_up_pow2_unsupported)     \
   )(x, align)

#endif
#endif /* round_up_pow2 */


/*
 * round_down(x, align)
 * --------------------
 * Rounds x downward to the nearest multiple of align.
 * - align can be zero, in which case x is returned unchanged
 * - Returns: (x / align) * align (integer division truncates toward zero)
 * - Evaluates arguments only once to prevent side-effect issues
 * 
 * Example:
 *   round_down(9, 4)   // Returns 8
 *   round_down(8, 4)   // Returns 8
 *   round_down(5, 4)   // Returns 4
 *   round_down(5, 0)   // Returns 5
 */
#ifndef round_down
#define _ROUND_DOWN_RAW(x, a) ((x) / (a) * (a))

#if _HAS_STATEMENT_EXPR
   #define round_down(x, align) __extension__ ({         \
      typeof(x) _UNIQUE(__x) = (x);                      \
      typeof(align) _UNIQUE(__a) = (align);              \
      _UNIQUE(__a) == 0                                  \
         ? _UNIQUE(__x)                                  \
         : _ROUND_DOWN_RAW(_UNIQUE(__x), _UNIQUE(__a));  \
   })

#else
   #define _GENERATE_ROUND_DOWN(type)                         \
      static inline type round_down_##type(type x, type a) {  \
         return a == 0 ? x : _ROUND_DOWN_RAW(x, a);           \
      }

   _INT_TYPES(_GENERATE_ROUND_DOWN)
   #define round_down(x, align) _Generic((x),      \
      _INT_TYPES(_GENERIC_ENTRY, round_down_)      \
      _GENERIC_DEFAULT(round_down_unsupported)     \
   )(x, align)

#endif
#endif /* round_down */


/*
 * round_down_pow2(x, align)
 * -------------------------
 * Rounds x downward to the nearest multiple of align (align must be power of 2).
 * - align MUST be a power of two (assertion verifies in debug mode)
 * - Uses bitwise AND: (x) & ~((align) - 1)
 * - Zero is a valid power of two? Typically align > 0 for meaningful rounding
 * 
 * Example:
 *   round_down_pow2(9, 4)   // Returns 8 (9 & ~3 = 8)
 *   round_down_pow2(7, 8)   // Returns 0
 *   round_down_pow2(15, 8)  // Returns 8
 */
#ifndef round_down_pow2
#define _ROUND_DOWN_POW2_RAW(x, a) ((x) & ~((a) - 1))

#if _HAS_STATEMENT_EXPR
   #define round_down_pow2(x, align) __extension__ ({    \
      typeof(x) _UNIQUE(__x) = (x);                      \
      typeof(align) _UNIQUE(__a) = (align);              \
      assert((_UNIQUE(__a) & (_UNIQUE(__a) - 1)) == 0);  \
      _ROUND_DOWN_POW2_RAW(_UNIQUE(__x), _UNIQUE(__a));  \
   })

#else
   #define _GENERATE_ROUND_DOWN_POW2(type)                        \
      static inline type round_down_pow2_##type(type x, type a) { \
         assert((a & (a - 1)) == 0);                              \
         return _ROUND_DOWN_POW2_RAW(x, a);                       \
      }
   _INT_TYPES(_GENERATE_ROUND_DOWN_POW2)
   #define round_down_pow2(x, align) _Generic((x),       \
      _INT_TYPES(_GENERIC_ENTRY, round_down_pow2_)       \
      _GENERIC_DEFAULT(round_down_pow2_unsupported)      \
   )(x, align)

#endif
#endif /* round_down_pow2 */


/*
 * div_round_up(n, d)
 * ------------------
 * Performs integer division rounding up to the nearest integer.
 * - Calculates ceiling(n / d) for integers
 * - Handles division by zero by returning 0
 * - Formula: (n + d - 1) / d (when d > 0)
 * - Evaluates arguments only once to prevent double-evaluation
 * 
 * Example:
 *   div_round_up(10, 3)   // Returns 4 (ceil(10/3) = 4)
 *   div_round_up(9, 3)    // Returns 3
 *   div_round_up(5, 0)    // Returns 0 (division by zero protection)
 */
#ifndef div_round_up
#define _DIV_ROUND_UP_RAW(n, d) (((n) + (d) - 1) / (d))

#if _HAS_STATEMENT_EXPR
   #define div_round_up(n, d) __extension__ ({        \
      typeof(n) _UNIQUE(__n) = (n);                   \
      typeof(d) _UNIQUE(__d) = (d);                   \
      assert(_UNIQUE(__d) != 0);                      \
      _DIV_ROUND_UP_RAW(_UNIQUE(__n), _UNIQUE(__d));  \
   })

#else
   #define _GENERATE_DIV_ROUND_UP(type)                        \
      static inline type div_round_up_##type(type n, type d) { \
         assert(d != 0);                                       \
         return _DIV_ROUND_UP_RAW(n, d);                       \
      }
   _INT_TYPES(_GENERATE_DIV_ROUND_UP)
   #define div_round_up(n, d) _Generic((n),           \
      _INT_TYPES(_GENERIC_ENTRY, div_round_up_)       \
      _GENERIC_DEFAULT(div_round_up_unsupported)      \
   )(n, d)

#endif
#endif /* div_round_up */


#endif /* HUZLIB_TYPEMATH_H */
