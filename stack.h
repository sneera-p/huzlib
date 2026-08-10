#ifndef HUZLIB_STACK_H
#define HUZLIB_STACK_H


#define HUZLIB_STACK_ALLOC_MIN_ALIGN 16
// #define HUZLIB_STACK_NO_STD_MALLOC
// #define HUZLIB_STACK_NO_SHORT_NAMES


#ifdef HUZLIB_STATIC_STACK_IMPL
   #define HUZLIB_IMPL
#endif

#ifdef HUZLIB_STATIC_STACK_SHARED
   #define HUZLIB_SHARED
#endif


#ifndef HUZLIB_STACK_INCLUDES
#define HUZLIB_STACK_INCLUDES

#include "utils/types.h"
#include "utils/hints.h"
#include "utils/assert.h"
#include "utils/mem.h"
#include "utils/api.h"

#endif /* HUZLIB_STACK_INCLUDES */



#include <stddef.h>
#include <stdbool.h>
#include <stdalign.h>


struct stack_callbacks
{
   /*
    * The allocator context passed as the first argument to alloc() and free().
    * Set to NULL if your allocator does not need a context (e.g. bare malloc/free).
    */
   void *ctx;

   /*
    * alloc(ctx, size) -- Allocate `size` bytes and return a pointer to them.
    *
    * ALIGNMENT CONTRACT:
    *
    * The returned pointer MUST be aligned to at least HUZLIB_STACK_ALLOC_MIN_ALIGN.
    * The stack over-allocates by (buf_align - MIN_ALIGN) bytes per chunk to
    * guarantee that __huzlib_memalign() can always find a valid aligned start
    * within the allocation. If your allocator returns a pointer with less than
    * MIN_ALIGN, __huzlib_memalign() may walk past the allocation boundary,
    * causing a heap corruption or a segfault.
    *
    * All serious allocators already satisfy this: malloc, jemalloc, mimalloc, and
    * typical arena bump-allocators all align to at least 16 bytes. If yours does
    * not, align your base pointer manually before returning.
    *
    * Returns NULL on failure. The stack will assert on a NULL return in debug
    * builds. In release builds, a NULL return is undefined behaviour.
    */
   void *(*alloc)(void *ctx, size_t size);

   /*
    * free(ctx, mem) -- Release a pointer previously returned by alloc().
    *
    * The stack never calls free(NULL). Passing NULL is undefined behaviour
    * and your implementation does not need to handle it.
    */
   void (*free)(void *ctx, void *mem);
};


/*
 * HUZ_STACK_CHUNK(type, capacity, [name])
 * ---------------------------------------
 * The building block of the dynamic stack.
 *
 * A chunk is a singly-linked list node. It contains a pointer to the chunk 
 * below it (`prev`), and a buffer of raw bytes (`buf`).
 *
 * Notice the macro trick: if you pass an empty CAPACITY, the array becomes 
 * a Flexible Array Member (FAM): `TYPE buf[]`. This is crucial for the 
 * hidden backbone struct, which we define later to handle the "byte world".
 *
 * C macros cannot count arguments, and ISO C forbids empty __VA_ARGS__. 
 * To solve this, we always pass dummy arguments down to the inner macro. 
 * The inner macro expects exactly 4 arguments. If you don't provide a name, 
 * the macro inserts an empty slot, resulting in an anonymous struct. The 
 * final dummy argument (_1) is just there to satisfy the compiler's 
 * requirement for non-empty variadic arguments; it is completely ignored.
 */
#define __HUZ_STACK_CHUNK(TYPE, CAPACITY, NAME, ...)  \
struct NAME {                                         \
   void *prev;                                        \
   TYPE buf[CAPACITY];                                \
}

#define HUZ_STACK_CHUNK(...) __HUZ_STACK_CHUNK(__VA_ARGS__, /* empty */, _1)

#define __huz_stack_chunk_type(chunk)      typeof((chunk)->buf[0])
#define __huz_stack_chunk_size(chunk)      sizeof((chunk)->buf)
#define __huz_stack_chunk_unit_size(chunk) sizeof(__huz_stack_chunk_type(chunk))
#define __huz_stack_chunk_align(chunk)     alignof(__huz_stack_chunk_type(chunk))
#define __huz_stack_chunk_cap(chunk)       (__huz_stack_chunk_size(chunk) / __huz_stack_chunk_unit_size(chunk))


