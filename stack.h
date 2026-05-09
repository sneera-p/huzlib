#ifndef HUZLIB_STACK_H
#define HUZLIB_STACK_H


#ifndef HUZLIB_STACK_INCLUDES
#define HUZLIB_STACK_INCLUDES

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



/*
 * unreachable()
 * -------------
 * Compiler hint indicating that a code path must never be reached at runtime.
 * 
 * Behavior:
 * - If reached, the behavior is undefined (optimizers may assume it never happens).
 * - Enables aggressive dead code elimination and optimization.
 * - Use after calls to functions that never return (e.g., exit(), abort()),
 *   in default cases of exhaustive switches, or after impossible conditions.
 * 
 * Example:
 *   switch (value) {
 *       case A: return foo();
 *       case B: return bar();
 *   }
 *   unreachable();  // all enum values handled above
 */
#ifndef unreachable
#if (__STDC_VERSION__ >= 202311L)
   #include <stdlib.h>

#elif defined(__GNUC__) || defined(__clang__)
   #define unreachable() __builtin_unreachable()

#elif defined(_MSC_VER)
   #define unreachable() __assume(0)

#else
   #include <assert.h>
   #define unreachable() assert(0)

#endif
#endif /* unreachable */


#endif /* HUZLIB_STACK_INCLUDES */



#include <stddef.h>
#include <stdbool.h>
#include <stdalign.h>


#define __STACK_CHUNK(TYPE, CAPACITY, NAME, ...)   \
struct NAME {                                      \
   void *prev;                                     \
   TYPE buf[CAPACITY];                             \
}

#define STACK_CHUNK(...) __STACK_CHUNK(__VA_ARGS__, /* empty */, _1)
#define stack_chunk_prev(chunk) ((typeof(chunk))((chunk)->prev))



#define __STACK_TAIL(TYPE, CAPACITY, NAME, ...)    \
struct NAME {                                      \
   STACK_CHUNK(TYPE, CAPACITY) *chunk;             \
}

#define STACK_TAIL(...) __STACK_TAIL(__VA_ARGS__, /* empty */, _1)



#define __STACK(TYPE, CHUNK_CAPACITY, NAME, ...)   \
struct NAME {                                      \
   size_t top_lenb;                                \
   void *(*alloc)(size_t);                         \
   void (*free)(void *);                           \
   STACK_TAIL(TYPE, CHUNK_CAPACITY) tail;          \
}

#define STACK(...) __STACK(__VA_ARGS__, /* empty */, _1)

#define STACK_INIT(name, alloc_fn, free_fn) (   \
   (typeof(name)) {                             \
   .top_lenb = 0,                               \
   .alloc = alloc_fn,                           \
   .free = free_fn,                             \
   .tail = {                                    \
      .chunk = NULL                             \
   },                                           \
})


#define __stack_chunk_size(stack)      (sizeof((stack)->tail.chunk->buf))
#define __stack_chunk_unit_size(stack) (sizeof((stack)->tail.chunk->buf[0]))
#define __stack_chunk_align(stack)     (alignof(typeof((stack)->tail.chunk->buf[0])))
#define __stack_chunk_cap(stack)       (__stack_chunk_size(stack) / __stack_chunk_unit_size(stack))


STACK(unsigned char, /* empty */, __huzlib_stack);
#define __stack_cast(stack) container_of(&(stack)->top_lenb, struct __huzlib_stack, top_lenb) 


#ifdef NDEBUG
   #define HUZLIB_STACK_API __huzlib_inline__
#else
   #define HUZLIB_STACK_API __huzlib_noinline__
#endif /* NDEBUG */


extern HUZLIB_STACK_API void __stack_init(struct __huzlib_stack *stack, void *(*alloc)(size_t), void (*dealloc)(void *));
extern HUZLIB_STACK_API __huzlib_pure__ bool __stack_is_empty(const struct __huzlib_stack *stack) __huzlib_reproducible__;
extern HUZLIB_STACK_API __huzlib_pure__ bool __stack_is_full(const struct __huzlib_stack *stack, size_t size) __huzlib_reproducible__;
extern HUZLIB_STACK_API __huzlib_pure__ void *__stack_peek(const struct __huzlib_stack *stack, size_t size, size_t unit_size, size_t align) __huzlib_reproducible__;
extern HUZLIB_STACK_API void __stack_push(struct __huzlib_stack *stack, size_t size, size_t unit_size, size_t align, const void *new);
extern HUZLIB_STACK_API void __stack_pop(struct __huzlib_stack *stack, size_t size, size_t unit_size);
extern HUZLIB_STACK_API void __stack_clear(struct __huzlib_stack *stack);


#define stack_init(stack, alloc, free) (  \
   __stack_init(                          \
      __stack_cast(stack),                \
      alloc,                              \
      free                                \
   )                                      \
)

