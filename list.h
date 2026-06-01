#ifndef HUZLIB_LIST_H
#define HUZLIB_LIST_H



#ifndef HUZLIB_LIST_INCLUDES
#define HUZLIB_LIST_INCLUDES


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



/*
 * HUZLIB_LIKELY_HINTS
 * --------------------------------------
 * Branch prediction hints to guide compiler optimization.
 * 
 * __huzlib_likely__(x)   - "x" is usually true  (common case)
 * __huzlib_unlikely__(x) - "x" is usually false (error handling)
 * 
 * Example:
 *   if (__huzlib_unlikely__(error)) {
 *       handle_error();  // moved to cold section
 *   }
 * 
 * Falls back to plain expression evaluation for other compilers.
 */
#ifndef HUZLIB_LIKELY_HINTS
#define HUZLIB_LIKELY_HINTS

#if defined(__clang__) || defined(__GNUC__) || defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__) || defined(__xlC__)

   #define __huzlib_likely__(x)     __builtin_expect(!!(x), 1)
   #define __huzlib_unlikely__(x)   __builtin_expect(!!(x), 0)

#else

   #define __huzlib_likely__(x)     (x)
   #define __huzlib_unlikely__(x)   (x)

#endif

#endif /* HUZLIB_LIKELY_HINTS */



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
 * prefetch_read(addr)
 * -------------------
 * Portable prefetch — compiler detection chain.
 * Always: read prefetch, non-temporal (locality 0).
 * Falls back to a no-op (void cast) on unknown compilers —
 * a no-op is always correct, just not optimal.
 */
#ifndef prefetch_read

#define _PREFETCH_X86 (defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64))
#define _PREFETCH_ARM (defined(__arm__) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64))


#if (defined(__xlC__) || defined(__ibmxl__)) && (defined(__powerpc__) || defined(__ppc__) || defined(__POWERPC__))
   #define prefetch_read(addr)      __dcbt(addr)

#elif defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER) || defined(__POCC__) || defined(__SUNPRO_C) || defined(__SUNPRO_CC)
   #if defined(__sparc) || defined(__sparc__)
      #define prefetch_read(addr)   __asm__ volatile ("prefetch [%0], 1" : : "r" (addr))
   #elif _PREFETCH_X86
      #include <immintrin.h>
      #define prefetch_read(addr)   _mm_prefetch((const char *)(addr), _MM_HINT_NTA)
   #else
      #define prefetch_read(addr)   ((void)(addr))
   #endif

#elif defined(_MSC_VER)
   #include <intrin.h>
   #if _PREFETCH_ARM
      #define prefetch_read(addr)   __prefetch(addr)
   #elif _PREFETCH_X86
      #define prefetch_read(addr)   _mm_prefetch((const char *)(addr), _MM_HINT_NTA)
   #else
      #define prefetch_read(addr)   ((void)(addr))
   #endif

#elif defined(__ZIG__) || defined(__zig__) || defined(__TINYC__) || defined(__clang__) || defined(__GNUC__)
   #define prefetch_read(addr)      __builtin_prefetch((addr), 0, 0)

#else
   #define prefetch_read(addr)      ((void)(addr))

#endif

#endif /* prefetch_read */


#endif /* HUZLIB_LIST_INCLUDES */



#include <stddef.h>
#include <stdbool.h>


struct list_node
{
   struct list_node *next, *prev;
};

#define LIST_HEAD_INIT(name) {   \
   .next = &(name),              \
   .prev = &(name),              \
}

#define list_init(node) typecheck_expr(   \
   struct list_node *, node,              \
   (node)->next = (node)->prev = (node)   \
)


#ifdef NDEBUG
   #define HUZLIB_LIST_API __huzlib_inline__ __huzlib_pure__
#else
   #define HUZLIB_LIST_API __huzlib_inline__
#endif


