#ifndef _HUZLIB_TYPES_H
#define _HUZLIB_TYPES_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "xmacro.h"

/*
 * Use the following compiler detection order in macros to 
 * avoid my ass being riddled with spice trying to find which 
 * one of my braincells forgot to add a compiler and maintain 
 * Human(logn) search time for this particular ass ripping
 *
 * Order:
 *    defined(__NVCC__)
 *    defined(__INTEL_LLVM_COMPILER)
 *    defined(__INTEL_COMPILER)
 *    defined(__ARMCOMPILER_VERSION)
 *    defined(__ibmxl__)
 *    defined(__xlC__)
 *    defined(__zig__)
 *    defined(__TINYC__)
 *    defined(__POCC__)
 *    defined(__SUNPRO_C)
 *    defined(__SUNPRO_CC)
 *    defined(_MSC_VER)
 *    defined(__clang__)
 *    defined(__GNUC__)
 *
 * Prebuilt:
 *    defined(__NVCC__) || defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__) || defined(__xlC__) || defined(__zig__) || defined(__TINYC__) || defined(__POCC__) || defined(__SUNPRO_C) || defined(__SUNPRO_CC) || defined(_MSC_VER) || defined(__clang__) || defined(__GNUC__)
 */


#define _HAS_TYPEOF (                           \
   defined(__NVCC__) ||                         \
   defined(__INTEL_LLVM_COMPILER) ||            \
   defined(__INTEL_COMPILER) ||                 \
   defined(__ARMCOMPILER_VERSION) ||            \
   defined(__ibmxl__) ||                        \
   defined(__xlC__) ||                          \
   defined(__zig__) ||                          \
   defined(__TINYC__) ||                        \
   defined(__POCC__) ||                         \
   defined(__SUNPRO_C) ||                       \
   defined(__SUNPRO_CC) ||                      \
   (defined(_MSC_VER) && _MSC_VER >= 1938) ||   \
   defined(__clang__) ||                        \
   defined(__GNUC__)                            \
)

#define _HAS_TYPEOF_UNQUAL (                                                  \
   (defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 20230000)) || \
   (defined(__ARMCOMPILER_VERSION) && (__ARMCOMPILER_VERSION >= 130000)) ||   \
   (defined(_MSC_VER) && _MSC_VER >= 1938) ||                                 \
   (defined(__clang__) && (__clang_major__ >= 13)) ||                         \
   (defined(__GNUC__) && (__GNUC__ >= 11))                                    \
)

#define _HAS_DECLTYPE (                \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(_MSC_VER) ||                \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)

#define _HAS_TYPES_COMPATIBLE (        \
   defined(__NVCC__) ||                \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__zig__) ||                 \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)

#define _HAS_STATEMENT_EXPR (          \
   defined(__NVCC__) ||                \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__zig__) ||                 \
   defined(__TINYC__) ||               \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)




/* _UNIQUE(a)
 * -------------------
 * internal cancatation utility used to
 * create unique tmp varaible name
 *
 * WARN:
 * This macro is the internal implementation and should not be used directly.
 */ 
#define __UNIQUE_CONCAT_INTERNAL(a, b) a##b
#define __UNIQUE_CONCAT(a, b) __UNIQUE_CONCAT_INTERNAL(a, b)
#define _UNIQUE(name) __UNIQUE_CONCAT(name, __LINE__)


#define _NUMERIC_TYPES(x, ...)         \
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
   x(float, __VA_ARGS__)               \
   x(double, __VA_ARGS__)              \
   x(long double, __VA_ARGS__)





/*
 * typeof(expr)
 * ------------
 * Retrieves the exact type of an expression at compile-time.
 */
#if (__STDC_VERSION__ <= 201710L) && !defined(typeof)
#if _HAS_TYPEOF
   #define typeof(expr) __typeof__(expr)

#elif _HAS_DECLTYPE
   #define typeof(expr) __decltype(expr)

#else
   #error "Cannot define typeof(expr)"

#endif
#endif /* typeof */


/*
 * typeof_member(type, member)
 * ---------------------------
 * Retrieves exact type of type->member
 */
#ifndef typeof_member
#define typeof_member(type, member) typeof(((type)0)->member)
#endif /* typeof_member */


/*
 * types_equal(typea, typeb)
 * -------------------------
 * Validates types 'typea' and 'typeb' are the same
 */ 
#ifndef types_equal
#if _HAS_TYPES_COMPATIBLE
   #define types_equal(typea, typeb) __builtin_types_compatible_p(typea, typeb)

