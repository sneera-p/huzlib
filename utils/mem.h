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