#define stack_is_empty(stack) (           \
   __stack_is_empty(                      \
      __stack_cast(stack)                 \
   )                                      \
)

#define stack_is_full(stack) (            \
   __stack_is_full(                       \
      __stack_cast(stack),                \
      __stack_chunk_size(stack)           \
   )                                      \
)

#define stack_peek(stack) (               \
   (typeof((stack)->tail.chunk->buf[0]) *)\
   __stack_peek(                          \
      __stack_cast(stack),                \
      __stack_chunk_size(stack),          \
      __stack_chunk_unit_size(stack),     \
      __stack_chunk_align(stack)          \
   )                                      \
)

#define stack_push(stack, new) (          \
   __stack_push(                          \
      __stack_cast(stack),                \
      __stack_chunk_size(stack),          \
      __stack_chunk_unit_size(stack),     \
      __stack_chunk_align(stack),         \
      (void *)(new)                       \
   )                                      \
)

#define stack_pop(stack) (                \
   __stack_pop(                           \
      __stack_cast(stack),                \
      __stack_chunk_size(stack),          \
      __stack_chunk_unit_size(stack)      \
   )                                      \
)

#define stack_clear(stack) (              \
   __stack_clear(                         \
      __stack_cast(stack)                 \
   )                                      \
)




#ifdef HUZLIB_STACK_IMPL

#include <assert.h>
#include <stdint.h>
#include <string.h>


#ifdef NDEBUG
   #define HUZLIB_STACK_INTERNAL static __huzlib_inline__
#else
   #define HUZLIB_STACK_INTERNAL static inline
#endif /* NDEBUG */


STACK_CHUNK(unsigned char, /* empty */, __huzlib_stack_chunk);
STACK_TAIL(unsigned char, /* empty */, __huzlib_stack_tail);


HUZLIB_STACK_INTERNAL __huzlib_const__
unsigned char *__chunk_buf_alignup(uintptr_t addr, size_t align) __huzlib_unsequenced__
{
   assert(align > 0 && (align & (align - 1)) == 0);
   size_t offset = (align - (addr & (align - 1))) & (align - 1);
   return (unsigned char *)(addr + offset);
}



/* --- stack_chunk API --- */

HUZLIB_STACK_INTERNAL __huzlib_pure__
void *__stack_chunk_peek(struct __huzlib_stack_chunk *restrict chunk, size_t unit_size, size_t align, size_t lenb) __huzlib_reproducible__
{
   assert(chunk && (lenb > 0));
   unsigned char *restrict aligned = __chunk_buf_alignup((uintptr_t)&chunk->buf, align);
   return (void *)(aligned + lenb - unit_size);
}

HUZLIB_STACK_INTERNAL 
void __stack_chunk_push(struct __huzlib_stack_chunk *restrict chunk, size_t size, size_t unit_size, size_t align, size_t lenb, const void *restrict new)
{
   assert(chunk && new && (lenb < size));
   unsigned char *restrict aligned = __chunk_buf_alignup((uintptr_t)&chunk->buf, align);
   memcpy(aligned + lenb, new, unit_size);
}


/* --- stack tail API --- */

HUZLIB_STACK_INTERNAL
void __stack_tail_add(struct __huzlib_stack_tail *restrict tail, struct __huzlib_stack_chunk *restrict new)
{
   assert(tail && new);
   new->prev = (void *)tail->chunk;
   tail->chunk = (typeof(tail->chunk))new;
}

HUZLIB_STACK_INTERNAL
void __stack_tail_rm(struct __huzlib_stack_tail *restrict tail)
{
   assert(tail && tail->chunk);
   tail->chunk = (typeof(tail->chunk))tail->chunk->prev;
}


/* --- stack helpers --- */

HUZLIB_STACK_INTERNAL __huzlib_pure__
struct __huzlib_stack_chunk *__stack_tail_chunk(const struct __huzlib_stack *restrict stack) __huzlib_reproducible__
{
   assert(stack);
   return (struct __huzlib_stack_chunk *)stack->tail.chunk;
}



HUZLIB_STACK_API 
void __stack_init(struct __huzlib_stack *restrict stack, void *(*alloc)(size_t), void (*dealloc)(void *))
{
   assert(stack && alloc && dealloc);
   *stack = STACK_INIT(*stack, alloc, dealloc);
}

HUZLIB_STACK_API __huzlib_pure__ 
bool __stack_is_empty(const struct __huzlib_stack *restrict stack) __huzlib_reproducible__
{
   assert(stack);
   /*
    * The stack exists as chunks. So if the stack is truly empty,
    * there can be only one chunk with no elements (lenb == 0)
    * OR there are no chunks
    */
   struct __huzlib_stack_chunk *restrict chunk = __stack_tail_chunk(stack);
   return (chunk == NULL) || (chunk->prev == NULL && stack->top_lenb == 0);
}