#ifndef NDEBUG
extern size_t list_len(const struct list_node *head);                                   // WARN: O(n) complexity, do not use in production
extern void list_dump(const struct list_node *head, void (*dump)(struct list_node *));  // WARN: O(n) complexity, do not use in production
extern bool list_contains(const struct list_node *head, const struct list_node *entry); // WARN: O(n) complexity, do not use in production
#endif /* NDEBUG */


/* --- query operations --- */
extern HUZLIB_LIST_API bool list_is_empty(const struct list_node *head);
extern HUZLIB_LIST_API bool list_is_singular(const struct list_node *head);
extern HUZLIB_LIST_API bool list_is_first(const struct list_node *head, const struct list_node *entry);
extern HUZLIB_LIST_API bool list_is_last(const struct list_node *head, const struct list_node *entry);


/* --- mutate operations --- */
extern HUZLIB_LIST_API void list_add_after(struct list_node *node, struct list_node *new);
extern HUZLIB_LIST_API void list_add_before(struct list_node *node, struct list_node *new);
extern HUZLIB_LIST_API void list_del(struct list_node *entry);
extern HUZLIB_LIST_API void list_del_init(struct list_node *entry);
extern HUZLIB_LIST_API void list_replace(struct list_node *entry, struct list_node *new);
extern HUZLIB_LIST_API void list_replace_init(struct list_node *entry, struct list_node *new);


/* --- inplace rearrange operations --- */
extern HUZLIB_LIST_API void list_swap(struct list_node *a, struct list_node *b);
extern HUZLIB_LIST_API void list_mov_after(struct list_node *node, struct list_node *dest);
extern HUZLIB_LIST_API void list_mov_before(struct list_node *node, struct list_node *dest);
extern HUZLIB_LIST_API void list_rotate_after(struct list_node *head);
extern HUZLIB_LIST_API void list_rotate_before(struct list_node *head);
extern HUZLIB_LIST_API void list_reverse(struct list_node *head);
extern HUZLIB_LIST_API void list_sort(struct list_node *head, int (*cmp)(struct list_node *, struct list_node *));


/* --- chain operations --- */
extern HUZLIB_LIST_API void list_splice_after(struct list_node *node, struct list_node *src);
extern HUZLIB_LIST_API void list_splice_after_init(struct list_node *node, struct list_node *src);
extern HUZLIB_LIST_API void list_splice_before(struct list_node *node, struct list_node *src);
extern HUZLIB_LIST_API void list_splice_before_init(struct list_node *node, struct list_node *src);
extern HUZLIB_LIST_API void list_cut_after(struct list_node *node, struct list_node *entry, struct list_node *dest);
extern HUZLIB_LIST_API void list_cut_before(struct list_node *node, struct list_node *entry, struct list_node *dest);


/* --- list traversal --- */
#define list_foreach(pos, head) for (                 \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->next;                              \
   (pos) != (head);                                   \
   (pos) = (pos)->next                                \
)

#define list_foreach_rev(pos, head) for (             \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->prev;                              \
   (pos) != (head);                                   \
   (pos) = (pos)->prev                                \
)

#define list_foreach_prefetch(pos, head) for (        \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->next;                              \
   (pos) != (head);                                   \
   (pos) = (pos)->next,                               \
   prefetch_read((pos)->next)                         \
)

#define list_foreach_prefetch2(pos, head) for (       \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->next;                              \
   (pos) != (head);                                   \
   (pos) = (pos)->next,                               \
   ((pos)->next != (head)                             \
      ? prefetch_read((pos)->next->next)              \
      : (void)0                                       \
   )                                                  \
)

#define list_foreach_safe(pos, tmp, head) for (       \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(tmp)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->next,                              \
   (tmp) = (pos)->next;                               \
   (pos) != (head);                                   \
   (pos) = (tmp),                                     \
   (tmp) = (tmp)->next                                \
)

#define list_foreach_safe_rev(pos, tmp, head) for (   \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(tmp)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->prev,                              \
   (tmp) = (pos)->prev;                               \
   (pos) != (head);                                   \
   (pos) = (tmp),                                     \
   (tmp) = (tmp)->prev                                \
)