#else
   #define types_equal(typea, typeb) _Generic(  \
      (typea){0},                               \
      typeb: 1,                                 \
      default: 0                                \
   )

#endif
#endif /* types_equal */


/*
 * typecheck(type, expr)
 * ---------------------
 * Validates 'expr' matches 'type' exactly.
 */
#ifndef typecheck
#if _HAS_TYPES_COMPATIBLE && _HAS_TYPEOF
   #define typecheck(type, expr) __builtin_types_compatible_p(__typeof__(expr), type)

#else
   #define typecheck(type, expr) _Generic(   \
      (typeof(expr)){0},                     \
      type: 1,                               \
      default: 0                             \
   )

#endif
#endif /* typecheck */


/*
 * typecheck_unqual(type, expr)
 * ----------------------------
 * Validates 'expr' is of base type 'type'.
 */
#ifndef typecheck_unqual
#if _HAS_TYPES_COMPATIBLE && _HAS_TYPEOF_UNQUAL
   #define typecheck_unqual(type, expr) __builtin_types_compatible_p(__typeof_unqual__(expr), type)

#else
   #define typecheck_unqual(type, expr) _Generic(  \
      (typeof(expr)){0},                           \
      type: 1,                                     \
      const type: 1,                               \
      volatile type: 1,                            \
      const volatile type: 1,                      \
      default: 0                                   \
   )

#endif
#endif /* typecheck_unqual */


/*
 * typecheck_ptr(type, ptr)
 * -------------------------------
 * Validates that 'ptr' is of 'type*'.
 */
#ifndef typecheck_ptr
#define typecheck_ptr(type, ptr) (void)_Generic(   \
   (ptr),                                          \
   type *: 1,                                      \
   type *const: 1,                                 \
   type *volatile: 1,                              \
   type *const volatile: 1,                        \
   const type *: 1,                                \
   const type *const: 1,                           \
   const type *volatile: 1,                        \
   const type *const volatile: 1,                  \
   volatile type *: 1,                             \
   volatile type *const: 1,                        \
   volatile type *volatile: 1,                     \
   volatile type *const volatile: 1,               \
   const volatile type *: 1,                       \
   const volatile type *const: 1,                  \
   const volatile type *volatile: 1,               \
   const volatile type *const volatile: 1,         \
   default: 0                                      \
)
#endif /* typecheck_ptr */


/*
 * typecheck_expr(type, var, expr)
 * -------------------------------
 * Validates that 'var' is of 'type', then evaluates 'expr'.
 *
 * Features:
 *    - Qualifier Agnostic:   Accepts type, const type, volatile type, and const volatile type.
 *    - Safe sideeffects:     sideeffect expressions will be evaluated exactly once
 */
#ifndef typecheck_expr
#define typecheck_expr(type, var, expr) ( \
   (void)_Generic((var),                  \
      type: 1,                            \
      const type: 1,                      \
      volatile type: 1,                   \
      const volatile type: 1              \
   ),                                     \
   (expr)                                 \
)
#endif /* typecheck_expr */




/*
 * container_of(ptr, type, member)
 * -------------------------------
 * cast a member of a structure out to the containing structure
 *
 * @ptr:    pointer to member
 * @type:   type of the container struct member is embedded in
 * @member: the name of the member field
 *
 * Return: pointer to the containing structure
 */
#if (__STDC_VERSION__ <= 202311L) && !defined(containerof)
#include <stddef.h>

/*
 * WARNING:
 * This macro is the internal implementation and should not be used directly.
 * Use container_of() instead which provides type checking when available.
 */
#define __container_of(ptr, type, member) \
   ((type *)((char*)(ptr) - offsetof(type, member)))


#if _HAS_STATEMENT_EXPR && _HAS_TYPEOF
   #define container_of(ptr, type, member) __extension__ ({ \
      __typeof__(((type *)0)->member) *_UNIQUE(_m) = (ptr); \
      __container_of(_UNIQUE(_m), type, member);            \
   })

#else
   #define container_of(ptr, type, member) typecheck_expr(  \
      typeof(((type *)0)->member) *, ptr,                   \
      __container_of(ptr, type, member)                     \
   )

#endif
#endif /* container_of */


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
#ifndef SWAP
#define SWAP(a, b) do {             \
   typecheck_unqual(typeof(a), b);  \
   typeof(a) _UNIQUE(__tmp) = a;    \
   a = b;                           \
   b = _UNIQUE(__tmp);              \
} while (0)
#endif