/*
 * HUZ_STACK(type, capacity, [name])
 * ---------------------------------
 * Create a dynamic, chunked stack type.
 *
 * You can call this in two ways:
 *   HUZ_STACK(int, 64)           -> anonymous struct
 *   HUZ_STACK(int, 64, mystack)  -> named struct "mystack"
 *
 * The main stack structure contains:
 * 1. top_lenb: The number of bytes currently used in the top chunk.
 * 2. top: A raw pointer to the top chunk. We use a raw NULL pointer instead 
 *    of a sentinel node because a sentinel would require allocating a full 
 *    chunk (wasting CAPACITY * sizeof(TYPE) bytes) just to represent an 
 *    empty stack. A NULL check is virtually free on modern CPUs.
 *
 *    Additionaly, the caller passes a `struct stack_callbacks` on operations that 
 *    require memory management. The callback struct contains alloc(), free() 
 *    functions and a allocator_context so different stacks can use different 
 *    memory pools (e.g., standard malloc, custom arena, or kernel kmalloc)
 *
 *
 * THE PROBLEM WITH DYNAMIC ARRAYS (realloc):
 *
 * The traditional way to build a dynamic stack is a single array that grows 
 * using realloc(). But realloc() has a fatal flaw: when the array runs out 
 * of space, the OS might not be able to expand it in place. It has to allocate 
 * a brand new, larger block of memory and COPY every single existing element 
 * to the new location.
 *
 * If your stack has 1 million elements, that's a massive, cache-destroying 
 * O(N) copy operation just to push one more item.
 *
 *
 * THE VISION: CHUNKED LAZY ALLOCATION
 *
 * To avoid copying, we abandon the single array. Instead, we build a 
 * SINGLY-LINKED LIST of fixed-size memory chunks.
 *
 * When the current chunk gets full, we don't copy anything. We just allocate 
 * a brand new chunk, point its `prev` to the old chunk, and make it the new top.
 * Pushing to a full stack is an O(1) operation. No copying. No cache thrashing.
 *
 *
 * THE "EMPTY CHUNK" HYSTERESIS CACHE (Lazy Free):
 *
 * What happens when we pop elements? 
 * If we pop the very last element of a chunk, `top_lenb` becomes 0. The chunk 
 * is now completely empty. 
 *
 * Do we free it immediately? No! We keep it.
 *
 * If the very next operation is a push, we just reuse this empty chunk. We 
 * avoid a `malloc` call entirely. 
 *
 * We only free the empty chunk if we try to pop *again*. At that point, we 
 * realize the empty chunk is useless, so we free it and drop down to the 
 * previous chunk.
 *
 * This creates a brilliant hysteresis loop: the stack will *always* maintain 
 * exactly one empty chunk at the top. This completely eliminates the 
 * `malloc`/`free` overhead when a workload oscillates around a chunk boundary 
 * (e.g., pushing and popping exactly 64 items repeatedly).
 */
#define __HUZ_STACK(TYPE, CHUNK_CAPACITY, NAME, ...)  \
struct NAME {                                         \
   size_t top_lenb;                                   \
   HUZ_STACK_CHUNK(TYPE, CHUNK_CAPACITY) *top;        \
}

#define HUZ_STACK(...) __HUZ_STACK(__VA_ARGS__, /* empty */, _1)
#define HUZ_STACK_INIT(name) (   \
   (typeof(name)) {              \
      .top_lenb = 0,             \
      .top = NULL,               \
   }                             \
)

#define __huz_stack_type(stack)        __huz_stack_chunk_type((stack)->top)
#define __huz_stack_size(stack)        __huz_stack_chunk_size((stack)->top)
#define __huz_stack_unit_size(stack)   __huz_stack_chunk_unit_size((stack)->top)
#define __huz_stack_buf_align(stack)   __huz_stack_chunk_align((stack)->top)
#define __huz_stack_cap(stack)         __huz_stack_chunk_cap((stack)->top)


