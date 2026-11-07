/*
 * huz_memalign(ptr, align)
 * ------------------------
 * moves 'ptr' up to the next multiple of 'align'
 *
 * @ptr:   memory address to align
 * @align: alignment required
 *
 * Return: aligned address
 *
 * NOTE: 'align' must be a power of 2
 */
#ifndef huz_memalign

#if !(defined(huz_requal_expr) && defined(huz_assert))
   #error "Need huz_requal_expr && huz_assert"
#endif

#include <stdint.h>

static inline uintptr_t __huz_memalign(const uintptr_t addr, const size_t align)
{
   huz_assert((align > 0) && ((align & (align - 1)) == 0));
   uintptr_t offset = (align - (addr & (align - 1))) & (align - 1);
   return addr + offset;
}

#define huz_memalign(ptr, align) \
   huz_requal_expr(ptr, huz_typeof(*(ptr)), __huz_memalign((uintptr_t)(ptr), align))

#endif /* huz_memalign */



/*
 * huz_memcpy(dest, src, n)
 * ------------------------
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
#ifndef huz_memcpy
#if (                                  \
   defined(__GNUC__) ||                \
   defined(__clang__) ||               \
   defined(__INTEL_COMPILER) ||        \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__zig__)                    \
)
   #define huz_memcpy __builtin_memcpy

#elif defined(_MSC_VER)
   #pragma intrinsic(memcpy)
   #define huz_memcpy memcpy

#elif (                                            \
   (defined(__TINYC__) || defined(__SUNPRO_C)) &&  \
   (defined(__x86_64__) || defined(__i386__))      \
)
   #include <stddef.h>
 
   /*
    * rep movsb is a hardware string copy instruction.
    * GNU extended inline asm: "+D"/"+S"/"+c" bind dest/src/n to
    * rdi/rsi/rcx (edi/esi/ecx on 32-bit), which rep movsb consumes
    * and modifies. DF is guaranteed clear by the ABI.
    */
   static inline void *__huz_memcpy_asm(void *restrict dest, const void *restrict src, size_t n)
   {
      void *ret = dest;
      __asm__ volatile (
         "rep movsb"
         : "+D"(dest), 
           "+S"(src), 
           "+c"(n)
         :
         : "memory"
      );
      return ret;
   }
   #define huz_memcpy __huz_memcpy_asm
 
#elif defined(__POCC__) && defined(_M_IX86)
   #include <stddef.h>
 
   /* MSVC-style __asm block; Pelles C supports this on 32-bit x86 only. */
   static void *__huz_memcpy_asm(void *dest, const void *src, size_t n)
   {
      __asm {
         mov edi, dest
         mov esi, src
         mov ecx, n
         rep movsb
      }
      return dest;
   }
   #define huz_memcpy __huz_memcpy_asm

#else
   #include <stddef.h>
   #include <stdint.h>

   static inline void *__huz_memcpy_fallback(void *restrict dest, const void *restrict src, size_t n)
   {
      unsigned char *d = dest;
      const unsigned char *s = src;

      while (n--)
         *d++ = *s++;

      return dest;
   }

   static inline void *__huz_memcpy_word_fallback(void *restrict dest, const void *restrict src, size_t n)
   {
      unsigned char *restrict d = dest;
      const unsigned char *restrict s = (unsigned char *)src;

      /*
       * !!! WARNING !!!
       * ---------------
       * This is NOT SAFE YET, it doesn't consider 
       * relative misalignment of src & dest
       *
       * in strict-alignment architectures (ARM, RISC-V), this causes SIGBUS, and
       * in ones that allow it, this degrades performance
       *
       * I need to fix this before adding it to the default fallback
       */

      // For large sizes, use a word-by-word copying
      if (n >= sizeof(uintptr_t))
      {
         size_t words = n / sizeof(uintptr_t);
         uintptr_t *restrict dl = (uintptr_t *)d;
         const uintptr_t *restrict sl = (const uintptr_t *)s;
   
         // 1. Copy word-by-word
         while (words--)
            *dl++ = *sl++;
       
         // 2. Handle any remaining tail bytes
         d = (unsigned char *)dl;
         s = (const unsigned char *)sl;
         n %= sizeof(uintptr_t);
      }

      // For very small sizes || remaining tail bytes
      while (n--)
         *d++ = *s++;

      return dest;
   }

   #define huz_memcpy __huz_memcpy_fallback

#endif
#endif /* huz_memcpy */