/*
 * swap(a, b)
 * ----------
 * Swaps the values of variables @a and @b using temporary variable pointers
 *
 * This macro works with any data type, including structures and unions, as
 * long as assignment is supported. It does not require the two variables to
 * be of exactly the same type, but they must be assignment-compatible.
 *
 * NOTE:
 * This macro is sideeffect-free unlike it's cousin SWAP(a, b).
 * But it might be slow in some contexts. Use SWAP(...) when possible
 */
#ifndef swap
#define swap(a, b) do {                      \
   typecheck_unqual(typeof(a), b);           \
   typeof(a) *_UNIQUE(__a) = &(a);           \
   typeof(b) *_UNIQUE(__b) = &(b);           \
   typeof(a) _UNIQUE(__tmp) = *_UNIQUE(__a); \
   *_UNIQUE(__a) = *_UNIQUE(__b);            \
   *_UNIQUE(__b) = _UNIQUE(__tmp);           \
} while (0)
#endif




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
   #define min(x, y) __extension__ ({                   \
      typecheck_unqual(typeof(x), y);     \
      typeof(y) _UNIQUE(_x) = (x);        \
      typeof(x) _UNIQUE(_y) = (y);        \
      _MIN_RAW(_UNIQUE(_x), _UNIQUE(_y)); \
   })

#else
   #define _GENERATE_MIN(type)                        \
      static inline type min_##type(type x, type y) { \
         return _MIN_RAW(x, y);                       \
      }

   _NUMERIC_TYPES(_GENERATE_MIN)
   #define min(x, y) _Generic((x),                    \
      typeof(y): _Generic((y),                        \
         _NUMERIC_TYPES(XMACRO_GENERIC_ENTRY, min_)   \
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
   #define max(x, y) __extension__ ({                   \
      typecheck_unqual(typeof(x), y);     \
      typeof(y) _UNIQUE(_x) = (x);        \
      typeof(x) _UNIQUE(_y) = (y);        \
      _MAX_RAW(_UNIQUE(_x), _UNIQUE(_y)); \
   })

#else
   #define _GENERATE_MAX(type)                        \
      static inline type max_##type(type x, type y) { \
         return _MAX_RAW(x, y);                       \
      }

   _NUMERIC_TYPES(_GENERATE_MAX)
   #define max(x, y) _Generic((x),                    \
      typeof(y): _Generic((y),                        \
         _NUMERIC_TYPES(XMACRO_GENERIC_ENTRY, max_)   \
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
   #define round_up(x, align) __extension__ ({  \
      typeof(x) _UNIQUE(_x) = (x);              \
      typeof(align) _UNIQUE(_a) = (align);      \
      _ROUND_UP_RAW(_UNIQUE(_x), _UNIQUE(_a));  \
   })

#else
   #define _GENERATE_ROUND_UP(type)                         \
      static inline type round_up_##type(type x, type a) {  \
         return _ROUND_UP_RAW(x, a);                        \
      }

   _NUMERIC_TYPES(_GENERATE_ROUND_UP)
   #define round_up(x, align) _Generic((x),           \
      _NUMERIC_TYPES(XMACRO_GENERIC_ENTRY, round_up_) \
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
   #define round_up_pow2(x, align) __extension__ ({   \
      typeof(x) _UNIQUE(_x) = (x);                    \
      typeof(align) _UNIQUE(_a) = (align);            \
      assert((_UNIQUE(_a) & (_UNIQUE(_a) - 1)) == 0); \
      _ROUND_UP_POW2_RAW(_UNIQUE(_x), _UNIQUE(_a));   \
   })

#else
   #define _GENERATE_ROUND_UP_POW2(type)                          \
      static inline type round_up_pow2_##type(type x, type a) {   \
         assert((a & (a - 1)) == 0);                              \
         return _ROUND_UP_POW2_RAW(x, a);                         \
      }

   _NUMERIC_TYPES(_GENERATE_ROUND_UP_POW2)
   #define round_up_pow2(x, align) _Generic((x),      \
      _NUMERIC_TYPES(_GENERIC_ENTRY, round_up_pow2_)  \
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
   #define round_down(x, align) __extension__ ({      \
      typeof(x) _UNIQUE(_x) = (x);                    \
      typeof(align) _UNIQUE(_a) = (align);            \
      _UNIQUE(_a) == 0                                \
         ? _UNIQUE(_x)                                \
         : _ROUND_DOWN_RAW(_UNIQUE(_x), _UNIQUE(_a)); \
   })