/*
 * struct __huzlib_stack
 * ---------------------
 * The hidden byte backbones for the dynamic chunked stack.
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
 * At the hardware level, a dynamic stack is just a linked list of memory 
 * chunks. It allocates chunks, links them, pushes bytes in, and pops bytes 
 * out. The stack doesn't need to know what those bytes mean. Only your 
 * application cares.
 *
 * So, we make two fake structs that hold raw bytes:
 *
 *    struct __huzlib_stack_chunk {
 *       struct __huzlib_stack_chunk *prev; // pointer to the chunk below
 *       unsigned char buf[];               // the raw bytes (Flexible Array Member)
 *    };
 *
 *    struct __huzlib_stack {
 *       size_t top_lenb;                   // length in bytes of the top chunk
 *       struct __huzlib_stack_chunk *top;  // pointer to the top chunk
 *    };
 *
 * WHY THE FAM (Flexible Array Member)?
 *
 *    The chunk's FAM has no size. It doesn't say "buf[64]" or "buf[1024]". 
 *    It just says "buf[]". The real size comes from the struct that wraps it.
 *
 *    When you write `HUZ_STACK(int, 64, mystack)`, the macro creates a chunk 
 *    with `int buf[64]`. Two different structs, but they share the exact same 
 *    foundation: a `prev` pointer at the front, followed by a buffer at the back.
 *
 *
 * THE ALIGNMENT & PADDING TRAP:
 *
 * But what happens if you create a stack of a type that has huge alignment 
 * requirements? For example, a massive SIMD vector that must sit on a clean 
 * 16-byte or 32-byte memory boundary?
 *
 * In our chunk, the `prev` pointer sits at the very front (8 bytes on 64-bit). 
 * If your type requires 16-byte alignment, the compiler will secretly inject 
 * 8 bytes of dead space (padding) between `prev` and `buf` to make sure your 
 * data lines up perfectly for the CPU.
 *
 * Crucially, our fake backbone chunk uses an `unsigned char` buffer. Since 
 * bytes only need 1-byte alignment, the fake struct will NEVER have padding. 
 * Its `buf` always starts immediately after `prev`. 
 *
 * Because of this hidden padding discrepancy, the user's real buffer and 
 * our fake buffer don't always start at the exact same offset!
 *
 * To make this casting 100% safe on any processor (including ARM or ESP32), 
 * we use two clever tricks:
 *
 * 1. The Offset Zero Invariant:
 *
 *    The `top_lenb` variable sits at the very front of the main stack struct. 
 *    Offset zero. No padding can ever exist before it. This means converting 
 *    your main stack pointer into the fake backbone pointer is always safe:
 *
 *    #define __huz_stack_cast(stack) \
 *       container_of(&(stack)->top_lenb, struct __huzlib_stack, top_lenb)
 *
 * 2. Dynamic Alignment Math:
 *
 *    Our real functions never read `chunk->buf` directly. Instead, the macros 
 *    pass the real type's alignment requirement down to the engine. The engine 
 *    runs a super-fast math function:
 *
 *    unsigned char *aligned = __chunk_buf_alignup((uintptr_t)&chunk->buf, align);
 *
 *    If your type forced the compiler to insert padding, `__chunk_buf_alignup` 
 *    calculates it instantly and skips right past the dead space to find your 
 *    real data. If there is no padding, the math does nothing and costs zero 
 *    overhead.
 *
 *
 * THE WORKERS UNDER THE HOOD:
 *
 * The real functions only know the fake structs. They allocate chunks, link 
 * them together, push bytes, pop bytes, and free empty chunks. They never 
 * see your ints or floats:
 * 
 *    void __stack_push(struct __huzlib_stack *stack, size_t size, 
 *                      size_t unit_size, size_t align, const void *new);
 *
 * A macro translation layer wraps these functions. They give the real functions 
 * the byte sizes of your chunk buffer, the byte size of one element, the 
 * alignment of TYPE, and a pointer to your data.
 *
 *    #define stack_push(stack, new) (    \
 *       __stack_push(                    \
 *          __huz_stack_cast(stack),      \
 *          __huz_stack_size(stack),      \
 *          __huz_stack_unit_size(stack), \
 *          __huz_stack_buf_align(stack), \
 *          (void *)(new)                 \
 *       )                                \
 *    )
 *
 * The real functions live in the "byte world," moving raw memory around and 
 * managing the linked list. When you peek at the stack, the macro layer 
 * instantly casts the raw byte pointer back to your original type:
 *
 *    #define stack_peek(stack) (         \
 *       (__huz_stack_type(stack) *)      \
 *       __stack_peek(                    \
 *          __huz_stack_cast(stack),      \
 *          __huz_stack_size(stack),      \
 *          __huz_stack_unit_size(stack), \
 *          __huz_stack_buf_align(stack)  \
 *       )                                \
 *    )
 *
 * The caller sees an int, a float, or a custom struct. The stack never 
 * cared. It just allocated a chunk, linked it, and moved bytes around.
 *
 * One set of functions. Works for every type you throw at it. No code bloat. 
 * No template explosion. Just bytes, linked list pointers, a little math, 
 * and safe pointer casting.
 *
 * These fake structs are the foundation. Everything else is a thin layer of 
 * macros on top. The real work happens here, in the byte world, far away 
 * from your types and your fancy abstractions.
 */
HUZ_STACK(unsigned char, /* empty */, __huzlib_stack);
#define __huz_stack_cast(stack)        container_of(&(stack)->top_lenb, struct __huzlib_stack, top_lenb) 

HUZ_STACK_CHUNK(unsigned char, /* empty */, __huzlib_stack_chunk);
#define __huz_stack_chunk_cast(chunk)  container_of(&(chunk)->prev, struct __huzlib_stack_chunk, prev)




