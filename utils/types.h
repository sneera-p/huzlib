/*
 * Use the following compiler detection order in macros to 
 * avoid my ass being riddled with spice trying to find which 
 * one of my braincells forgot to add a compiler and maintain 
 * Human(logn) search time for this particular ass ripping
 *
 * Order:
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


#ifndef HUZ_INTERNAL_HAS_TYPEOF
#if (                                           \
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
   (defined(_MSC_VER) && (_MSC_VER >= 1938)) || \
   defined(__clang__) ||                        \
   defined(__GNUC__)                            \
)
   #define HUZ_INTERNAL_HAS_TYPEOF 1
#else
   #define HUZ_INTERNAL_HAS_TYPEOF 0
#endif
#endif /* HUZ_INTERNAL_HAS_TYPEOF */



#ifndef HUZ_INTERNAL_HAS_TYPEOF_UNQUAL
#if (                                                                         \
   (defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 20230000)) || \
   (defined(__ARMCOMPILER_VERSION) && (__ARMCOMPILER_VERSION >= 130000)) ||   \
   (defined(_MSC_VER) && (_MSC_VER >= 1938)) ||                               \
   (defined(__clang__) && (__clang_major__ >= 13)) ||                         \
   (defined(__GNUC__) && (__GNUC__ >= 11))                                    \
)
   #define HUZ_INTERNAL_HAS_TYPEOF_UNQUAL 1
#else
   #define HUZ_INTERNAL_HAS_TYPEOF_UNQUAL 0
#endif
#endif /* HUZ_INTERNAL_HAS_TYPEOF_UNQUAL */



#ifndef HUZ_INTERNAL_HAS_DECLTYPE
#if (                                  \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(_MSC_VER) ||                \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)
   #define HUZ_INTERNAL_HAS_DECLTYPE 1
#else
   #define HUZ_INTERNAL_HAS_DECLTYPE 0
#endif
#endif /* HUZ_INTERNAL_HAS_DECLTYPE */



#ifndef HUZ_INTERNAL_HAS_TYPES_COMPATIBLE
#if (                                  \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__zig__) ||                 \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)
   #define HUZ_INTERNAL_HAS_TYPES_COMPATIBLE 1
#else
   #define HUZ_INTERNAL_HAS_TYPES_COMPATIBLE 0
#endif
#endif /* HUZ_INTERNAL_HAS_TYPES_COMPATIBLE */



#ifndef HUZ_INTERNAL_HAS_STATEMENT_EXPR
#if (                                  \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__zig__) ||                 \
   defined(__TINYC__) ||               \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)
   #define HUZ_INTERNAL_HAS_STATEMENT_EXPR 1
#else
   #define HUZ_INTERNAL_HAS_STATEMENT_EXPR 0
#endif
#endif /* HUZ_INTERNAL_HAS_STATEMENT_EXPR */



/* 
 * huz_unique(name)
 * ----------------
 * internal cancatation utility used to
 * create unique tmp varaible name
 *
 * WARN:
 * This macro is the internal implementation and should not be used directly.
 */
#ifndef huz_unique
#define __huz_unique_cat__(a, b) a##b
#define __huz_unique_cat(a, b)   __huz_unique_cat__(a, b)
#define huz_unique(name)         __huz_unique_cat(name, __LINE__)
#endif /* huz_unique */



/*
 * huz_typeof(expr)
 * ----------------
 * Retrieves the exact type of an expression at compile-time.
 */
#if (__STDC_VERSION__ >= 202311L) || defined(typeof)
   #define huz_typeof(expr)   typeof(expr)

#elif HUZ_INTERNAL_HAS_TYPEOF
   #define huz_typeof(expr)    __typeof__(expr)

#elif HUZ_INTERNAL_HAS_DECLTYPE
   #define huz_typeof(expr)    __decltype(expr)

#else
   #error "Cannot define huz_typeof(expr)"

#endif /* huz_typeof */



/*
 * huz_typeof_member(type, member)
 * -------------------------------
 * Retrieves exact type of type->member
 */
#ifndef huz_typeof_member
#define huz_typeof_member(type, member) huz_typeof(((type *)0)->member)
#endif /* huz_typeof_member */



/*
 * huz_sizeof_member(type, member)
 * -------------------------------
 * Retrieves exact size of type->member
 */
#ifndef huz_sizeof_member
#define huz_sizeof_member(type, member) sizeof(huz_typeof_member(type, member))
#endif /* huz_sizeof_member */



/*
 * huz_alignof_member(type, member)
 * --------------------------------
 * Retrieves exact align of type->member
 */
