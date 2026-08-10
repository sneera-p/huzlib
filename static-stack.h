#ifndef HUZLIB_STATIC_STACK_H
#define HUZLIB_STATIC_STACK_H


#ifdef HUZLIB_STATIC_STACK_IMPL
   #define HUZLIB_IMPL
#endif

#ifdef HUZLIB_STATIC_STACK_SHARED
   #define HUZLIB_SHARED
#endif


#ifndef HUZLIB_STATIC_STACK_INCLUDES
#define HUZLIB_STATIC_STACK_INCLUDES

#include "utils/types.h"
#include "utils/hints.h"
#include "utils/assert.h"
#include "utils/mem.h"
#include "utils/api.h"

#endif /* HUZLIB_STATIC_STACK_INCLUDES */



#include <stddef.h>
#include <stdbool.h>
#include <stdalign.h>

/*
 * HUZ_STATIC_STACK(type, capacity, [name])
 * ------------------------------------
 * Create a fixed-size stack type
 *
 * You can call this macro in two ways:
 *   HUZ_STATIC_STACK(int, 4)           -> anonymous struct, no name needed
 *   HUZ_STATIC_STACK(int, 4, mystack)  -> named struct "mystack"
 *
 * The problem: C macros cannot count arguments. They cannot tell if you
 * passed 2 args or 3 args. ISO C also complains if you leave __VA_ARGS__ empty.
 *
 * The trick: always pass 4 arguments down to the inner macro. The 4th
 * argument does nothing. It just sits there and eats a dummy value.
 *
 * What happens behind the scenes:
 *
 *   You write:  HUZ_STATIC_STACK(int, 4)
 *   Macro adds: __HUZ_STATIC_STACK(int,   4,        ,    _1)
 *                             ^^     ^^^    ^^^^    ^^
 *                             type capacity empty  dummy
 *                                           name
 *
 *   You write:  HUZ_STATIC_STACK(int, 4, mystack)
 *   Macro adds: __HUZ_STATIC_STACK(int, 4,   mystack,       , _1)
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
#define __HUZ_STATIC_STACK(TYPE, CAPACITY, NAME, ...) \
struct NAME {                                         \
   size_t lenb;                                       \
   TYPE buf[CAPACITY];                                \
}

#define HUZ_STATIC_STACK(...) __HUZ_STATIC_STACK(__VA_ARGS__, /* empty */, _1)

#define HUZ_STATIC_STACK_INIT(name)           ((typeof(name)) { .lenb = 0 })

#define __huz_static_stack_type(stack)        typeof((stack)->buf[0])
#define __huz_static_stack_size(stack)        sizeof((stack)->buf)
#define __huz_static_stack_unit_size(stack)   sizeof(__huz_static_stack_type(stack))
#define __huz_static_stack_buf_align(stack)   alignof(__huz_static_stack_type(stack))
#define __huz_static_stack_cap(stack)         (__huz_static_stack_size(stack) / __huz_static_stack_unit_size(stack))


