#ifndef HUZLIB_STATIC_STACK_H
#define HUZLIB_STATIC_STACK_H


#ifndef HUZLIB_STATIC_STACK_INCLUDES
#define HUZLIB_STATIC_STACK_INCLUDES

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
 *
 * WARNING:
 * This macro is the internal implementation and should not be used directly.
 */
#define __requal_expr(ptr, type, expr) _Generic((ptr),               \
   volatile typeof(*(ptr)) *: ((volatile type *)(expr)),             \
   const typeof(*(ptr)) *:    ((const type *)(expr)),                \
   default:                   ((type *)(expr))                       \
)

#define container_of(ptr, type, member) \
   __requal_expr(ptr, type, __container_of_unqual(ptr, type, member))

#endif /* container_of */



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



/*
 * HUZLIB_INLINE_HINTS
 * --------------------------------------
 * Compiler hints for function inlining control.
 * 
 * __huzlib_inline__   - force function to be inlined (small, hot functions)
 * __huzlib_noinline__ - prevent inlining (large functions, error paths)
 */
#ifndef HUZLIB_INLINE_HINTS
#define HUZLIB_INLINE_HINTS

#if defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER) || defined(__POCC__) || defined(_MSC_VER)

   #define __huzlib_inline__     __forceinline
   #define __huzlib_noinline__   __declspec(noinline)

#elif defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__) || defined(__xlC__) || defined(__zig__) || defined(__clang__) || defined(__GNUC__)

   #define __huzlib_inline__     inline __attribute__((always_inline))
   #define __huzlib_noinline__   __attribute__((noinline))

#else

   #define __huzlib_inline__     inline
   #define __huzlib_noinline__

#endif

#endif /* HUZLIB_INLINE_HINTS */



/*
 * HUZLIB_PURE_HINTS
 * --------------------------------------
 * Function attribute hints for optimization based on side-effect analysis.
 *
 * Append before return type:
 *    __huzlib_const__ - output depends ONLY on input (e.g., math functions)
 *    __huzlib_pure__  - no side effects, may read global memory (e.g., strlen)
 *
 * Append after parameter list:
 *    __huzlib_unsequenced__  - output depends ONLY on input (e.g., math functions)
 *    __huzlib_reproducible__ - no side effects, may read global memory (e.g., strlen)
 *
 * Falls back to empty for unsupported compilers.
 */
#ifndef HUZLIB_PURE_HINTS
#define HUZLIB_PURE_HINTS

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)

   #define __huzlib_unsequenced__  [[unsequenced]]
   #define __huzlib_reproducible__ [[reproducible]]

#else

   #define __huzlib_unsequenced__
   #define __huzlib_reproducible__

#endif

#if defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__) || defined(__xlC__) || defined(__zig__) || defined(__clang__) || defined(__GNUC__)

   #define __huzlib_const__   __attribute__((const))
   #define __huzlib_pure__    __attribute__((pure))

#elif defined(__INTEL_COMPILER) || defined(__POCC__) || defined(_MSC_VER)

   #define __huzlib_const__   __declspec(noalias)
   #define __huzlib_pure__    __declspec(noalias)

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)

   #define __huzlib_const__   _Pragma("no_side_effect")
   #define __huzlib_pure__    _Pragma("no_side_effect")

#else

   #define __huzlib_const__
   #define __huzlib_pure__

#endif

#endif /* HUZLIB_PURE_HINTS */



#ifndef __huzlib_assert
/*
 * first, we check for NDEBUG, 
 * which means we are compiling under Optimized mode
 */
#ifdef NDEBUG

   #define __huzlib_assert(cond) ((void)0)

#else
   /*
    * now we check for -freestanding, 
    * which means <assert.h> is not available
    */
   #if defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0)

      #if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ZIG__) || defined(__xlC__) || defined(__ibmxl__)

         #define __huzlib_assert(cond) do {  \
            if (!(cond))                     \
               __builtin_trap();             \
         } while(0)

      #elif defined(_MSC_VER) || defined(__POCC__)

         #define __huzlib_assert(cond) do {  \
            if (!(cond))                     \
               __debugbreak();               \
         } while(0)

      #else

         #define __huzlib_assert(cond) do {  \
            if (!(cond)) {                   \
               volatile int *__huz_fail = 0; \
               (void)*__huz_fail;            \
            }                                \
         } while(0)

      #endif

   #else

      #include <assert.h>
      #define __huzlib_assert(cond) assert(cond)

   #endif /* __STDC_HOSTED__ */