#else
   #define _GENERATE_ROUND_DOWN(type)                         \
      static inline type round_down_##type(type x, type a) {  \
         return a == 0 ? x : _ROUND_DOWN_RAW(x, a);           \
      }

   _NUMERIC_TYPES(_GENERATE_ROUND_DOWN)
   #define round_down(x, align) _Generic((x),      \
      _NUMERIC_TYPES(_GENERIC_ENTRY, round_down_)  \
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
   #define round_down_pow2(x, align) __extension__ ({ \
      typeof(x) _x = (x);                             \
      typeof(align) _a = (align);                     \
      assert((_UNIQUE(_a) & (_UNIQUE(_a) - 1)) == 0); \
      _ROUND_DOWN_POW2_RAW(_UNIQUE(_x), _UNIQUE(_a)); \
   })

#else
   #define _GENERATE_ROUND_DOWN_POW2(type)                        \
      static inline type round_down_pow2_##type(type x, type a) { \
         assert((a & (a - 1)) == 0);                              \
         return _ROUND_DOWN_POW2_RAW(x, a);                       \
      }
   _NUMERIC_TYPES(_GENERATE_ROUND_DOWN_POW2)
   #define round_down_pow2(x, align) _Generic((x),       \
      _NUMERIC_TYPES(_GENERIC_ENTRY, round_down_pow2_)   \
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
   #define div_round_up(n, d) __extension__ ({           \
      typeof(n) _UNIQUE(_n) = (n);                       \
      typeof(d) _UNIQUE(_d) = (d);                       \
      _UNIQUE(_d) == 0                                   \
         ? 0                                             \
         : _DIV_ROUND_UP_RAW(_UNIQUE(_n), _UNIQUE(_d));  \
   })

#else
   #define _GENERATE_DIV_ROUND_UP(type)                        \
      static inline type div_round_up_##type(type n, type d) { \
         return d == 0 ? 0 : _DIV_ROUND_UP_RAW(n, d);          \
      }
   _NUMERIC_TYPES(_GENERATE_DIV_ROUND_UP)
   #define div_round_up(n, d) _Generic((n),           \
      _NUMERIC_TYPES(_GENERIC_ENTRY, div_round_up_)   \
      _GENERIC_DEFAULT(div_round_up_unsupported)      \
   )(n, d)

#endif
#endif /* div_round_up */


/*
 * div_round_up_pow2(n, d)
 * -----------------------
 * Performs integer division rounding up where divisor is a power of two.
 * - d MUST be a power of two (assertion checks this)
 * - Uses __builtin_ctz (count trailing zeros) for optimization
 * - Formula: (n + d - 1) >> log2(d)
 * - Significantly faster than generic div_round_up for power-of-two divisors
 * 
 * Example:
 *   div_round_up_pow2(10, 4)   // Returns 3 (ceil(10/4) = 3)
 *   div_round_up_pow2(8, 4)    // Returns 2
 *   div_round_up_pow2(9, 8)    // Returns 2 (ceil(9/8) = 2)
 */
#ifndef div_round_up_pow2
#define _DIV_ROUND_UP_POW2_RAW(n, d) (((n) + (d) - 1) >> __builtin_ctz(d))

#if _HAS_STATEMENT_EXPR
   #define div_round_up_pow2(n, d) __extension__ ({      \
      typeof(n) _UNIQUE(_n) = (n);                       \
      typeof(d) _UNIQUE(_d) = (d);                       \
      assert((_UNIQUE(_d) & (_UNIQUE(_d) - 1)) == 0);    \
      _DIV_ROUND_UP_POW2_RAW(_UNIQUE(_n), _UNIQUE(_d));  \
   })

#else
   #define _GENERATE_DIV_ROUND_UP_POW2(type)                         \
      static inline type div_round_up_pow2_##type(type n, type d) {  \
         assert((a & (a - 1)) == 0);                                 \
         return _DIV_ROUND_UP_POW2_RAW(n, d);                        \
      }
   _NUMERIC_TYPES(_GENERATE_DIV_ROUND_UP_POW2)
   #define div_round_up_pow2(n, d) _Generic((n),         \
      _NUMERIC_TYPES(_GENERIC_ENTRY, div_round_up_pow2_) \
      _GENERIC_DEFAULT(div_round_up_pow2_unsupported)    \
   )(n, d)

#endif

#endif /* div_round_up_pow2 */


#endif /* _HUZLIB_TYPES_H */