HUZLIB_STACK_API __huzlib_pure__ 
bool __stack_is_full(const struct __huzlib_stack *restrict stack, size_t size) __huzlib_reproducible__
{
   assert(stack);
   /*
    * By definition a new chunk is allocated only when the previous one is full.
    * So we don't need to check if all chunks are full nor do we need to track 
    * the lenght of all the chunks, just the top-chunk's will do.
    *
    * So to check if the stack is full, we only need to check that the topmost 
    * chunks's lenght in bytes (lenb) is equal to it's size (CAPACITY in bytes)
    */
   return (stack->top_lenb == size);
}

HUZLIB_STACK_API __huzlib_pure__ 
void *__stack_peek(const struct __huzlib_stack *restrict stack, size_t size, size_t unit_size, size_t align) __huzlib_reproducible__
{
   assert(!__stack_is_empty(stack));
   struct __huzlib_stack_chunk *restrict chunk = __stack_tail_chunk(stack);

   if (__huzlib_unlikely__(stack->top_lenb == 0))
   {
      /*
       * The top chunk is empty, we must go to the chunk
       * before that to obtain the topmost element in the stack
       */
      chunk = (struct __huzlib_stack_chunk *)chunk->prev;
      return __stack_chunk_peek(chunk, unit_size, align, size);
   }
   else
   {
      return __stack_chunk_peek(chunk, unit_size, align, stack->top_lenb);
   }
}


HUZLIB_STACK_API 
void __stack_push(struct __huzlib_stack *restrict stack, size_t size, size_t unit_size, size_t align, const void *restrict new)
{
   assert(stack && new);
   struct __huzlib_stack_chunk *restrict chunk;

   if (__huzlib_unlikely__(__stack_is_full(stack, size) || __stack_tail_chunk(stack) == NULL))
   {
      /*
       * We have no space... we need to create a new chunk
       * to store the 'new' element pushed into the stack
       */
      chunk = stack->alloc(size + (size_t)__chunk_buf_alignup(offsetof(struct __huzlib_stack_chunk, buf[0]), align));
      __stack_tail_add((struct __huzlib_stack_tail *)&stack->tail, chunk);

      __stack_chunk_push(chunk, size, unit_size, align, 0, new);
      stack->top_lenb = unit_size;
   }
   else
   {
      chunk = __stack_tail_chunk(stack);
      __stack_chunk_push(chunk, size, unit_size, align, stack->top_lenb, new);
      stack->top_lenb += unit_size;
   }
}

HUZLIB_STACK_API 
void __stack_pop(struct __huzlib_stack *restrict stack, size_t size, size_t unit_size)
{
   assert(!__stack_is_empty(stack));
   struct __huzlib_stack_chunk *restrict chunk = __stack_tail_chunk(stack);

   if (__huzlib_unlikely__(stack->top_lenb == 0))
   {
      /*
       * The top chunk is empty, we must go to the chunk
       * before that to remove the topmost element in the stack
       *
       * While doing that, we are also going to remove the empty chunk
       */

      struct __huzlib_stack_chunk *del = chunk;
      chunk = (struct __huzlib_stack_chunk *)chunk->prev;

      __stack_tail_rm((struct __huzlib_stack_tail *)&stack->tail);
      stack->free(del);

      stack->top_lenb = size - unit_size;
   }
   else
   {
      stack->top_lenb -= unit_size;
   }
}

HUZLIB_STACK_API 
void __stack_clear(struct __huzlib_stack *restrict stack)
{
   struct __huzlib_stack_chunk *restrict chunk = __stack_tail_chunk(stack);
   if (chunk)
   {
      do {
         struct __huzlib_stack_chunk *del = chunk;
         chunk = (struct __huzlib_stack_chunk *)chunk->prev;
         stack->free(del);
      }
      while (chunk);

      stack->top_lenb = 0;
      stack->tail.chunk = NULL;
   }
}


#endif /* HUZLIB_STACK_IMPL */



#ifdef HUZLIB_STACK_TEST

#include <stdlib.h>
#include <stdio.h>
#include "pcg_basic.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
   STACK(int, 4) stack = STACK_INIT(stack, malloc, free);
   int vals[7] = { 0, 1, 2, 3, 4, 5, 6 };

   printf("stack size: [%zu]\n", sizeof(stack));
   printf("chunk size: [%zu]\n", sizeof(*stack.tail.chunk));

   stack_push(&stack, vals + 0);
   stack_push(&stack, vals + 1);
   stack_push(&stack, vals + 2);
   stack_push(&stack, vals + 3);
   stack_push(&stack, vals + 4);
   stack_push(&stack, vals + 5);
   stack_push(&stack, vals + 6);

   printf("[%d]\n", *stack_peek(&stack));
	return 0;
}

#endif /* HUZLIB_STACK_TEST */



#endif /* HUZLIB_STACK_H */