/*
 * struct __huzlib_uchar_static_stack
 * -------------------------------------------------------------------------
 * The hidden byte backbone for the static stack.
 *
 *
 * THE PROBLEM:
 *
 * We want one stack implementation to work with all types (ints, floats, or 
 * custom structs). C++ fixes this with templates, but templates make a new 
 * copy of the code for every type. The binary gets huge. We don't want that.
 *
 *
 * THE VISION:
 *
 * At the hardware level, a stack is just a chunk of memory. It pushes bytes 
 * in, and pops bytes out. The stack doesn't need to know what those bytes 
 * mean. Only your application cares.
 *
 * So, we make one fake struct that holds raw bytes:
 *
 *    struct __huzlib_uchar_static_stack {
 *       size_t lenb;         // length in bytes
 *       unsigned char buf[]; // the raw bytes (Flexible Array Member)
 *    };
 *
 * WHY THE FAM (Flexible Array Member)?
 *
 *    the FAM has no size. It doesn't say "buf[1024]" or "buf[64]". It 
 *    just says "buf[]". The real size comes from the struct that wraps it.
 *
 *    When you write `HUZ_STATIC_STACK(int, 10, mystack)`, the macro creates:
 *
 *       struct mystack {
 *          size_t lenb;
 *          int buf[10];
 *       };
 *
 *    Two different structs, but they share the exact same foundation: a length 
 *    variable at the front, followed by a buffer at the back.
 *
 *
 * THE ALIGNMENT & PADDING TRAP:
 *
 * But what happens if you create a stack of a type that has huge alignment 
 * requirements? For example, a massive SIMD vector or a strict 64-bit type 
 * on a 32-bit SoC that must sit on a clean memory boundary?
 *
 * If your type requires a larger alignment than `size_t`, the compiler will 
 * secretly inject dead space (padding bytes) between `lenb` and `buf` to 
 * make sure your data lines up perfectly for the CPU.
 *
 * Crucially, our fake backbone struct uses an `unsigned char` buffer. Since 
 * bytes only need 1-byte alignment, the fake struct will NEVER have padding. 
 * Its `buf` always starts immediately after `lenb`. 
 *
 * Because of this hidden padding discrepancy, the user's real buffer and 
 * our fake buffer don't always start at the exact same offset!
 *
 * To make this casting 100% safe on any processor (including ARM or ESP32), 
 * we use two clever tricks:
 *
 * 1. The Offset Zero Invariant:
 *
 *    The `lenb` variable sits at the very front of both structs. Offset zero. 
 *    No padding can ever exist before it. This means converting your struct 
 *    pointer into the fake backbone pointer is always completely safe:
 *
 *    #define __huz_static_stack_cast(stack) \
 *       container_of(&(stack)->lenb, struct __huzlib_uchar_static_stack, lenb)
 *
 * 2. Dynamic Alignment Math:
 *
 *    Our real functions never read `stack->buf` directly. Instead, the macros 
 *    pass the real type's alignment requirement down to the engine. The engine 
 *    runs a super-fast math function:
 *
 *    unsigned char *aligned = __huzlib_memalign(stack->buf, buf_align);
 *
 *    If your type forced the compiler to insert padding, `__huzlib_memalign` 
 *    calculates it instantly and skips right past the dead space to find your 
 *    real data. If there is no padding, the math does nothing and costs zero 
 *    overhead.
 *
 *
 * THE WORKERS UNDER THE HOOD:
 *
 * The real functions only know the fake struct. They push bytes, pop bytes, 
 * and count bytes. They never see your ints or floats:
 * 
 *    void __huz_static_stack_push(struct __huzlib_uchar_static_stack *stack, 
 *                             const void *new, 
 *                             size_t push_size, 
 *                             size_t size, 
 *                             size_t buf_align);
 *
 * A macro translation layer wraps these functions. They give the real functions 
 * the byte sizes of your buffer, the byte size of one element, the alignment of TYPE, 
 * and a pointer to your data.
 *
 *    #define huz_static_stack_push(stack, value) (   \
 *       __huz_static_stack_push(                     \
 *          __huz_static_stack_cast(stack),           \
 *          (void *)tmpptr(                           \
 *             __huz_static_stack_type(stack),        \
 *             (value)                                \
 *          ),                                        \
 *          sizeof(__huz_static_stack_type(stack)),   \
 *          __huz_static_stack_size(stack),           \
 *          __huz_static_stack_buf_align(stack)       \
 *       )                                            \
 *    )
 *
 * The real functions live in the "byte world," moving raw memory around. 
 * When you peek at the stack, the macro layer instantly casts the raw byte 
 * pointer back to your original type and dereferences it:
 *
 *    #define huz_static_stack_peek(stack) (       \
 *       *(__huz_static_stack_type(stack) *)       \
 *       __huz_static_stack_peek(                  \
 *          __huz_static_stack_cast(stack),        \
 *          __huz_static_stack_unit_size(stack),   \
 *          __huz_static_stack_buf_align(stack)    \
 *       )                                         \
 *    )
 *
 * The caller sees an int, a float, or a custom struct. The stack never 
 * cared. It just moved bytes around.
 *
 * One set of functions. Works for every type you throw at it. No code bloat. 
 * No template explosion. Just bytes, a little math, and safe pointer casting.
 *
 * This fake struct is the foundation. Everything else is a thin layer of 
 * macros on top. The real work happens here, in the byte world, far away 
 * from your types and your fancy abstractions.
 */
HUZ_STATIC_STACK(unsigned char, /* empty */, __huzlib_uchar_static_stack);
#define __huz_static_stack_cast(stack) container_of(&(stack)->lenb, struct __huzlib_uchar_static_stack, lenb)



extern HUZLIB_IMPL_API void __huz_static_stack_clear_scrub(struct __huzlib_uchar_static_stack *stack, size_t buf_align);
extern HUZLIB_IMPL_API void __huz_static_stack_pop_scrub(struct __huzlib_uchar_static_stack *stack, size_t pop_size, size_t buf_align);


#ifndef HUZLIB_STATIC_STACK_INLINE_IMPL
#define HUZLIB_STATIC_STACK_INLINE_IMPL