#endif /* NDEBUG */
#endif /* __huzlib_assert */



/*
 * __huzlib_memcpy(dest, src, n)
 * -----------------------------
 * Copy n bytes from src to dest with no undefined behavior.
 *
 * Why not just use memcpy() from <string.h>?
 *    This header aims to be standalone. No libc dependencies.
 *    Some compilers provide __builtin_memcpy. We use that.
 *    MSVC provides #pragma intrinsic(memcpy). We use that.
 *    For the rest, we fall back to a portable loop.
 *
 * Compiler support:
 *    GCC/Clang/Intel/ARM/ZIG → __builtin_memcpy (intrinsic, fast)
 *    MSVC                    → #pragma intrinsic(memcpy) (intrinsic, fast)
 *    Everything else         → portable byte-by-byte loop (slow but works)
 *
 * The portable loop:
 *    Cast void* to unsigned char* for byte-by-byte copy.
 *    C standard forbids arithmetic on void*, so we need the casts.
 *    Compilers optimize this loop to memcpy() or SIMD at -O2 anyway.
 *
 * WARNING:
 *    dest and src must not overlap. This implementation does not handle
 *    overlapping regions (just like standard memcpy).
 */
#ifndef __huzlib_memcpy
#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ZIG__)

   #define __huzlib_memcpy __builtin_memcpy

#elif defined(_MSC_VER)

   #pragma intrinsic(memcpy)
   #define __huzlib_memcpy memcpy

#else

   #include <stddef.h>

   static inline void *__huzlib_memcpy_fallback(void *restrict dest, const void *restrict src, size_t n)
   {
      unsigned char *d = dest;
      const unsigned char *s = src;

      while (n--)
         *d++ = *s++;

      return dest;
   }

   #define __huzlib_memcpy __huzlib_memcpy_fallback

#endif
#endif /* __huzlib_memcpy */



/*
 * __huzlib_memset(dest, ch, n)
 * ----------------------------
 * Fills the first n bytes of the memory area pointed to by dest with the 
 * constant byte ch, with no undefined behavior.
 *
 * Why not just use memset() from <string.h>?
 * This header aims to be standalone. No libc dependencies.
 * Some compilers provide __builtin_memset. We use that.
 * MSVC provides #pragma intrinsic(memset). We use that.
 * For the rest, we fall back to a portable loop.
 *
 * Compiler support:
 * GCC/Clang/Intel/ARM/ZIG → __builtin_memset (intrinsic, fast)
 * MSVC                    → #pragma intrinsic(memset) (intrinsic, fast)
 * Everything else         → portable byte-by-byte loop (slow but works)
 *
 * The portable loop:
 * Cast void* to unsigned char* for byte-by-byte filling.
 * C standard forbids arithmetic on void*, so we need the casts.
 * Compilers optimize this loop to memset() or vector initialization 
 * at -O2 anyway.
 */
#ifndef __huzlib_memset
#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ZIG__)

   #define __huzlib_memset __builtin_memset

#elif defined(_MSC_VER)

   #pragma intrinsic(memset)
   #define __huzlib_memset memset

#else

   #include <stddef.h>

   static inline void *__huzlib_memset_fallback(void *dest, int ch, size_t n)
   {
      unsigned char *d = dest;
      unsigned char c = (unsigned char)ch;

      while (n--)
         *d++ = c;

      return dest;
   }

   #define __huzlib_memset __huzlib_memset_fallback

#endif
#endif /* __huzlib_memset */



/*
 * __huzlib_memalign(ptr, align)
 * -----------------------------
 * moves 'ptr' up to the next multiple of 'align'
 *
 * @ptr:   memory address to align
 * @align: alignment required
 *
 * Return: aligned address
 *
 * NOTE: 'align' must be a power of 2
 */