#define list_foreach_from(pos, head) for (            \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head));              \
   (pos) != (head);                                   \
   (pos) = (pos)->next                                \
)

#define list_foreach_rev_from(pos, head) for (        \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head));              \
   (pos) != (head);                                   \
   (pos) = (pos)->prev                                \
)


/* --- container macros --- */
#define list_entry(ptr, type, member)           container_of(ptr, type, member)
#define list_first_entry(head, type, member)    container_of((head)->next, type, member)
#define list_last_entry(head, type, member)     container_of((head)->prev, type, member)
#define list_next_entry(entr, type, member)     container_of((entr)->member.next, type, member)
#define list_prev_entry(entr, type, member)     container_of((entr)->member.prev, type, member)

#define list_foreach_entry(entr, head, type, member) for (                 \
   typecheck(type, *(entr)),                                               \
   typecheck(struct list_node, *(head)),                                   \
   typecheck_member(struct list_node, type, member),                       \
   (entr) = list_first_entry(head, type, member);                          \
   &(entr)->member != (head);                                              \
   (entr) = list_next_entry(entr, type, member)                            \
)

#define list_foreach_entry_rev(entr, head, type, member) for (             \
   typecheck(type, *(entr)),                                               \
   typecheck(struct list_node, *(head)),                                   \
   typecheck_member(struct list_node, type, member),                       \
   (entr) = list_last_entry(head, type, member);                           \
   &(entr)->member != (head);                                              \
   (entr) = list_prev_entry(entr, type, member)                            \
)

#define list_foreach_entry_safe(entr, temp, head, type, member) for (      \
   typecheck(type, *(entr)),                                               \
   typecheck(type, *(temp)),                                               \
   typecheck(struct list_node, *(head)),                                   \
   typecheck_member(struct list_node, type, member),                       \
   (entr) = list_first_entry(head, type, member),                          \
   (temp) = list_next_entry(entr, type, member);                           \
   &(entr)->member != (head);                                              \
   (entr) = (temp),                                                        \
   (temp) = list_next_entry(temp, type, member)                            \
)

#define list_foreach_entry_safe_rev(entr, temp, head, type, member) for (  \
   typecheck(type, *(entr)),                                               \
   typecheck(type, *(temp)),                                               \
   typecheck(struct list_node, *(head)),                                   \
   typecheck_member(struct list_node, type, member),                       \
   (entr) = list_last_entry(head, type, member),                           \
   (temp) = list_prev_entry(entr, type, member);                           \
   &(entr)->member != (head);                                              \
   (entr) = (temp),                                                        \
   (temp) = list_prev_entry(temp, type, member)                            \
)

#define list_foreach_entry_from(entr, head, type, member) for (            \
   typecheck(type, *(entr)),                                               \
   typecheck(struct list_node, *(head));                                   \
   typecheck_member(struct list_node, type, member),                       \
   &(entr)->member != (head);                                              \
   (entr) = list_next_entry(entr, type, member)                            \
)

#define list_foreach_entry_rev_from(entr, head, type, member) for (        \
   typecheck(type, *(entr)),                                               \
   typecheck(struct list_node, *(head));                                   \
   typecheck_member(struct list_node, type, member),                       \
   &(entr)->member != (head);                                              \
   (entr) = list_prev_entry(entr, type, member)                            \
)



#ifdef HUZLIB_LIST_IMPL

#ifndef NDEBUG

size_t list_len(const struct list_node *head)
{
   __huzlib_assert(head);
   struct list_node *restrict cur;
   size_t len = 0;
   list_foreach(cur, head)
      len++;
   return len;
}

void list_dump(const struct list_node *head, void (*dump)(struct list_node *))
{
   __huzlib_assert(head && dump);
   struct list_node *restrict cur;
   list_foreach(cur, head)
      dump(cur);
}

bool list_contains(const struct list_node *head, const struct list_node *entry)
{
   __huzlib_assert(head && entry);
   struct list_node *restrict cur;
   list_foreach(cur, head)
      if (cur == entry)
          return true;
   return false;
}