HUZLIB_INLINE_API __huzlib_pure__ bool __huz_static_stack_is_empty(const struct __huzlib_uchar_static_stack *restrict stack)
{
   __huzlib_assert(stack);
   return stack->lenb == 0;
}

HUZLIB_INLINE_API __huzlib_pure__ bool __huz_static_stack_is_full(const struct __huzlib_uchar_static_stack *restrict stack, size_t size)
{
   __huzlib_assert(stack && size > 0);
   return stack->lenb >= size;
}

HUZLIB_INLINE_API __huzlib_pure__ size_t __huz_static_stack_len(const struct __huzlib_uchar_static_stack *restrict stack, size_t unit_size)
{
   __huzlib_assert(stack && unit_size > 0);
   return stack->lenb / unit_size;
}

HUZLIB_INLINE_API __huzlib_pure__ void *__huz_static_stack_peek(const struct __huzlib_uchar_static_stack *restrict stack, size_t unit_size, size_t buf_align)
{
   __huzlib_assert(stack && !__huz_static_stack_is_empty(stack) && (unit_size > 0) && (buf_align > 0));
   const unsigned char *restrict aligned = __huzlib_memalign(stack->buf, buf_align);
   return (void *)(aligned + stack->lenb - unit_size);
}

HUZLIB_INLINE_API void __huz_static_stack_init(struct __huzlib_uchar_static_stack *restrict stack)
{
   __huzlib_assert(stack);
   stack->lenb = 0;
}

HUZLIB_INLINE_API void __huz_static_stack_clear(struct __huzlib_uchar_static_stack *restrict stack)
{
   __huz_static_stack_init(stack);
}

HUZLIB_INLINE_API void __huz_static_stack_push(struct __huzlib_uchar_static_stack *restrict stack, const void *restrict new, size_t push_size, size_t size, size_t buf_align)
{
   __huzlib_assert(stack && new && (push_size > 0) && (size > 0) && (buf_align > 0) && (stack->lenb + push_size <= size));
   unsigned char *restrict aligned = __huzlib_memalign(stack->buf, buf_align);
   __huzlib_memcpy(aligned + stack->lenb, new, push_size);
   stack->lenb += push_size;
}

HUZLIB_INLINE_API void __huz_static_stack_pop(struct __huzlib_uchar_static_stack *restrict stack, size_t pop_size)
{
   __huzlib_assert(stack && (stack->lenb >= pop_size));
   stack->lenb -= pop_size;
}


#endif /* HUZLIB_STATIC_STACK_INLINE_IMPL */


#define huz_static_stack_is_empty(stack) (            \
   __huz_static_stack_is_empty(                       \
      __huz_static_stack_cast(stack)                  \
   )                                                  \
)

#define huz_static_stack_is_full(stack) (             \
   __huz_static_stack_is_full(                        \
      __huz_static_stack_cast(stack),                 \
      __huz_static_stack_size(stack)                  \
   )                                                  \
)

#define huz_static_stack_len(stack) (                 \
   __huz_static_stack_len(                            \
      __huz_static_stack_cast(stack),                 \
      __huz_static_stack_unit_size(stack)             \
   )                                                  \
)

#define huz_static_stack_peek(stack) (                \
   *(__huz_static_stack_type(stack) *)                \
   __huz_static_stack_peek(                           \
      __huz_static_stack_cast(stack),                 \
      __huz_static_stack_unit_size(stack),            \
      __huz_static_stack_buf_align(stack)             \
   )                                                  \
)

#define huz_static_stack_init(stack) (                \
   __huz_static_stack_init(                           \
      __huz_static_stack_cast(stack)                  \
   )                                                  \
)

#define huz_static_stack_clear(stack) (               \
   __huz_static_stack_clear(                          \
      __huz_static_stack_cast(stack)                  \
   )                                                  \
)

#define huz_static_stack_clear_scrub(stack) (         \
   __huz_static_stack_clear_scrub(                    \
      __huz_static_stack_cast(stack),                 \
      __huz_static_stack_buf_align(stack)             \
   )                                                  \
)

#define huz_static_stack_push(stack, ...) (           \
   __huz_static_stack_push(                           \
      __huz_static_stack_cast(stack),                 \
      (void *)tmpptr(                                 \
         __huz_static_stack_type(stack),              \
         __VA_ARGS__                                  \
      ),                                              \
      sizeof(tmpptr(                                  \
         __huz_static_stack_type(stack),              \
         __VA_ARGS__                                  \
      )),                                             \
      __huz_static_stack_size(stack),                 \
      __huz_static_stack_buf_align(stack)             \
   )                                                  \
)

