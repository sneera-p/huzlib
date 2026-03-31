#ifndef _SNEERA_TYPES_H
#define _SNEERA_TYPES_H


/*
 * typeof(expr)
 * ------------
 * Retrieves the exact type of an expression at compile-time.
 */
#if (__STDC_VERSION__ <= 201710L) && !defined(typeof)
#if defined(__GNUC__) || defined(__clang__)|| defined(__POCC__) || defined(__TINYC__) || defined(__ICC) || defined(__INTEL_COMPILER)
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
#if defined(__GNUC__) || defined(__clang__)
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
#if defined(__GNUC__) || defined(__clang__)
   #define typecheck_unqual(type, expr) _Static_assert(                       \
      __builtin_types_compatible_p(__typeof__(__builtin_unqual(expr)), type), \
      "typecheck_unqual(" #expr ", " #type ") failed"                         \
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
   ),
   (expr)
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


#if defined(__GNUC__) || defined(__clang__) || defined(__POCC__) || defined(__TINYC__) || defined(__ICC) || defined(__INTEL_COMPILER)
   #define container_of(ptr, type, member) __extension__ ({  \
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


#endif /* _SNEERA_TYPES_H */
