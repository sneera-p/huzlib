/*
 * __huzlib_assert(cond)
 * ---------------------
 * zero-dependency, environment-aware runtime assertion for a boolean condition
 * same as assert() from <assert.h> but work even in -freestanding mode
 *
 * BEHAVIOR:
 *   1. NDEBUG defined (Release/Optimized):
 *      • Expands to `(void)0`. Completely eliminated by the optimizer.
 *
 *   2. Freestanding (__STDC_HOSTED__ == 0):
 *      • <assert.h> is unavailable. Uses compiler-specific trap intrinsics:
 *        - GCC/Clang/ICC/ARM/Zig/XL: `__builtin_trap()`
 *        - MSVC/Pelles C:          `__debugbreak()`
 *        - Fallback:               Volatile null-dereference to trigger a
 *                                  hardware fault/exception.
 *      • No I/O or standard library dependencies. Safe for kernels,
 *        bootloaders, and bare-metal targets.
 *
 *   3. Hosted (default):
 *      • Delegates to <assert.h>. Prints file/line/condition to stderr
 *        and calls `abort()` on failure.
 *
 * EXAMPLES:
 *   __huzlib_assert(ptr != NULL);
 *   __huzlib_assert(index < array_length);
 *   __huzlib_assert(state == EXPECTED_STATE);
 */
#ifndef __huzlib_assert
#ifdef NDEBUG

   #define __huzlib_assert(cond) do {        \
      ((void)0);                             \
   } while (0)

#else

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
         } while (0)

      #else

         #define __huzlib_assert(cond) do {  \
            if (!(cond)) {                   \
               volatile int *__huz_trap = 0; \
               (void)*__huz_trap;            \
            }                                \
         } while (0)

      #endif

   #else

      #include <assert.h>
      #define __huzlib_assert(cond) do {     \
         assert(cond);                       \
      } while (0)

   #endif /* __STDC_HOSTED__ */

#endif /* NDEBUG */
#endif /* __huzlib_assert */