#ifndef __huzlib_memalign

#if !(defined(__requal_expr) && defined(__huzlib_assert))
   #error "Need __requal_expr && __huzlib_assert"
#endif

#include <stdint.h>

static inline uintptr_t __huzlib_memalign_impl(const uintptr_t addr, const size_t align)
{
   __huzlib_assert((align > 0) && ((align & (align - 1)) == 0));
   uintptr_t offset = (align - (addr & (align - 1))) & (align - 1);
   return addr + offset;
}

#define __huzlib_memalign(ptr, align) \
   __requal_expr(ptr, typeof(*(ptr)), __huzlib_memalign_impl((uintptr_t)(ptr), align))

#endif /* __huzlib_memalign */


#endif /* HUZLIB_STATIC_STACK_INCLUDES */



#include <stddef.h>
#include <stdbool.h>
#include <stdalign.h>

/*
 * STATIC_STACK(type, capacity, [name])
 * ------------------------------------
 * Create a fixed-size stack type
 *
 * You can call this macro in two ways:
 *   STATIC_STACK(int, 4)           -> anonymous struct, no name needed
 *   STATIC_STACK(int, 4, mystack)  -> named struct "mystack"
 *
 * The problem: C macros cannot count arguments. They cannot tell if you
 * passed 2 args or 3 args. ISO C also complains if you leave __VA_ARGS__ empty.
 *
 * The trick: always pass 4 arguments down to the inner macro. The 4th
 * argument does nothing. It just sits there and eats a dummy value.
 *
 * What happens behind the scenes:
 *
 *   You write:  STATIC_STACK(int, 4)
 *   Macro adds: __STATIC_STACK(int,   4,        ,    _1)
 *                             ^^     ^^^    ^^^^    ^^
 *                             type capacity empty  dummy
 *                                           name
 *
 *   You write:  STATIC_STACK(int, 4, mystack)
 *   Macro adds: __STATIC_STACK(int, 4,   mystack,       , _1)
 *                             ^^    ^^     ^^^^      ^^   ^^
 *                             type capacity name    2 dummies
 *                                                  (empty arg also becomes dummy)
 *
 * Why the empty argument? When you give no name, you get an empty spot.
 * The inner macro sees "struct { ... }" which is a valid anonymous struct.
 *
 * Why the dummy _1? ISO C bans a completely empty __VA_ARGS__ parameter.
 * The _1 fills that slot. The inner macro ignores it completely.
 */
#define __STATIC_STACK(TYPE, CAPACITY, NAME, ...)  \
struct NAME {                                      \
   size_t lenb;                                    \
   TYPE buf[CAPACITY];                             \
}

#define STATIC_STACK(...) __STATIC_STACK(__VA_ARGS__, /* empty */, _1)

#define STATIC_STACK_INIT(name)           ((typeof(name)) { .lenb = 0 })

#define __static_stack_type(stack)        typeof((stack)->buf[0])
#define __static_stack_size(stack)        sizeof((stack)->buf)
#define __static_stack_unit_size(stack)   sizeof(__static_stack_type(stack))
#define __static_stack_buf_align(stack)   alignof(__static_stack_type(stack))
#define __static_stack_cap(stack)         (__static_stack_size(stack) / __static_stack_unit_size(stack))
#define __static_stack_enclose(TYPE, new) &(TYPE[]){ (new) }[0]


