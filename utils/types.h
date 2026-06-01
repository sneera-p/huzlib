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


#ifndef HUZLIB_INTERNAL_HAS_TYPEOF
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
   #define HUZLIB_INTERNAL_HAS_TYPEOF 1
#else
   #define HUZLIB_INTERNAL_HAS_TYPEOF 0
#endif
#endif /* HUZLIB_INTERNAL_HAS_TYPEOF */



#ifndef HUZLIB_INTERNAL_HAS_TYPEOF_UNQUAL
#if (                                                                         \
   (defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 20230000)) || \
   (defined(__ARMCOMPILER_VERSION) && (__ARMCOMPILER_VERSION >= 130000)) ||   \
   (defined(_MSC_VER) && (_MSC_VER >= 1938)) ||                               \
   (defined(__clang__) && (__clang_major__ >= 13)) ||                         \
   (defined(__GNUC__) && (__GNUC__ >= 11))                                    \
)
   #define HUZLIB_INTERNAL_HAS_TYPEOF_UNQUAL 1
#else
   #define HUZLIB_INTERNAL_HAS_TYPEOF_UNQUAL 0
#endif
#endif /* HUZLIB_INTERNAL_HAS_TYPEOF_UNQUAL */



#ifndef HUZLIB_INTERNAL_HAS_DECLTYPE
#if (                                  \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(_MSC_VER) ||                \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)
   #define HUZLIB_INTERNAL_HAS_DECLTYPE 1
#else
   #define HUZLIB_INTERNAL_HAS_DECLTYPE 0
#endif
#endif /* HUZLIB_INTERNAL_HAS_DECLTYPE */



#ifndef HUZLIB_INTERNAL_HAS_TYPES_COMPATIBLE
#if (                                  \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__zig__) ||                 \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)
   #define HUZLIB_INTERNAL_HAS_TYPES_COMPATIBLE 1
#else
   #define HUZLIB_INTERNAL_HAS_TYPES_COMPATIBLE 0
#endif
#endif /* HUZLIB_INTERNAL_HAS_TYPES_COMPATIBLE */



#ifndef HUZLIB_INTERNAL_HAS_STATEMENT_EXPR
#if (                                  \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__zig__) ||                 \
   defined(__TINYC__) ||               \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)
   #define HUZLIB_INTERNAL_HAS_STATEMENT_EXPR 1
#else
   #define HUZLIB_INTERNAL_HAS_STATEMENT_EXPR 0
#endif
#endif /* HUZLIB_INTERNAL_HAS_STATEMENT_EXPR */



/* __huzuq(a)
 * -------------------
 * internal cancatation utility used to
 * create unique tmp varaible name
 *
 * WARN:
 * This macro is the internal implementation and should not be used directly.
 */
#ifndef __huzuq
#define HUZLIB_UNIQUE_CONCAT_INTERNAL(a, b) a##b
#define HUZLIB_UNIQUE_CONCAT(a, b) HUZLIB_UNIQUE_CONCAT_INTERNAL(a, b)
#define __huzuq(name) HUZLIB_UNIQUE_CONCAT(name, __LINE__)
#endif /* __huzuq */



/*
 * typeof(expr)
 * ------------
 * Retrieves the exact type of an expression at compile-time.
 */
#if (__STDC_VERSION__ <= 201710L) && !defined(typeof)
#if HUZLIB_INTERNAL_HAS_TYPEOF
   #define typeof(expr) __typeof__(expr)

#elif HUZLIB_INTERNAL_HAS_DECLTYPE
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
#define typeof_member(type, member) typeof(((type *)0)->member)
#endif /* typeof_member */



/*
 * sizeof_member(type, member)
 * ---------------------------
 * Retrieves exact size of type->member
 */
#ifndef sizeof_member
#define sizeof_member(type, member) sizeof(typeof_member(type, member))
#endif /* sizeof_member */



/*
 * alignof_member(type, member)
 * ---------------------------
 * Retrieves exact align of type->member
 */
#ifndef alignof_member
#include <stdalign.h>
#define alignof_member(type, member) alignof(typeof_member(type, member))
#endif /* alignof_member */