#define huz_static_stack_pushk(stack, new, count) (   \
   __huz_static_stack_push(                           \
      __huz_static_stack_cast(stack),                 \
      typecheck_expr(                                 \
         __huz_static_stack_type(stack),              \
         (new)[0],                                    \
         (void *)(new)                                \
      ),                                              \
      __huz_static_stack_unit_size(stack) * count,    \
      __huz_static_stack_size(stack),                 \
      __huz_static_stack_buf_align(stack)             \
   )                                                  \
)

#define huz_static_stack_pop(stack) (                 \
   __huz_static_stack_pop(                            \
      __huz_static_stack_cast(stack),                 \
      __huz_static_stack_unit_size(stack)             \
   )                                                  \
)

#define huz_static_stack_pop_scrub(stack) (           \
   __huz_static_stack_pop_scrub(                      \
      __huz_static_stack_cast(stack),                 \
      __huz_static_stack_unit_size(stack),            \
      __huz_static_stack_buf_align(stack)             \
   )                                                  \
)

#define huz_static_stack_popk(stack, count) (         \
   __huz_static_stack_pop(                            \
      __huz_static_stack_cast(stack),                 \
      __huz_static_stack_unit_size(stack) * count     \
   )                                                  \
)

#define huz_static_stack_popk_scrub(stack, count) (   \
   __huz_static_stack_pop_scrub(                      \
      __huz_static_stack_cast(stack),                 \
      __huz_static_stack_unit_size(stack) * count,    \
      __huz_static_stack_buf_align(stack)             \
   )                                                  \
)




#ifdef HUZLIB_STATIC_STACK_IMPL


HUZLIB_IMPL_API void __huz_static_stack_clear_scrub(struct __huzlib_uchar_static_stack *restrict stack, size_t buf_align)
{
   __huzlib_assert(stack && buf_align > 0);
   unsigned char *restrict aligned = __huzlib_memalign(stack->buf, buf_align);
   __huzlib_memset(aligned, 0, stack->lenb);
   __huz_static_stack_clear(stack);
}

HUZLIB_IMPL_API void __huz_static_stack_pop_scrub(struct __huzlib_uchar_static_stack *restrict stack, size_t pop_size, size_t buf_align)
{
   __huzlib_assert(buf_align > 0);
   __huz_static_stack_pop(stack, pop_size);
   unsigned char *restrict aligned = __huzlib_memalign(stack->buf, buf_align);
   __huzlib_memset(aligned + stack->lenb, 0, pop_size);
}


#endif /* HUZLIB_STATIC_STACK_IMPL */



#ifndef HUZLIB_STATIC_STACK_NO_SHORT_NAMES

   #define STATIC_STACK                HUZ_STATIC_STACK
   #define STATIC_STACK_INIT           HUZ_STATIC_STACK_INIT

   #define static_stack_is_empty       huz_static_stack_is_empty 
   #define static_stack_is_full        huz_static_stack_is_full
   #define static_stack_len            huz_static_stack_len
   #define static_stack_peek           huz_static_stack_peek

   #define static_stack_init           huz_static_stack_init
   #define static_stack_clear          huz_static_stack_clear
   #define static_stack_clear_scrub    huz_static_stack_clear_scrub

   #define static_stack_push           huz_static_stack_push
   #define static_stack_pushk          huz_static_stack_pushk

   #define static_stack_pop            huz_static_stack_pop
   #define static_stack_popk           huz_static_stack_popk
   #define static_stack_pop_scrub      huz_static_stack_pop_scrub
   #define static_stack_popk_scrub     huz_static_stack_popk_scrub

#endif /* HUZLIB_STATIC_STACK_NO_SHORT_NAMES */



#ifdef HUZLIB_STATIC_STACK_TEST

#include <stdio.h>
// #include "pcg_basic.h"
// #include "unity.h"
//
// void setUp(void) {}
// void tearDown(void) {}

int main(void)
{
   STATIC_STACK(int, 8) stack = STATIC_STACK_INIT(stack);

   printf("stack size: %zu\n", sizeof(stack));
   static_stack_push(&stack, 2, 5, 3, 7, 4, 1);

   while (!static_stack_is_empty(&stack))
   {
      printf("%d\n", static_stack_peek(&stack));
      static_stack_pop_scrub(&stack);
   }

   return 0;
}

#endif /* HUZLIB_STATIC_STACK_TEST */


#endif /* HUZLIB_STATIC_STACK_H */