#endif /* NDEBUG */


/* ------------------------------------------------ */
/* --------------- query operations --------------- */
/* ------------------------------------------------ */

HUZLIB_LIST_API bool list_is_empty(const struct list_node *head)
{
   __huzlib_assert(head);
   return head->next == head;
}

HUZLIB_LIST_API bool list_is_singular(const struct list_node *head)
{
   __huzlib_assert(head);
   return !list_is_empty(head) && head->next->next == head;
}

HUZLIB_LIST_API bool list_is_first(const struct list_node *head, const struct list_node *entry)
{
   __huzlib_assert(head && entry);
   return head->next == entry;
}

HUZLIB_LIST_API bool list_is_last(const struct list_node *head, const struct list_node *entry)
{
   __huzlib_assert(head && entry);
   return head->prev == entry;
}


/* ------------------------------------------------- */
/* --------------- helper functions  --------------- */
/* ------------------------------------------------- */

/*
 * before:  prev <-> next, prev <-> old <-> next
 * after:   prev <-> new <-> next
 */ 
static HUZLIB_LIST_API void __list_add(struct list_node *restrict new, struct list_node *prev, struct list_node *next)
{
   __huzlib_assert(new && prev && next);
   __huzlib_assert((prev->next == next && next->prev == prev) || (prev->next->next == next && next->prev->prev == prev));

   prev->next = new;
   next->prev = new;
   new->prev = prev;
   new->next = next;
}

/*
 * before:  prev <-> next, prev <-> old(...) <-> next
 * after:   prev <-> new_head <-> ... <-> new_tail <-> next
 */ 
static HUZLIB_LIST_API void __list_add_batch(struct list_node *new_head, struct list_node *new_tail, struct list_node *prev, struct list_node *next)
{
   __huzlib_assert(new_head && new_tail && prev && next);

   prev->next = new_head;
   next->prev = new_tail;
   new_head->prev = prev;
   new_tail->next = next;
}

/*
 * before:  prev <-> del <-> next
 * after:   prev <-> next
 */ 
static HUZLIB_LIST_API void __list_rm(struct list_node *prev, struct list_node *next)
{
   __huzlib_assert(prev && next);
   prev->next = next;
   next->prev = prev;
}

/*
 * before:  aprev <-> a <-> anext,  bprev <-> b <-> bnext
 * after:   aprev <-> b <-> anext,  bprev <-> a <-> bnext
 */ 
static HUZLIB_LIST_API void __list_swap(struct list_node *aprev, struct list_node *restrict a, struct list_node *anext, struct list_node *bprev, struct list_node *restrict b, struct list_node *bnext)
{
   __huzlib_assert(aprev && a && anext && bprev && b && bnext);
   __huzlib_assert(aprev == a->prev && aprev->next == a);
   __huzlib_assert(anext == a->next && anext->prev == a);
   __huzlib_assert(bprev == b->prev && bprev->next == b);
   __huzlib_assert(bnext == b->next && bnext->prev == b);

   aprev->next = b;
   anext->prev = b;

   bprev->next = a;
   bnext->prev = a;

   a->prev = bprev;
   a->next = bnext;

   b->prev = aprev;
   b->next = anext;
}

/*
 * before:  pre <-> prev <-> next <-> suc
 * after:   pre <-> next <-> prev <-> suc
 */ 
static HUZLIB_LIST_API void __list_swap_adj(struct list_node *pre, struct list_node *restrict prev, struct list_node *restrict next, struct list_node *suc)
{
   __huzlib_assert(prev && next);
   __huzlib_assert(pre == prev->prev && pre->next == prev);
   __huzlib_assert(suc == next->next && suc->prev == next);

   pre->next = next;
   suc->prev = prev;

   prev->next = suc;
   next->prev = pre;

   prev->prev = next;
   next->next = prev;
}


/* ------------------------------------------------- */
/* --------------- mutate operations --------------- */
/* ------------------------------------------------- */