/*
 * struct __huzlib_uchar_static_stack { }
 * --------------------------------------
 * The hidden backbone for static stack in this library.
 *
 * Problem: one stack implementation needs to work with all types.
 * C++ fixes this with templates. Each type gets its own copy of the code.
 * The binary gets bigger. We do not want that.
 *
 * So to solve this, we use the intuition that a stack is just bytes.
 * The stack push bytes in, and pop bytes oout.
 * The stack does not need to know what those bytes mean.
 * Only the caller needs to know.
 *
 * So we make one fake struct that holds bytes:
 *
 *   struct __huzlib_uchar_static_stack {
 *        size_t lenb;         // length in bytes
 *        unsigned char buf[]; // the bytes themselves
 *   };
 *
 * Why the FAM? Two reasons.
 *
 * First, the FAM has no size. It does not say "buf[1024]" or "buf[64]".
 * It says "buf[]". The real size comes from the struct that wraps it.
 *
 * When you write STATIC_STACK(int, 10, mystack), the macro makes:
 *
 *   struct mystack {
 *        size_t lenb;
 *        int buf[10];
 *   };
 *
 * Two different structs. Same shape. A length. Then a buffer.
 * The memory looks the same. The compiler sees the same layout.
 *
 * Second, the FAM sits at the end of the struct. Right after lenb.
 * No padding in between. The real struct has the same arrangement.
 * This makes the cast between them safe. One struct can pretend
 * to be the other. The bytes line up.
 *
 *
 * The macros use this trick. Every stack operation secretly turns
 * your struct into the fake one:
 *
 *   #define __static_stack_cast(stack) \
 *        container_of(&(stack)->lenb, struct __huzlib_uchar_static_stack, lenb)
 *
 * Why is this safe? The lenb member sits at the very front of both structs.
 * Offset zero. No padding before it. The buffer starts right after lenb.
 * One struct has ints there. The other has unsigned chars.
 * Same address. Different eyes looking at it.
 *
 * The real functions only know the fake struct. They push bytes.
 * They pop bytes. They count bytes. They never see your ints or floats.
 *
 *   void __static_stack_push(struct __huzlib_uchar_static_stack *stack,
 *                            size_t cap_bytes,
 *                            size_t unit_bytes,
 *                            const void *new);
 *
 * The macros wrap these functions. They give the real functions
 * the byte size of your buffer. The byte size of one element.
 * And a pointer to your data.
 *
 * When you peek the stack, the macro casts the byte pointer back
 * to whatever type your stack holds:
 *
 *   #define static_stack_peek(stack) (                        \
 *        *(__static_stack_type(stack) *) __static_stack_peek( \
 *            __static_stack_cast(stack),                      \
 *            sizeof((stack)->buf[0])                          \
 *   )
 *
 * The caller sees an int*. Or a float*. Or a struct sockaddr*.
 * The stack never cared. It just moved bytes around.
 *
 * One set of functions. Works for every type you throw at it.
 * No code bloat. No template explosion. Just bytes and a little casting.
 *
 * This fake struct is the foundation. Everything else is a thin layer
 * of macros on top. The real work happens here, in the byte world,
 * far away from your types and your structs and your fancy abstractions.
 */
STATIC_STACK(unsigned char, /* empty */, __huzlib_uchar_static_stack);
#define __static_stack_cast(stack) container_of(&(stack)->lenb, struct __huzlib_uchar_static_stack, lenb)



#if defined(HUZLIB_STATIC_STACK_SHARED)

   #define HUZLIB_STATIC_STACK_API        __huzlib_export__ __huzlib_noinline__
   #define HUZLIB_STATIC_STACK_INTERNAL   static __huzlib_inline__

#elif defined(NDEBUG)

   #define HUZLIB_STATIC_STACK_API        __huzlib_inline__
   #define HUZLIB_STATIC_STACK_INTERNAL   static __huzlib_inline__

#else

   #define HUZLIB_STATIC_STACK_API        __huzlib_noinline__
   #define HUZLIB_STATIC_STACK_INTERNAL   static __huzlib_noinline__

#endif /* NDEBUG */