/*
 * types_equal(typea, typeb)
 * -------------------------
 * Validates types 'typea' and 'typeb' are the same
 */ 
#ifndef types_equal
#if HUZLIB_INTERNAL_HAS_STATEMENT_EXPR
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
 * Validates 'expr' matches 'type'.
 *
 * NOTE:
 * 'type' parameter entered must an unqualified type
 */
#ifndef typecheck
#define typecheck(type, expr) ((void)_Generic(  \
   (expr),                                      \
   type: 1                                      \
))
#endif /* typecheck */



/* typecheck_member(mtype, vartype, member)
 * -----------------------------------
 * Validated 'vartype->member' matches 'mtype'
 *
 * NOTE:
 * 'mtype' parameter entered must an unqualified type
 */
#ifndef typecheck_member
#define typecheck_member(mtype, vartype, member) typecheck(mtype, ((vartype *)0)->member)
#endif /* typecheck_member */



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
#define typecheck_expr(type, var, expr) ((void)typecheck(type, var), (expr))
#endif /* typecheck_expr */



/*
 * __requal_expr(ptr, type, expr)
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
#ifndef __requal_expr
#define __requal_expr(ptr, type, expr) _Generic((ptr),               \
   volatile typeof(*(ptr)) *: ((volatile type *)(expr)),             \
   const typeof(*(ptr)) *:    ((const type *)(expr)),                \
   default:                   ((type *)(expr))                       \
)
#endif /* __requal_expr */



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
#if !defined(container_of) && (__STDC_VERSION__ <= 202311L) 

/*
 * __container_of_raw(ptr, type, member)
 * --------------------------------------
 * Computes a pointer to the containing structure by subtracting the offset
 * of member from ptr. No type checking is performed.
 *
 * WARNING:
 * This macro is the internal implementation and should not be used directly.
 */
#include <stddef.h>
#define __container_of_raw(ptr, type, member) \
   ((type *)((char *)(ptr) - offsetof(type, member)))


/*
 * __container_of_unqual(ptr, type, member)
 * -----------------------------------------
 * Type-checked wrapper around __container_of_raw() that verifies ptr points
 * to the correct member type before performing the offset arithmetic.
 *
 * Returns a bare type * with no CV-qualifiers preserved. Use container_of()
 * which wraps this with __requal_expr() to restore qualifiers on the result.
 *
 * WARNING:
 * This macro is the internal implementation and should not be used directly.
 */
#if HUZLIB_INTERNAL_HAS_STATEMENT_EXPR
   #define __container_of_unqual(ptr, type, member) __extension__ ({ \
      const typeof(((type *)0)->member) *__mcumptr = (ptr);          \
      __container_of_raw(__mcumptr, type, member);                   \
   })

#else
   #define __container_of_unqual(ptr, type, member) typecheck_expr(  \
      typeof(((type *)0)->member), *(ptr),                           \
      __container_of_raw(ptr, type, member)                          \
   )

#endif


#define container_of(ptr, type, member) \
   __requal_expr(ptr, type, __container_of_unqual(ptr, type, member))

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
   typeof(a) __huzuq(__tmp) = a;    \
   a = b;                           \
   b = __huzuq(__tmp);              \
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
   typeof(a) *__huzuq(__a) = &(a);     \
   typeof(b) *__huzuq(__b) = &(b);     \
   SWAP(*__huzuq(__a), *__huzuq(__b)); \
} while (0)
#endif



