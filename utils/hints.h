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
 * __huzlib_const__ - output depends ONLY on input (e.g., math functions)
 * __huzlib_pure__  - no side effects, may read global memory (e.g., strlen)
 *
 * Falls back to empty for unsupported compilers.
 */
#ifndef HUZLIB_PURE_HINTS
#define HUZLIB_PURE_HINTS

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)

   #define __huzlib_const__   [[unsequenced]]
   #define __huzlib_pure__    [[reproducible]]


#elif defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__) || defined(__xlC__) || defined(__zig__) || defined(__clang__) || defined(__GNUC__)

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