extern HUZLIB_STATIC_STACK_API __huzlib_pure__ bool __static_stack_is_empty(const struct __huzlib_uchar_static_stack *stack) __huzlib_reproducible__;
extern HUZLIB_STATIC_STACK_API __huzlib_pure__ bool __static_stack_is_full(const struct __huzlib_uchar_static_stack *stack, size_t size) __huzlib_reproducible__;
extern HUZLIB_STATIC_STACK_API __huzlib_pure__ size_t __static_stack_len(const struct __huzlib_uchar_static_stack *stack, size_t unit_size) __huzlib_reproducible__;
extern HUZLIB_STATIC_STACK_API __huzlib_pure__ void *__static_stack_peek(const struct __huzlib_uchar_static_stack *stack, size_t unit_size, size_t buf_align) __huzlib_reproducible__;
extern HUZLIB_STATIC_STACK_API void __static_stack_init(struct __huzlib_uchar_static_stack *stack);
extern HUZLIB_STATIC_STACK_API void __static_stack_clear(struct __huzlib_uchar_static_stack *stack);
extern HUZLIB_STATIC_STACK_API void __static_stack_clear_scrub(struct __huzlib_uchar_static_stack *stack, size_t buf_align);
extern HUZLIB_STATIC_STACK_API void __static_stack_push(struct __huzlib_uchar_static_stack *stack, const void *new, size_t push_size, size_t size, size_t buf_align);
extern HUZLIB_STATIC_STACK_API void __static_stack_pop(struct __huzlib_uchar_static_stack *stack, size_t pop_size);
extern HUZLIB_STATIC_STACK_API void __static_stack_pop_scrub(struct __huzlib_uchar_static_stack *stack, size_t pop_size, size_t buf_align);


#define static_stack_is_empty(stack) (          \
   __static_stack_is_empty(                     \
      __static_stack_cast(stack)                \
   )                                            \
)

#define static_stack_is_full(stack) (           \
   __static_stack_is_full(                      \
      __static_stack_cast(stack),               \
      __static_stack_size(stack)                \
   )                                            \
)

#define static_stack_len(stack) (               \
   __static_stack_len(                          \
      __static_stack_cast(stack),               \
      __static_stack_unit_size(stack)           \
   )                                            \
)

#define static_stack_peek(stack) (              \
   *(__static_stack_type(stack) *)              \
   __static_stack_peek(                         \
      __static_stack_cast(stack),               \
      __static_stack_unit_size(stack),          \
      __static_stack_buf_align(stack)           \
   )                                            \
)

#define static_stack_init(stack) (              \
   __static_stack_init(                         \
      __static_stack_cast(stack)                \
   )                                            \
)

#define static_stack_clear(stack) (             \
   __static_stack_clear(                        \
      __static_stack_cast(stack)                \
   )                                            \
)

#define static_stack_clear_scrub(stack) (       \
   __static_stack_clear_scrub(                  \
      __static_stack_cast(stack),               \
      __static_stack_buf_align(stack)           \
   )                                            \
)

#define static_stack_push(stack, ...) (         \
   __static_stack_push(                         \
      __static_stack_cast(stack),               \
      (void *)tmparrptr(                        \
         __static_stack_type(stack),            \
         __VA_ARGS__                            \
      ),                                        \
      sizeof((__static_stack_type(stack)[]){    \
         __VA_ARGS__                            \
      }),                                       \
      __static_stack_size(stack),               \
      __static_stack_buf_align(stack)           \
   )                                            \
)

#define static_stack_pushk(stack, new, count) ( \
   __static_stack_push(                         \
      __static_stack_cast(stack),               \
      typecheck_expr(                           \
         __static_stack_type(stack),            \
         (new)[0],                              \
         (void *)(new)                          \
      ),                                        \
      __static_stack_unit_size(stack) * count,  \
      __static_stack_size(stack),               \
      __static_stack_buf_align(stack)           \
   )                                            \
)

#define static_stack_pop(stack) (               \
   __static_stack_pop(                          \
      __static_stack_cast(stack),               \
      __static_stack_unit_size(stack)           \
   )                                            \
)

#define static_stack_pop_scrub(stack) (         \
   __static_stack_pop_scrub(                    \
      __static_stack_cast(stack),               \
      __static_stack_unit_size(stack),          \
      __static_stack_buf_align(stack)           \
   )                                            \
)

#define static_stack_popk(stack, count) (       \
   __static_stack_pop(                          \
      __static_stack_cast(stack),               \
      __static_stack_unit_size(stack) * count   \
   )                                            \
)

#define static_stack_popk_scrub(stack, count) ( \
   __static_stack_pop_scrub(                    \
      __static_stack_cast(stack),               \
      __static_stack_unit_size(stack) * count,  \
      __static_stack_buf_align(stack)           \
   )                                            \
)



#ifdef HUZLIB_STATIC_STACK_IMPL