/*
 * tmpvalptr(value)
 * ----------------
 * Converts an rvalue (literal/expression) or lvalue into a temporary pointer 
 * by creating an anonymous, inline compound literal array on the stack.
 *
 * INTENT:
 * Standard C does not allow you to take the address of a literal or temporary 
 * expression (e.g., `&42` or `&(x + 1)` is a compile error). This macro forces 
 * the compiler to provision a slot on the current stack frame to store the 
 * value, returning a valid pointer (`typeof(value) *`) to that memory.
 *
 * This allows passing values directly to functions that expect pointers 
 * (like byte-copying serialization or data structure pushes) while maintaining 
 * strict expression compliance (avoids `do { } while(0)` statement blocks).
 *
 *
 * LIFETIME & SAFETY:
 *
 *    The anonymous array has AUTOMATIC storage duration bound strictly 
 *    to the ENCLOSING BLOCK SCOPE (the nearest wrapping `{ }`).
 *
 *    1. SAFE USE: Passing to a function that reads/copies the data immediately 
 *       before returning (e.g., `memcpy`, `static_stack_push`).
 *
 *    2. UNSAFE USE: Storing the pointer or returning it from a function. 
 *       The memory will become a dangling pointer the moment execution exits the block.
 *
 *    3. SIDE EFFECTS: The argument is evaluated exactly ONCE per macro expansion 
 *       within the compound literal array declaration.
 *
 *
 * EXAMPLES:
 *
 *    // 1. Passing a literal to a function expecting a const pointer:
 *    void log_integer(const int *p);
 *    log_integer(tmpvalptr(42)); // Perfectly safe
 *
 *    // 2. Safe immediately-consumed use case:
 *    static_stack_push(&stack, tmpvalptr(x + 5));
 *
 *    // 3. DEADLY UNDEFINED BEHAVIOR (Dangling Pointer):
 *    int *get_forty_two(void) {
 *       return tmpvalptr(42); // WRONG: Memory dies at function exit!
 *    }
 */
#ifndef tmpvalptr
#define tmpvalptr(value) (&((typeof(value)[]) { (value) })[0])
#endif /* tmpvalptr */



/*
 * tmparrptr(TYPE, ...)
 * -------------------
 * Converts a comma-separated list of variadic initializers or values into a 
 * temporary pointer by constructing an anonymous, inline compound literal 
 * array of type @TYPE on the stack.
 *
 * INTENT:
 * Standard C does not allow you to easily initialize and take the address of 
 * an inline sequence of elements on the fly without declaring a named local 
 * array variable first. 
 *
 * This macro bypasses that restriction by wrapping the variadic arguments 
 * (`__VA_ARGS__`) into a C99 compound literal block, returning an explicit 
 * pointer (`TYPE *`) pointing straight to the first element (index 0) of that 
 * newly provisioned sequence.
 *
 * This is engineered primarily to facilitate multi-element data structure 
 * pushes (like pushing a temporary slice or multi-value literal chunk) directly 
 * inside functional expression boundaries, completely avoiding the need for 
 * `do { } while(0)` statement wrappers.
 *
 * LIFETIME & SAFETY:
 *
 *    The anonymous array allocation has AUTOMATIC storage duration bound strictly 
 *    to the nearest ENCLOSING BLOCK SCOPE (the nearest wrapping `{ }`).
 * 
 *    1. SAFE USE: Passing the pointer directly to an opaque or internal function 
 *       that reads/copies the multi-element block immediately before returning 
 *       (e.g., your internal `__static_stack_push` or `memcpy`).
 *
 *    2. UNSAFE USE: Storing the resulting pointer, assigning it to an external 
 *       struct field, or returning it from a function. The memory holding the array 
 *       elements will collapse into a dangling pointer the instant execution 
 *       leaves the curly-brace block it was initialized in.
 *
 *    3. SIDE EFFECTS: Each distinct parameter inside the argument list is evaluated 
 *       exactly ONCE per macro expansion inside the compound literal layout initializer.
 *
 * EXAMPLES:
 *
 *    // 1. Safely pushing multiple literals into a byte-copying container:
 *    static_stack_push(&stack, 10, 20, 30, 40); 
 *    // (Under the hood, this converts the variadic arguments into a temporary 
 *    // continuous chunk of 4 integers on the stack frame via tmparrptr).
 *
 *    // 2. DEADLY UNDEFINED BEHAVIOR (Dangling Slice Reference):
 *    const int *get_coordinate_chunk(void) {
 *       return tmparrptr(int, 100, 200, 300); // WRONG: Memory dies at function return!
 *    }
 */
#ifndef tmparrptr
#define tmparrptr(TYPE, ...) (&((TYPE[]) { __VA_ARGS__ })[0])
#endif /* tmparrptr */
