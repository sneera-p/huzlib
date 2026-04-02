#ifndef _HUZLIB_PREFETCH_H
#define _HUZLIB_PREFETCH_H


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


#endif /* _HUZLIB_PREFETCH_H */