#ifndef huz_alignof_member
#include <stdalign.h>
#define huz_alignof_member(type, member) alignof(huz_typeof_member(type, member))
#endif /* huz_alignof_member */


 
/*
 * huz_types_equal(typea, typeb)
 * -----------------------------
 * Compile-time check: 1 if the types are compatible, else breaks compile job.
 *
 * SEMANTICS:
 *   Matches on C type compatibility (C11 §6.2.7), not on size or representation.
 *   Implemented as _Generic((typea *)0, typeb *: 1);
 *   wrapping both types in a pointer means only the pointer itself is subject
 *   to _Generic's lvalue conversion, so qualifiers on the types are compared.
 *
 *   Returns 1:
 *     • Identical types:              (int, int)
 *     • Alternate spellings:          (int, signed int), (long, long int)
 *     • Typedef aliases:              (int, int32_t)  -- platform-dependent
 *     • Enum and its underlying type: (enum e, int)   -- implementation-defined
 *     • Incomplete vs complete array: (int[], int[3])
 *
 *   Breaks Compile Time:
 *     • Distinct integer types, even with equal size or signedness:
 *         (int, short), (int, unsigned), (int, long), (char, signed char)
 *     • Differing qualifiers:         (const int, int), (volatile int, int)
 *     • Arrays of different length:   (int[2], int[3])
 *
 * NOTE:
 *   Fixed-width typedefs vary by platform (int32_t is int on glibc, long on
 *   some embedded targets). Compare them to themselves, not to base types.
 *
 * EXAMPLE:
 *   _Static_assert(huz_type_equal(int, int32_t), "need int32_t == int");
 */
#ifndef huz_types_equal
#define huz_types_equal(typea, typeb)  \
   (void)_Generic((typea *)0,          \
      typeb *: 1                       \
   )
#endif /* huz_types_equal */



/*
 * huz_typecheck(type, expr)
 * -------------------------
 * Validates 'expr' matches 'type'.
 *
 * NOTE:
 * 'type' parameter entered must an unqualified type
 */
#ifndef huz_typecheck
#define huz_typecheck(type, expr) huz_types_equal(type, huz_typeof(expr))
#endif /* huz_typecheck */



/*
 * huz_typecheck_then(type, expr, result)
 * --------------------------------------
 * Validates that 'expr' is of 'type', then evaluates 'result'.
 */
#ifndef huz_typecheck_then
#define huz_typecheck_then(type, expr, result) ((void)huz_typecheck(type, expr), (result))
#endif /* huz_typecheck_then */



/*
 * huz_typecheck_member(mtype, vartype, member)
 * --------------------------------------------
 * Validated 'vartype->member' matches 'mtype'
 *
 * NOTE:
 *  'mtype' parameter entered must an unqualified type
 */
#ifndef huz_typecheck_member
#define huz_typecheck_member(mtype, vartype, member) huz_types_equal(mtype, huz_typeof_member(type, member))
#endif /* huz_typecheck_member */



/*
 * huz_requal_expr(ptr, type, expr)
 * --------------------------------
 * Restores CV-qualifiers from ptr onto type, then casts expr to the result.
 *
 * Branch order is intentional: volatile is checked before const so that
 * const volatile pointers match the volatile branch, returning volatile type *
 * with const silently dropped.
 *
 * This is the safer default for const volatile because:
 *   - volatile drop: silently causes missed hardware reads/writes, a library bug
 *   - const drop:    programmer may write through the pointer, a programmer error
 *                    the compiler may still catch via other diagnostics
 *
 * const volatile is almost exclusively used on memory-mapped hardware registers
 * which are inherently writable, so dropping const is less dangerous in practice.
 *
 * NOTE: To preserve const instead of volatile for const volatile pointers,
 * move the const branch above the volatile branch.
 */
#ifndef huz_requal_expr
#define huz_requal_expr(ptr, type, expr) _Generic((ptr),       \
   volatile huz_typeof(*(ptr)) *: ((volatile type *)(expr)),   \
   const huz_typeof(*(ptr)) *:    ((const type *)(expr)),      \
   default:                   ((type *)(expr))                 \
)
#endif /* huz_requal_expr */



/*
 * huz_container_of(ptr, type, member)
 * -----------------------------------
 * cast a member of a structure out to the containing structure
 *
 * @ptr:    pointer to member
 * @type:   type of the container struct member is embedded in
 * @member: the name of the member field
 *
 * Return: pointer to the containing structure
 */
#if (__STDC_VERSION__ > 202311L) || defined(container_of)
   #define huz_container_of(ptr, type, member)  container_of((ptr), (type), (member))

#else
/*
 * __huz_container_of_raw(ptr, type, member)
 * -----------------------------------------
 * Computes a pointer to the containing structure by subtracting the offset
 * of member from ptr. No type checking is performed.
 *
 * WARNING:
 * This macro is the internal implementation and should not be used directly.
 */
#include <stddef.h>
#define __huz_container_of_raw(ptr, type, member) \
   ((type *)((char *)(ptr) - offsetof(type, member)))


/*
 * __huz_container_of_unqual(ptr, type, member)
 * --------------------------------------------
 * Type-checked wrapper around __huz_container_of_raw() that verifies ptr points
 * to the correct member type before performing the offset arithmetic.
 *
 * Returns a bare type * with no CV-qualifiers preserved. Use huz_container_of()
 * which wraps this with huz_requal_expr() to restore qualifiers on the result.
 *
 * WARNING:
 * This macro is the internal implementation and should not be used directly.
 */