extern HUZLIB_IMPL_API __huzlib_pure__ size_t __huz_stack_len(const struct __huzlib_stack *stack, size_t size, size_t unit_size) __huzlib_reproducible__;
extern HUZLIB_IMPL_API void __huz_stack_push_resize(struct __huzlib_stack *stack, const struct stack_callbacks *cb, const void *new, size_t push_size, size_t size, size_t buf_align, struct __huzlib_stack_chunk *restrict chunk, size_t remaining);
extern HUZLIB_IMPL_API void __huz_stack_pop_resize(struct __huzlib_stack *stack, const struct stack_callbacks *cb, size_t pop_size, size_t size);
extern HUZLIB_IMPL_API void __huz_stack_pop_scrub(struct __huzlib_stack *stack, const struct stack_callbacks *cb, size_t pop_size, size_t size, size_t buf_align);
extern HUZLIB_IMPL_API void __huz_stack_clear(struct __huzlib_stack *stack, const struct stack_callbacks *cb);
extern HUZLIB_IMPL_API void __huz_stack_clear_scrub(struct __huzlib_stack *stack, const struct stack_callbacks *cb, size_t size, size_t buf_align);
extern HUZLIB_IMPL_API void __huz_stack_destroy(struct __huzlib_stack *stack, const struct stack_callbacks *cb);
extern HUZLIB_IMPL_API void __huz_stack_destroy_scrub(struct __huzlib_stack *stack, const struct stack_callbacks *cb, size_t size, size_t buf_align);




#ifndef HUZLIB_STACK_INLINE_IMPL
#define HUZLIB_STACK_INLINE_IMPL

/*
 * ---------------------------
 * --- STACK_CHUNK helpers ---
 * ---------------------------
 */

HUZLIB_INTERNAL_API __huzlib_pure__ void *__huz_stack_chunk_peek(struct __huzlib_stack_chunk *restrict chunk, size_t unit_size, size_t buf_align, size_t lenb) __huzlib_reproducible__
{
   __huzlib_assert(chunk && (lenb > 0));
   unsigned char *restrict aligned = __huzlib_memalign(chunk->buf, buf_align);
   return (void *)(aligned + lenb - unit_size);
}

HUZLIB_INTERNAL_API void __huz_stack_chunk_push(struct __huzlib_stack_chunk *restrict chunk, size_t push_size, size_t size, size_t buf_align, size_t lenb, const void *new)
{
   __huzlib_assert(chunk && (push_size > 0) && (size > 0) && (buf_align > 0) && (lenb + push_size <= size) && new);
   unsigned char *restrict aligned = __huzlib_memalign(chunk->buf, buf_align);
   __huzlib_memcpy(aligned + lenb, new, push_size);
}

HUZLIB_INTERNAL_API void __huz_stack_chunk_scrub(struct __huzlib_stack_chunk *restrict chunk, size_t size, size_t buf_align, size_t lenb)
{
   __huzlib_assert(chunk && (size > 0) && (buf_align > 0));
   unsigned char *restrict aligned = __huzlib_memalign(chunk->buf, buf_align);
   __huzlib_memset(aligned + lenb, 0, size - lenb);
}

HUZLIB_INTERNAL_API void __huz_stack_chunk_scrub_dummy(struct __huzlib_stack_chunk *restrict chunk, size_t size, size_t buf_align, size_t lenb)
{
   (void)chunk;
   (void)size;
   (void)buf_align;
   (void)lenb;
}


/*
 * ---------------------
 * --- STACK helpers ---
 * ---------------------
 */

HUZLIB_INTERNAL_API size_t __huzlib_const__ __huz_stack_alloc_size(const size_t size, const size_t buf_align) __huzlib_unsequenced__
{
   __huzlib_assert((buf_align > 0) && (size > 0));
   const size_t padding = (buf_align > HUZLIB_STACK_ALLOC_MIN_ALIGN)
      ? buf_align - HUZLIB_STACK_ALLOC_MIN_ALIGN
      : 0;
   return sizeof(struct __huzlib_stack_chunk) + padding + size;
}

HUZLIB_INTERNAL_API void __huz_stack_clear_impl(struct __huzlib_stack *restrict stack, const struct stack_callbacks *restrict cb, size_t size, size_t buf_align, void (*scrub)(struct __huzlib_stack_chunk *, size_t, size_t, size_t))
{
   __huzlib_assert(stack && cb && scrub);
   struct __huzlib_stack_chunk *restrict chunk = __huz_stack_chunk_cast(stack->top);

   if (__huzlib_likely__(chunk))
   {
      while (chunk->prev)
      {
         struct __huzlib_stack_chunk *del = chunk;
         chunk = chunk->prev;
         scrub(del, size, buf_align, 0);
         cb->free(cb->ctx, del);
      }

      stack->top_lenb = 0;
      stack->top = (void *)chunk;
   }
}


