#ifndef _HUZLIB_TYPES_H
#define _HUZLIB_TYPES_H

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

/*
 * typeof(expr)
 * ------------
 * Retrieves the exact type of an expression at compile-time.
 */
#if (__STDC_VERSION__ <= 201710L) && !defined(typeof)
#if defined(__NVCC__) || defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__) || defined(__xlC__) || defined(__zig__) || defined(__TINYC__) || defined(__POCC__) || defined(__SUNPRO_C) || defined(__SUNPRO_CC) || defined(__clang__) || defined(__GNUC__)
   #define typeof(expr) __typeof__(expr)

#elif defined (_MSC_VER)
   #define typeof(expr) __decltype(expr)

#else
   #error "Cannot define typeof(expr)"

#endif
#endif /* typeof */


/*
 * typecheck(type, expr)
 * ---------------------
 * Static assert that 'expr' matches 'type' exactly.
 */
#if !defined(typecheck)
#if defined(__NVCC__) || defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__zig__) || defined(__clang__) || defined(__GNUC__)
   #define typecheck(type, expr) _Static_assert(            \
      __builtin_types_compatible_p(__typeof__(expr), type), \
      "typecheck(" #expr ", " #type ") failed"              \
   )

#else
   #define typecheck(type, expr) _Static_assert(   \
      _Generic((typeof(expr)){0},                  \
         type: 1,                                  \
         default: 0                                \
      ),                                           \
      "typecheck(" #expr ", " #type ") failed"     \
   )

#endif
#endif /* typecheck */


/*
 * typecheck_unqual(type, expr)
 * ----------------------------
 * Static assert that 'expr' is of base type 'type'.
 */
#if !defined(typecheck_unqual)
#if defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 20230000) || defined(__ARMCOMPILER_VERSION) && (__ARMCOMPILER_VERSION >= 130000) || defined(__clang__) && (__clang_major__ >= 13) || defined(__GNUC__) && (__GNUC__ >= 11)
   #define typecheck_unqual(type, expr) _Static_assert(              \
      __builtin_types_compatible_p(__typeof_unqual__(expr), type),   \
      "typecheck_unqual(" #expr ", " #type ") failed"                \
   )

#else
   #define typecheck_unqual(type, expr) _Static_assert(  \
      _Generic((typeof(expr)){0},                        \
         type: 1,                                        \
         const type: 1,                                  \
         volatile type: 1,                               \
         const volatile type: 1,                         \
         default: 0                                      \
      ),                                                 \
      "typecheck_unqual(" #expr ", " #type ") failed"    \
   )

#endif
#endif /* typecheck_unqual */


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


#if defined(__NVCC__) || defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__zig__) || defined(__TINYC__) || defined(__clang__) || defined(__GNUC__)
   #define container_of(ptr, type, member) __extension__ ({ \
      __typeof__(((type *)0)->member) *__m = (ptr);         \
      __container_of(__m, type, member);                    \
   })

#else
   #define container_of(ptr, type, member) typecheck_expr(  \
      typeof(((type *)0)->member) *, ptr,                   \
      __container_of(ptr, type, member)                     \
   )

#endif
#endif /* container_of */


#endif /* _HUZLIB_TYPES_H */