#if HUZ_INTERNAL_HAS_STATEMENT_EXPR
   #define __huz_container_of_unqual(ptr, type, member) __extension__ ({      \
      const huz_typeof_member(type, member) *__mcumptr = (ptr);               \
      __huz_container_of_raw(__mcumptr, type, member);                        \
   })

#else
   #define __huz_container_of_unqual(ptr, type, member) huz_typecheck_then(   \
      huz_typeof_member(type, member), *(ptr),                                \
      __huz_container_of_raw(ptr, type, member)                               \
   )

#endif


#define huz_container_of(ptr, type, member) \
   huz_requal_expr(ptr, type, __huz_container_of_unqual(ptr, type, member))

#endif /* huz_container_of */



/*
 * HUZ_SWAP(a, b)
 * --------------
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
#ifndef HUZ_SWAP
#define HUZ_SWAP(a, b) do {               \
   huz_types_equal(a, b);                 \
   huz_typeof(a) huz_unique(__tmp) = a;   \
   a = b;                                 \
   b = huz_unique(__tmp);                 \
} while (0)
#endif /* HUZ_SWAP */



/*
 * huz_swap(a, b)
 * --------------
 * Swaps the values of variables @a and @b using temporary variable pointers
 *
 * This macro works with any data type, including structures and unions, as
 * long as assignment is supported. It does not require the two variables to
 * be of exactly the same type, but they must be assignment-compatible.
 *
 * NOTE:
 * This macro is sideeffect-free unlike it's cousin HUZ_SWAP(a, b).
 * But it might be slow in some contexts. Use HUZ_SWAP(...) when possible
 */
#ifndef huz_swap
#define huz_swap(a, b) do {                        \
   huz_typeof(a) *huz_unique(__a) = &(a);          \
   huz_typeof(b) *huz_unique(__b) = &(b);          \
   HUZ_SWAP(*huz_unique(__a), *huz_unique(__b));   \
} while (0)
#endif /* huz_swap */



/*
 * huz_tmpptr(type, ...)
 * ---------------------
 * INTENT:
 *   Standard C prohibits taking the address of rvalues or temporary expressions
 *   (e.g., `&42` or `&(x + 1)` is a compile error). This macro materializes one
 *   or more values into a temporary, stack-allocated array and returns a pointer
 *   to its first element. It enables clean, expression-style interoperability with
 *   pointer-expecting APIs without requiring named local variables or statement
 *   blocks (`do { ... } while(0)`).
 *
 * HOW IT WORKS:
 *   Leverages C99 compound literals `((type[]){ __VA_ARGS__ })` to create an
 *   anonymous array with automatic storage duration. Per ISO C semantics, the
 *   array expression automatically decays to a `type *` pointing to index 0.
 *   The memory is provisioned in the current stack frame and requires no manual
 *   cleanup.
 *
 * LIFETIME & SAFETY:
 *   The underlying array has AUTOMATIC storage duration, strictly bound to the
 *   nearest enclosing block scope `{ }`.
 *
 *   ✓ SAFE USE:
 *     • Passing directly to functions that read or copy the data immediately
 *       before returning (e.g., `memcpy`, `stack_push`, `send`, serialization).
 *     • Inline buffer initialization for immediate consumption.
 *
 *   ✗ UNSAFE USE (Undefined Behavior):
 *     • Storing the pointer in a variable for later use.
 *     • Returning the pointer from a function.
 *     • Assigning to a struct field, global, or heap-allocated memory.
 *     • Any access that outlives the enclosing `{ }` block → dangling pointer.
 *
 * USAGE NOTES:
 *   • ISO C requires at least one initializer (empty `__VA_ARGS__` fails to compile).
 *   • Each argument is evaluated exactly once during macro expansion.
 *   • If a single initializer contains commas (struct literals, comma operators,
 *     or function-like macro expansions), wrap it in parentheses to prevent it
 *     from being parsed as multiple array elements.
 *     Example: `tmpptr(struct point, (struct point){ .x = 1, .y = 2 })`
 *   • The result decays to `type *` and is fully interchangeable with pointer
 *     parameters. `void f(type arr[])` and `void f(type *p)` are identical.
 *
 * EXAMPLES:
 *   void push_bytes(const void *data, size_t len);
 *
 *   // ✓ Safe: Consumed immediately
 *   push_bytes(huz_tmpptr(int, 42), sizeof(int));
 *   push_bytes(huz_tmpptr(double, 1.0, 2.0, 3.0), 3 * sizeof(double));
 *
 *   // ✗ WRONG: Pointer escapes block scope → Undefined Behavior
 *   int *p = huz_tmpptr(int, 10);
 *   return huz_tmpptr(char, 'A');
 */
#ifndef huz_tmpptr
#define huz_tmpptr(type, ...) ((type[]) { __VA_ARGS__ })
#endif /* huz_tmpptr */