/*
 * -----------------
 * --- STACK API ---
 * -----------------
 */

HUZLIB_INLINE_API __huzlib_pure__ bool __huz_stack_is_empty(const struct __huzlib_stack *restrict stack) __huzlib_reproducible__
{
   __huzlib_assert(stack);
   /*
    * The stack exists as chunks. So if the stack is truly empty, there is either
    *    1. no chunks
    *    2. only one chunk with no elements (lenb == 0)
    */
   struct __huzlib_stack_chunk *restrict chunk = __huz_stack_chunk_cast(stack->top);
   return (chunk == NULL) || (chunk->prev == NULL && stack->top_lenb == 0);
}

HUZLIB_INLINE_API __huzlib_pure__ bool __huz_stack_is_full(const struct __huzlib_stack *restrict stack, size_t size) __huzlib_reproducible__
{
   __huzlib_assert(stack && (size > 0));
   /*
    * By definition a new chunk is allocated only when the previous one is full.
    * So we don't need to check if all chunks are full nor do we need to track 
    * the lenght of all the chunks, just the top-chunk's will do.
    *
    * So to check if the stack is full, we only need to check that the topmost 
    * chunks's lenght in bytes (lenb) is equal to it's size (CAPACITY in bytes)
    */
   return stack->top_lenb == size;
}

HUZLIB_INLINE_API __huzlib_pure__ void *__huz_stack_peek(const struct __huzlib_stack *restrict stack, size_t size, size_t unit_size, size_t buf_align) __huzlib_reproducible__
{
   __huzlib_assert(!__huz_stack_is_empty(stack));
   struct __huzlib_stack_chunk *restrict chunk = __huz_stack_chunk_cast(stack->top);

   if (__huzlib_unlikely__(stack->top_lenb == 0))
   {
      /*
       * The top chunk is empty, we must go to the chunk
       * before that to obtain the topmost element in the stack
       * (there is a previous chunk always - !__stack_is_empty(stack))
       */
      chunk = chunk->prev;
      return __huz_stack_chunk_peek(chunk, unit_size, buf_align, size);
   }
   else
   {
      return __huz_stack_chunk_peek(chunk, unit_size, buf_align, stack->top_lenb);
   }
}

HUZLIB_INLINE_API void __huz_stack_init(struct __huzlib_stack *restrict stack)
{
   __huzlib_assert(stack);
   *stack = HUZ_STACK_INIT(*stack);
}

HUZLIB_INLINE_API void __huz_stack_push(struct __huzlib_stack *restrict stack, const struct stack_callbacks *restrict cb, const void *new, size_t push_size, size_t size, size_t buf_align)
{
   __huzlib_assert(stack && (push_size > 0) && (size > 0) && (buf_align > 0) && new && cb);

   struct __huzlib_stack_chunk *restrict chunk = __huz_stack_chunk_cast(stack->top);
   size_t remaining = size - stack->top_lenb;

   if (__huzlib_unlikely__(chunk == NULL))
   {
      /*
       * Before the first time the stack is used, stack->top is NULL (lazy load)
       * therefore we must allocate a chunk here
       */
      chunk = cb->alloc(cb->ctx, __huz_stack_alloc_size(size, buf_align));
      chunk->prev = NULL;
   }

   if (__huzlib_likely__(push_size <= remaining))
   {
      /*
       * Here, the elements pushed can be filled 
       * within the current chunk
       */
      __huz_stack_chunk_push(chunk, size, push_size, buf_align, stack->top_lenb, new);
      stack->top_lenb += push_size;
      stack->top = (void *)chunk;
   }
   else
   {
      /*
       * The elements pushed cross chunk boundaries here,
       * that means we have to allocate new chunks
       */
      __huz_stack_push_resize(stack, cb, new, push_size, size, buf_align, chunk, remaining);
   }
}

HUZLIB_INLINE_API void __huz_stack_pop(struct __huzlib_stack *restrict stack, const struct stack_callbacks *restrict cb, size_t pop_size, size_t size)
{
   __huzlib_assert(!__huz_stack_is_empty(stack) && cb && (pop_size > 0) && (size > 0));

   if (pop_size <= stack->top_lenb)
   {
      /*
       * Here, the elements poped are
       * within the current chunk
       */
      stack->top_lenb -= pop_size;
   }
   else
   {
      /*
       * The elements poped cross chunk boundaries here,
       * that means we have to free some chunks
       */
      __huz_stack_pop_resize(stack, cb, pop_size, size);
   }
}


#endif /* HUZLIB_STACK_INLINE_IMPL */