/*
 * huz_memset(dest, ch, n)
 * -----------------------
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
#ifndef huz_memset
#if (                                  \
   defined(__GNUC__) ||                \
   defined(__clang__) ||               \
   defined(__INTEL_COMPILER) ||        \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__zig__)                    \
)
   #define huz_memset __builtin_memset

#elif defined(_MSC_VER)
   #pragma intrinsic(memset)
   #define huz_memset memset

#elif (                                            \
   (defined(__TINYC__) || defined(__SUNPRO_C)) &&  \
   (defined(__x86_64__) || defined(__i386__))      \
)
   #include <stddef.h>
 
   /*
    * rep stosb is a hardware string fill instruction.
    * stosb stores al; ch is passed in eax via the "a" constraint.
    */
   static inline void *__huz_memset_asm(void *dest, int ch, size_t n)
   {
      void *ret = dest;
      __asm__ volatile (
         "rep stosb"
         : "+D"(dest), "+c"(n)
         : "a"(ch)
         : "memory"
      );
      return ret;
   }
   #define huz_memset __huz_memset_asm
 
#elif defined(__POCC__) && defined(_M_IX86)
   #include <stddef.h>
 
   /* MSVC-style __asm block; Pelles C supports this on 32-bit x86 only. */
   static void *__huz_memset_asm(void *dest, int ch, size_t n)
   {
      __asm {
         mov edi, dest
         mov eax, ch
         mov ecx, n
         rep stosb
      }
      return dest;
   }
   #define huz_memset __huz_memset_asm

#else
   #include <stddef.h>
   #include <stdint.h>

   static inline void *__huz_memset_fallback(void *dest, int ch, size_t n)
   {
      unsigned char *d = dest;
      unsigned char c = (unsigned char)ch;

      while (n--)
         *d++ = c;

      return dest;
   }

   static inline void *__huz_memset_word_fallback(void *restrict dest, int ch, size_t n)
   {
      unsigned char *restrict d = dest;
      unsigned char c = (unsigned char)ch;

      // Align dest to uintptr_t boundary
      const size_t mask = sizeof(uintptr_t) - 1;
      while (n-- && ((uintptr_t)d & mask) != 0)
         *d++ = c;

      // For large sizes, use a word-by-word copying
      if (n >= sizeof(uintptr_t))
      {
         // 1. Broadcast the byte across a full word-sized pattern
         uintptr_t pattern = c;
         for (size_t i = 1; i < sizeof(uintptr_t); i++)
            pattern = (pattern << 8) | c;

         // 2. Bulk copy using word-sized chunks
         size_t words = n / sizeof(uintptr_t);
         uintptr_t *restrict dl = (uintptr_t *)d;

         while (words--)
            *dl++ = pattern;

         // 3. Handle any remaining tail bytes
         d = (unsigned char *)dl;
         n %= sizeof(uintptr_t);
      }

      // For very small sizes || remaining tail bytes
      while (n--)
         *d++ = c;

      return dest;
   }

   #define huz_memset __huz_memset_word_fallback

#endif
#endif /* huz_memset */



/*
 * huz_prefetch_read(addr)
 * -----------------------
 * Portable prefetch — compiler detection chain.
 * Always: read prefetch, non-temporal (locality 0).
 * Falls back to a no-op (void cast) on unknown compilers —
 * a no-op is always correct, just not optimal.
 */
#ifndef huz_prefetch_read

#define HUZ_PREFETCH_X86 ( \
   defined(__i386__) ||    \
   defined(__x86_64__) ||  \
   defined(_M_IX86) ||     \
   defined(_M_X64)         \
)

#define HUZ_PREFETCH_ARM ( \
   defined(__arm__) ||     \
   defined(__aarch64__) || \
   defined(_M_ARM) ||      \
   defined(_M_ARM64)       \
)

#if (                                                                   \
   (defined(__xlC__) || defined(__ibmxl__)) &&                          \
   (defined(__powerpc__) || defined(__ppc__) || defined(__POWERPC__))   \
)
   #define huz_prefetch_read(addr)      __dcbt(addr)

#elif (                                \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__SUNPRO_C) ||              \
   defined(__SUNPRO_CC)                \
)
   #if defined(__sparc) || defined(__sparc__)
      #define huz_prefetch_read(addr)   __asm__ volatile ("prefetch [%0], 1" : : "r" (addr))
   #elif HUZ_PREFETCH_X86
      #include <immintrin.h>
      #define huz_prefetch_read(addr)   _mm_prefetch((const char *)(addr), _MM_HINT_NTA)
   #else
      #define huz_prefetch_read(addr)   ((void)(addr))
   #endif

#elif (                 \
   defined(_MSC_VER) || \
   defined(__POCC__)    \
)
   #include <intrin.h>
   #if HUZ_PREFETCH_ARM
      #define huz_prefetch_read(addr)   __prefetch(addr)
   #elif HUZ_PREFETCH_X86
      #define huz_prefetch_read(addr)   _mm_prefetch((const char *)(addr), _MM_HINT_NTA)
   #else
      #define huz_prefetch_read(addr)   ((void)(addr))
   #endif

#elif (                    \
   defined(__zig__) ||     \
   defined(__TINYC__) ||   \
   defined(__clang__) ||   \
   defined(__GNUC__)       \
)
   #define huz_prefetch_read(addr)      __builtin_prefetch((addr), 0, 0)

#else
   #define huz_prefetch_read(addr)      ((void)(addr))

#endif

#endif /* huz_prefetch_read */
