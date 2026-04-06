#ifndef HUZLIB_TYPES_H
#define HUZLIB_TYPES_H

#include <stddef.h>
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
#if _HAS_STATEMENT_EXPR
   #define types_equal(typea, typeb) __extension__ ({ \
      (void)((typea *)0 == (typeb *)0);               \
      1;                                              \
   })
#else
   #define types_equal(typea, typeb) (                \
      sizeof((typea *)0 == (typeb *)0),               \
      1 \
   )
#endif
#endif /* types_equal */


/*
 * typecheck(type, expr)
 * ---------------------
 * Validates 'expr' matches 'type' exactly.
 */
#ifndef typecheck
#define typecheck(type, expr) _Generic(   \
   (expr),                                \
   type: 1,                               \
   const type: 1,                         \
   volatile type: 1,                      \
   const volatile type: 1                 \
)
#endif /* typecheck */


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
#define typecheck_expr(type, var, expr) (typecheck(type, var), (expr))
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
#if (__STDC_VERSION__ <= 202311L) && !defined(container_of)

/*
 * WARNING:
 * This macro is the internal implementation and should not be used directly.
 * Use container_of() instead which provides type checking when available.
 */
#define __container_of(ptr, type, member) \
   ((type *)((char*)(ptr) - offsetof(type, member)))


#if _HAS_STATEMENT_EXPR && _HAS_TYPEOF
   #define container_of(ptr, type, member) __extension__ ({       \
      __typeof__(((type *)0)->member) *_UNIQUE(__mptr) = (ptr);   \
      __container_of(_UNIQUE(__mptr), type, member);              \
   })

#else
   #define container_of(ptr, type, member) typecheck_expr(        \
      typeof(((type *)0)->member) *, (ptr),                       \
      __container_of(ptr, type, member)                           \
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
   typecheck(typeof(a), b);         \
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
#define swap(a, b) do {                \
   typeof(a) *_UNIQUE(__a) = &(a);     \
   typeof(b) *_UNIQUE(__b) = &(b);     \
   SWAP(*_UNIQUE(__a), *_UNIQUE(__b)); \
} while (0)
#endif



#endif /* HUZLIB_TYPES_H */