#define huz_stack_is_empty(stack) (                \
   __huz_stack_is_empty(                           \
      __huz_stack_cast(stack)                      \
   )                                               \
)

#define huz_stack_is_full(stack) (                 \
   __huz_stack_is_full(                            \
      __huz_stack_cast(stack),                     \
      __huz_stack_size(stack)                      \
   )                                               \
)

#define huz_stack_len(stack) (                     \
   __huz_stack_len(                                \
      __huz_stack_cast(stack),                     \
      __huz_stack_size(stack),                     \
      __huz_stack_unit_size(stack)                 \
   )                                               \
)

#define huz_stack_peek(stack) (                    \
   *(__huz_stack_type(stack) *)                    \
   __huz_stack_peek(                               \
      __huz_stack_cast(stack),                     \
      __huz_stack_size(stack),                     \
      __huz_stack_unit_size(stack),                \
      __huz_stack_buf_align(stack)                 \
   )                                               \
)

#define huz_stack_init(stack) (                    \
   __huz_stack_init(                               \
      __huz_stack_cast(stack)                      \
   )                                               \
)

#define huz_stack_clear(stack, cb) (               \
   __huz_stack_clear(                              \
      __huz_stack_cast(stack),                     \
      (cb)                                         \
   )                                               \
)

#define huz_stack_clear_scrub(stack, cb) (         \
   __huz_stack_clear_scrub(                        \
      __huz_stack_cast(stack),                     \
      (cb),                                        \
      __huz_stack_size(stack),                     \
      __huz_stack_buf_align(stack)                 \
   )                                               \
)

#define huz_stack_destroy(stack, cb) (             \
   __huz_stack_destroy(                            \
      __huz_stack_cast(stack),                     \
      (cb)                                         \
   )                                               \
)

#define huz_stack_destroy_scrub(stack, cb) (       \
   __huz_stack_destroy_scrub(                      \
      __huz_stack_cast(stack),                     \
      (cb),                                        \
      __huz_stack_size(stack),                     \
      __huz_stack_buf_align(stack)                 \
   )                                               \
)

#define huz_stack_push(stack, cb, ...) (           \
   __huz_stack_push(                               \
      __huz_stack_cast(stack),                     \
      (cb),                                        \
      (void *)tmpptr(                              \
         __huz_stack_type(stack),                  \
         __VA_ARGS__                               \
      ),                                           \
      sizeof(tmpptr(                               \
         __huz_stack_type(stack),                  \
         __VA_ARGS__                               \
      )),                                          \
      __huz_stack_size(stack),                     \
      __huz_stack_buf_align(stack)                 \
   )                                               \
)

#define huz_stack_pushk(stack, cb, new, count) (   \
   __huz_stack_push(                               \
      __huz_stack_cast(stack),                     \
      (cb),                                        \
      typecheck_expr(                              \
         __huz_stack_type(stack),                  \
         (new)[0],                                 \
         (void *)(new)                             \
      ),                                           \
      __huz_stack_unit_size(stack) * count,        \
      __huz_stack_size(stack),                     \
      __huz_stack_buf_align(stack)                 \
   )                                               \
)

#define huz_stack_pop(stack, cb) (                 \
   __huz_stack_pop(                                \
      __huz_stack_cast(stack),                     \
      (cb),                                        \
      __huz_stack_unit_size(stack),                \
      __huz_stack_size(stack)                      \
   )                                               \
)

#define huz_stack_pop_scrub(stack, cb) (           \
   __huz_stack_pop_scrub(                          \
      __huz_stack_cast(stack),                     \
      (cb),                                        \
      __huz_stack_unit_size(stack),                \
      __huz_stack_size(stack),                     \
      __huz_stack_buf_align(stack)                 \
   )                                               \
)

#define huz_stack_popk(stack, cb, count) (         \
   __huz_stack_pop(                                \
      __huz_stack_cast(stack),                     \
      (cb),                                        \
      __huz_stack_unit_size(stack) * count,        \
      __huz_stack_size(stack)                      \
   )                                               \
)

#define huz_stack_popk_scrub(stack, cb, count) (   \
   __huz_stack_pop_scrub(                          \
      __huz_stack_cast(stack),                     \
      (cb),                                        \
      __huz_stack_unit_size(stack) * count,        \
      __huz_stack_size(stack),                     \
      __huz_stack_buf_align(stack)                 \
   )                                               \
)


#ifdef HUZLIB_STACK_IMPL