HUZLIB_LIST_API void list_add_after(struct list_node *restrict node, struct list_node *restrict new)
{
   __huzlib_assert(node && new);
   __list_add(new, node, node->next);
}

HUZLIB_LIST_API void list_add_before(struct list_node *restrict node, struct list_node *restrict new)
{
   __huzlib_assert(node && new);
   __list_add(new, node->prev, node);
}

HUZLIB_LIST_API void list_del(struct list_node *restrict entry)
{
   __huzlib_assert(entry);
   __list_rm(entry->prev, entry->next);
}

HUZLIB_LIST_API void list_del_init(struct list_node *restrict entry)
{
   list_del(entry);
   list_init(entry);
}

HUZLIB_LIST_API void list_replace(struct list_node *restrict entry, struct list_node *restrict new)
{
   __huzlib_assert(entry && new);
   __list_add(new, entry->prev, entry->next);
}

HUZLIB_LIST_API void list_replace_init(struct list_node *restrict entry, struct list_node *restrict new)
{
   list_replace(entry, new);
   list_init(entry);
}


/* ------------------------------------------------ */
/* --------- inplace rearrange operations --------- */
/* ------------------------------------------------ */

HUZLIB_LIST_API void list_swap(struct list_node *restrict a, struct list_node *restrict b)
{
   __huzlib_assert(a && b);
   if (a->next == b)
      __list_swap_adj(a->prev, a, b, b->next);
   else if (a->prev == b)
      __list_swap_adj(b->prev, b, a, a->next);
   else
      __list_swap(a->prev, a, a->next, b->prev, b, b->next);
}

HUZLIB_LIST_API void list_mov_after(struct list_node *restrict node, struct list_node *restrict dest)
{
   __huzlib_assert(node && dest);
   list_del(node);
   list_add_after(dest, node);
}

HUZLIB_LIST_API void list_mov_before(struct list_node *restrict node, struct list_node *restrict dest)
{
   __huzlib_assert(node && dest);
   list_del(node);
   list_add_before(dest, node);
}

HUZLIB_LIST_API void list_rotate_after(struct list_node *restrict head)
{
   __huzlib_assert(head);
   if (!(list_is_empty(head) || list_is_singular(head)))
      __list_swap_adj(head->prev, head, head->next, head->next->next);
}

HUZLIB_LIST_API void list_rotate_before(struct list_node *restrict head)
{
   __huzlib_assert(head);
   if (!(list_is_empty(head) || list_is_singular(head)))
      __list_swap_adj(head->prev->prev, head->prev, head, head->next);
}

HUZLIB_LIST_API void list_reverse(struct list_node *restrict head)
{
   __huzlib_assert(head);
   struct list_node *cur, *tmp;
   list_foreach_safe(cur, tmp, head)
      SWAP(cur->prev, cur->next);
   SWAP(head->prev, head->next);
}

/*
 * list_sort - mergesort implementation
 *
 * Strategy:
 *    Bottom-up iterative merge
 *    merge sub-lists of size 1, then 2, then 4, etc.
 *
 * --- this function was generated by Gemini 3 Fast ---
 */
HUZLIB_LIST_API void list_sort(struct list_node *head, int (*cmp)(struct list_node *, struct list_node *))
{
   __huzlib_assert(head && cmp);

   if (list_is_empty(head) || list_is_singular(head))
      return;

   struct list_node *list = head->next;
   head->prev->next = NULL; // Temporarily turn circular list into null-terminated

   struct list_node *p, *q, *e, *tail;
   size_t insize = 1, nmerges, psize, qsize, i;

   while (1)
   {
      p = list;
      list = NULL;
      tail = NULL;
      nmerges = 0;

      while (p)
      {
         nmerges++;
         q = p;
         psize = 0;
         for (i = 0; i < insize; i++)
         {
            psize++;
            q = q->next;
            if (!q) break;
         }

         qsize = insize;
         while (psize > 0 || (qsize > 0 && q))
         {
            if (psize == 0)
            {
               e = q;
               q = q->next;
               qsize--;
            }
            else if (qsize == 0 || !q)
            {
               e = p;
               p = p->next;
               psize--;
            }
            else if (cmp(p, q) <= 0)
            {
               e = p;
               p = p->next;
               psize--;
            }
            else
            {
               e = q;
               q = q->next;
               qsize--;
            }

            if (tail)
               tail->next = e;
            else
               list = e;
 
            e->prev = tail; // Maintain the prev pointer
            tail = e;
         }
         p = q;
      }

      tail->next = NULL;
      if (nmerges <= 1)
      {
         // Sorting complete, close the circle
         head->next = list;
         list->prev = head;
         tail->next = head;
         head->prev = tail;
         return;
      }
      insize *= 2;
   }
}


