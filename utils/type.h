#ifndef _SNEERA_TYPE_H
#define _SNEERA_TYPE_H

/*
 * typeof(expr)
 * ------------
 * Retrieves the type of an expression at compile-time.
 */
#if (__STDC_VERSION__ < 202311L) && !defined(typeof)
   #if defined(__GNUC__) || defined(__clang__)
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
 * Static assertion that 'expr' matches 'type' exactly.
 */
#if defined(__GNUC__) || defined(__clang__)
   #define typecheck(type, expr) _Static_assert(       \
      __builtin_types_compatible_p(typeof(expr), type),  \
      "typecheck(" #expr ", " #type ") failed"         \
   )

#else
   #define typecheck(type, expr) _Static_assert(       \
      _Generic((typeof(expr)){0}, type: 1, default: 0),  \
      "typecheck(" #expr ", " #type ") failed"         \
   )

#endif /* typecheck */


/*
 * typecheck_ptr(type, ptr, expr)
 * ------------------------------
 * Validates that 'ptr' is a pointer to 'type', then evaluates 'expr'.
 *
 * Features:
 *    - Transparent:          Evaluates to the result of 'expr'.
 *    - Qualifier Agnostic:   Accepts type*, const type*, volatile type*, and const volatile type*.
 *    - Safe sideeffects:     sideeffect expressions will be evaluated exactly once
 */
#define typecheck_ptr(type, ptr, expr) (  \
   (void)_Generic((ptr),                  \
      type *: 1,                          \
      const type *: 1,                    \
      volatile type *: 1,                 \
      const volatile type *: 1            \
   ),                                     \
   (expr)                                 \
)


#endif /* _SNEERA_TYPE_H */