HUZLIB_IMPL_API __huzlib_pure__ size_t __huz_stack_len(const struct __huzlib_stack *restrict stack, size_t size, size_t unit_size) __huzlib_reproducible__
{
   __huzlib_assert(stack && (size > 0) && (unit_size > 0));

   /*
    * Like we said before the new chunk is onlu allocated only when the previous one is full.
    * So to caculate the lenght of stack we count how many chunks are there, get the capacity
    * of one and add to the current ones len
    *
    *    Lenght = (No. of chunks - 1) * CHUNK_CAPACITY + (top_lenb) / unit_size
    */

   struct __huzlib_stack_chunk *restrict chunk = __huz_stack_chunk_cast(stack->top);
   if (chunk)
   {
      chunk = chunk->prev;
      size_t full_chunks = 0;
      while (chunk)
      {
         chunk = chunk->prev;
         full_chunks++;
      }
      return full_chunks * (size / unit_size) + (stack->top_lenb / unit_size);
   }
   else
   {
      return 0;
   }
}

HUZLIB_IMPL_API void __huz_stack_push_resize(struct __huzlib_stack *restrict stack, const struct stack_callbacks *restrict cb, const void *new, size_t push_size, size_t size, size_t buf_align, struct __huzlib_stack_chunk *restrict chunk, size_t remaining)
{
   __huzlib_assert(stack && (push_size > 0) && (size > 0) && (buf_align > 0) && new && cb);
   const size_t ALLOC_SIZE = __huz_stack_alloc_size(size, buf_align);

   /*
    * The elements pushed cross chunk boundaries here,
    * that means we have to allocate new chunks
    */

   __huz_stack_chunk_push(chunk, size, remaining, buf_align, stack->top_lenb, new);
   new = (void *)((unsigned char *)new + remaining);
   push_size -= remaining;

   struct __huzlib_stack_chunk *restrict next;
   while (push_size > size)
   {
      next = cb->alloc(cb->ctx, ALLOC_SIZE);
      next->prev = chunk;
      chunk = next;

      __huz_stack_chunk_push(chunk, size, size, buf_align, 0, new);
      new = (const unsigned char *)new + size;
      push_size -= size;
   }

   next = cb->alloc(cb->ctx, ALLOC_SIZE);
   next->prev = chunk;
   chunk = next;

   __huz_stack_chunk_push(chunk, size, push_size, buf_align, 0, new);
   stack->top_lenb = push_size;
   stack->top = (void *)chunk;
}

HUZLIB_IMPL_API void __huz_stack_pop_resize(struct __huzlib_stack *restrict stack, const struct stack_callbacks *restrict cb, size_t pop_size, size_t size)
{
   __huzlib_assert(!__huz_stack_is_empty(stack) && cb && (pop_size > 0) && (size > 0));

   /*
    * The elements poped cross chunk boundaries here,
    * that means we have to free some chunks
    */
   struct __huzlib_stack_chunk *restrict chunk = __huz_stack_chunk_cast(stack->top);
   struct __huzlib_stack_chunk *del;

   pop_size -= stack->top_lenb;
   while (true)
   {
      del = chunk;
      chunk = chunk->prev;

      if (pop_size <= size)
         break;

      cb->free(cb->ctx, del);
      pop_size -= size;
   }

   stack->top_lenb = size - pop_size;
   stack->top = (void *)chunk;
}

HUZLIB_IMPL_API void __huz_stack_pop_scrub(struct __huzlib_stack *restrict stack, const struct stack_callbacks *restrict cb, size_t pop_size, size_t size, size_t buf_align)
{
   __huz_stack_pop(stack, cb, pop_size, size);
   __huz_stack_chunk_scrub(__huz_stack_chunk_cast(stack->top), size, buf_align, stack->top_lenb);
}

HUZLIB_IMPL_API void __huz_stack_clear(struct __huzlib_stack *restrict stack, const struct stack_callbacks *restrict cb)
{
   __huz_stack_clear_impl(stack, cb, 0, 0, __huz_stack_chunk_scrub_dummy);
}

HUZLIB_IMPL_API void __huz_stack_clear_scrub(struct __huzlib_stack *restrict stack, const struct stack_callbacks *restrict cb, size_t size, size_t buf_align)
{
   __huz_stack_clear_impl(stack, cb, size, buf_align, __huz_stack_chunk_scrub);
}

HUZLIB_IMPL_API void __huz_stack_destroy(struct __huzlib_stack *stack, const struct stack_callbacks *cb)
{
   __huzlib_assert(stack);
   if (__huzlib_likely__(stack->top))
   {
      __huz_stack_clear_impl(stack, cb, 0, 0, __huz_stack_chunk_scrub_dummy);
      cb->free(cb->ctx, stack->top);
   }
}