#include <stdint.h>


HUZLIB_STATIC_STACK_API __huzlib_pure__ bool __static_stack_is_empty(const struct __huzlib_uchar_static_stack *restrict stack)
{
   __huzlib_assert(stack);
   return stack->lenb == 0;
}

HUZLIB_STATIC_STACK_API __huzlib_pure__ bool __static_stack_is_full(const struct __huzlib_uchar_static_stack *restrict stack, size_t size)
{
   __huzlib_assert(stack && size > 0);
   return stack->lenb >= size;
}

HUZLIB_STATIC_STACK_API __huzlib_pure__ size_t __static_stack_len(const struct __huzlib_uchar_static_stack *restrict stack, size_t unit_size)
{
   __huzlib_assert(stack && unit_size > 0);
   return stack->lenb / unit_size;
}

HUZLIB_STATIC_STACK_API __huzlib_pure__ void *__static_stack_peek(const struct __huzlib_uchar_static_stack *restrict stack, size_t unit_size, size_t buf_align)
{
   __huzlib_assert(stack && !__static_stack_is_empty(stack) && (unit_size > 0) && (buf_align > 0));
   const unsigned char *restrict aligned = __huzlib_memalign(stack->buf, buf_align);
   return (void *)(aligned + stack->lenb - unit_size);
}

HUZLIB_STATIC_STACK_API void __static_stack_init(struct __huzlib_uchar_static_stack *restrict stack)
{
   __static_stack_clear(stack);
}

HUZLIB_STATIC_STACK_API void __static_stack_clear(struct __huzlib_uchar_static_stack *restrict stack)
{
   __huzlib_assert(stack);
   stack->lenb = 0;
}

HUZLIB_STATIC_STACK_API void __static_stack_clear_scrub(struct __huzlib_uchar_static_stack *restrict stack, size_t buf_align)
{
   __huzlib_assert(stack && buf_align > 0);
   unsigned char *restrict aligned = __huzlib_memalign(stack->buf, buf_align);
   __huzlib_memset(aligned, 0, stack->lenb);
   __static_stack_clear(stack);
}

HUZLIB_STATIC_STACK_API void __static_stack_push(struct __huzlib_uchar_static_stack *restrict stack, const void *restrict new, size_t push_size, size_t size, size_t buf_align)
{
   __huzlib_assert(stack && new && (push_size > 0) && (size > 0) && (buf_align > 0) && (stack->lenb + push_size <= size));
   unsigned char *restrict aligned = __huzlib_memalign(stack->buf, buf_align);
   __huzlib_memcpy(aligned + stack->lenb, new, push_size);
   stack->lenb += push_size;
}

HUZLIB_STATIC_STACK_API void __static_stack_pop(struct __huzlib_uchar_static_stack *restrict stack, size_t pop_size)
{
   __huzlib_assert(stack && (stack->lenb >= pop_size));
   stack->lenb -= pop_size;
}

HUZLIB_STATIC_STACK_API void __static_stack_pop_scrub(struct __huzlib_uchar_static_stack *restrict stack, size_t pop_size, size_t buf_align)
{
   __huzlib_assert(buf_align > 0);
   __static_stack_pop(stack, pop_size);
   unsigned char *restrict aligned = __huzlib_memalign(stack->buf, buf_align);
   __huzlib_memset(aligned + stack->lenb, 0, pop_size);
}


#endif /* HUZLIB_STATIC_STACK_IMPL */



#ifdef HUZLIB_STATIC_STACK_TEST

#include <stdio.h>
// #include "pcg_basic.h"
// #include "unity.h"
//
// void setUp(void) {}
// void tearDown(void) {}

int main(void)
{
   STATIC_STACK(int, 4) stack = STATIC_STACK_INIT(stack);

   static_stack_push(&stack, 2, 5, 4, 1);

   while (!static_stack_is_empty(&stack))
   {
      printf("%d\n", static_stack_peek(&stack));
      static_stack_pop_scrub(&stack);
   }

   return 0;
}

#endif /* HUZLIB_STATIC_STACK_TEST */



#endif /* HUZLIB_STATIC_STACK_H */