/* ------------------------------------------------ */
/* --------------- chain operations --------------- */
/* ------------------------------------------------ */

/*
 * target:  node <-> next_node
 * source:  [src] <-> first <-> ... <-> last <-> [src]
 *
 * after:   node <-> first <-> ... <-> last <-> next_node
 */
HUZLIB_LIST_API void list_splice_after(struct list_node *restrict node, struct list_node *restrict src)
{
   __huzlib_assert(node && src);
   __list_add_batch(src->next, src->prev, node, node->next);
}

HUZLIB_LIST_API void list_splice_after_init(struct list_node *restrict node, struct list_node *restrict src)
{
   __huzlib_assert(node && src);
   list_splice_after(node, src);
   list_init(src);
}

/*
 * target:  prev_node <-> node
 * source:  [src] <-> first <-> ... <-> last <-> [src]
 *
 * after:   prev_node <-> first <-> ... <-> last <-> node
 */
HUZLIB_LIST_API void list_splice_before(struct list_node *restrict node, struct list_node *restrict src)
{
   __huzlib_assert(node && src);
   __list_add_batch(src->next, src->prev, node->prev, node);
}

HUZLIB_LIST_API void list_splice_before_init(struct list_node *restrict node, struct list_node *restrict src)
{
   __huzlib_assert(node && src);
   list_splice_before(node, src);
   list_init(src);
}

/*
 * node:  [node] <-> 1 <-> ... <-> [entry] <-> [split_node] <-> ... <-> [node]
 * dest:  [dest] <-> [dest] (must be empty)
 *
 * step 1: [node] <--------------------------> [split_node] (bypass segment)
 * step 2: [dest] <-> 1 <-> ... <-> [entry] <-> [dest]      (re-anchor segment)
 *
 * after:  node contains [node] <-> [split_node] ...
 * dest contains [dest] <-> 1 ... <-> [entry]
 */
HUZLIB_LIST_API void list_cut_after(struct list_node *restrict node, struct list_node *restrict entry, struct list_node *restrict dest)
{
   __huzlib_assert(node && entry && list_is_empty(dest));
   struct list_node *restrict _tmp = node->next;
   __list_rm(node, entry->next);
   __list_add_batch(_tmp, entry, dest, dest);
}

/*
 * node:  [node] <-> ... <-> [prev_node] <-> [entry] <-> ... <-> [last] <-> [node]
 * dest:  [dest] <-> [dest] (must be empty)
 *
 * step 1: [node] <-> ... <-> [prev_node] <---------------------------> [node]
 * step 2: [dest] <---------> [entry] <-> ... <-> [last] <------------> [dest]
 *
 * after:  node contains [node] <-> ... <-> [prev_node]
 * dest contains [dest] <-> [entry] ... <-> [last]
 */
HUZLIB_LIST_API void list_cut_before(struct list_node *restrict node, struct list_node *restrict entry, struct list_node *restrict dest)
{
   __huzlib_assert(node && entry && list_is_empty(dest));
   struct list_node *restrict _tmp = node->prev;
   __list_rm(entry->prev, node);
   __list_add_batch(entry, _tmp, dest, dest);
}

#endif /* HUZLIB_LIST_IMPL */




#ifdef HUZLIB_LIST_TEST
#endif /* HUZLIB_LIST_TEST */




#endif /* HUZLIB_LIST_H */