HUZLIB_IMPL_API void __huz_stack_destroy_scrub(struct __huzlib_stack *stack, const struct stack_callbacks *cb, size_t size, size_t buf_align)
{
   __huzlib_assert(stack);
   if (__huzlib_likely__(stack->top))
   {
      __huz_stack_clear_impl(stack, cb, size, buf_align, __huz_stack_chunk_scrub);
      cb->free(cb->ctx, stack->top);
   }
}


#endif /* HUZLIB_STACK_IMPL */



#ifndef HUZLIB_STACK_NO_STD_MALLOC

   #include <stdlib.h>

   void *__huzlib_stack_alloc(void *ctx, size_t size)
   {
      (void)ctx;
      return malloc(size); 
   }

   void __huzlib_stack_free(void *ctx, void *mem)
   {
      (void)ctx;
      free(mem);
   }

   const struct stack_callbacks __huzlib_stack_malloc_cb = {
      .ctx = NULL,
      .alloc = __huzlib_stack_alloc,
      .free = __huzlib_stack_free,
   };

   #define huz_stack_clear_m(stack)          huz_stack_clear(stack, &__huzlib_stack_malloc_cb)
   #define huz_stack_clear_scrub_m(stack)    huz_stack_clear_scrub(stack, &__huzlib_stack_malloc_cb)
   #define huz_stack_destroy_m(stack)        huz_stack_destroy(stack, &__huzlib_stack_malloc_cb)
   #define huz_stack_destroy_scrub_m(stack)  huz_stack_destroy_scrub(stack, &__huzlib_stack_malloc_cb)

   #define huz_stack_push_m(stack, ...)      huz_stack_push(stack, &__huzlib_stack_malloc_cb, __VA_ARGS__)
   #define huz_stack_pushk_m(stack, new, n)  huz_stack_pushk(stack, &__huzlib_stack_malloc_cb, new, n)

   #define huz_stack_pop_m(stack)            huz_stack_pop(stack, &__huzlib_stack_malloc_cb)
   #define huz_stack_popk_m(stack, n)        huz_stack_popk(stack, &__huzlib_stack_malloc_cb, n)
   #define huz_stack_pop_scrub_m(stack)      huz_stack_pop_scrub(stack, &__huzlib_stack_malloc_cb)
   #define huz_stack_popk_scrub_m(stack, n)  huz_stack_popk_scrub(stack, &__huzlib_stack_malloc_cb, n)

#endif /* HUZLIB_STACK_NO_STD_MALLOC */



#ifndef HUZLIB_STACK_NO_SHORT_NAMES

   #define STACK                 HUZ_STACK
   #define STACK_INIT            HUZ_STACK_INIT

   #define stack_is_empty        huz_stack_is_empty 
   #define stack_is_full         huz_stack_is_full
   #define stack_len             huz_stack_len
   #define stack_peek            huz_stack_peek

   #define stack_init            huz_stack_init
   #define stack_clear           huz_stack_clear
   #define stack_clear_scrub     huz_stack_clear_scrub
   #define stack_destroy         huz_stack_destroy
   #define stack_destroy_scrub   huz_stack_destroy_scrub

   #define stack_push            huz_stack_push
   #define stack_pushk           huz_stack_pushk

   #define stack_pop             huz_stack_pop
   #define stack_popk            huz_stack_popk
   #define stack_pop_scrub       huz_stack_pop_scrub
   #define stack_popk_scrub      huz_stack_popk_scrub


   #ifndef HUZLIB_STACK_NO_STD_MALLOC

      #define stack_clear_m            huz_stack_clear_m
      #define stack_clear_scrub_m      huz_stack_clear_scrub_m
      #define stack_destroy_m          huz_stack_destroy_m
      #define stack_destroy_scrub_m    huz_stack_destroy_scrub_m

      #define stack_push_m             huz_stack_push_m
      #define stack_pushk_m            huz_stack_pushk_m

      #define stack_pop_m              huz_stack_pop_m
      #define stack_popk_m             huz_stack_popk_m
      #define stack_pop_scrub_m        huz_stack_pop_scrub_m
      #define stack_popk_scrub_m       huz_stack_popk_scrub_m

   #endif /* HUZLIB_STACK_NO_STD_MALLOC */

#endif /* HUZLIB_STACK_NO_SHORT_NAMES */




#ifdef HUZLIB_STACK_TEST

#include <stdio.h>
#include "pcg_basic.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
   STACK(int, 4) stack = STACK_INIT(stack);
   stack_push_m(&stack, 0, 1, 2, 3, 4, 5, 6);

   while (!stack_is_empty(&stack))
   {
      printf("[%d]\n", stack_peek(&stack));
      stack_pop_scrub_m(&stack);
   }

   stack_destroy_scrub_m(&stack);
   return 0;
}

#endif /* HUZLIB_STACK_TEST */



#endif /* HUZLIB_STACK_H */
