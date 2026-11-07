/*
 * HUZ_INLINE_HINTS
 * ----------------
 * Compiler hints for function inlining control.
 *    __huz_inline__   - force function to be inlined (small, hot functions)
 *    __huz_noinline__ - prevent inlining (large functions, error paths)
 */
#ifndef HUZ_INLINE_HINTS
#define HUZ_INLINE_HINTS

#if (                                  \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__POCC__) ||                \
   defined(_MSC_VER)                   \
)
   #define __huz_inline__     __forceinline
   #define __huz_noinline__   __declspec(noinline)

#elif (                                \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__ibmxl__) ||               \
   defined(__xlC__) ||                 \
   defined(__zig__) ||                 \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)
   #define __huz_inline__     inline __attribute__((always_inline))
   #define __huz_noinline__   __attribute__((noinline))

#else
   #define __huz_inline__     inline
   #define __huz_noinline__

#endif
#endif /* HUZ_INLINE_HINTS */



/*
 * HUZ_PURE_HINTS
 * --------------
 * Function attribute hints for optimization based on side-effect analysis.
 *
 * Append BEFORE return type:
 *    __huz_const__ - output depends ONLY on input (e.g., math functions)
 *    __huz_pure__  - no side effects, may read (dereference) memory (e.g., strlen)
 *
 * Append AFTER parameter list:
 *    __huz_unsequenced__  - output depends ONLY on input (e.g., math functions)
 *    __huz_reproducible__ - no side effects, may read (dereference) memory (e.g., strlen)
 *
 * Falls back to empty for unsupported compilers.
 */
#ifndef HUZ_PURE_HINTS
#define HUZ_PURE_HINTS

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
   #define __huz_unsequenced__  [[unsequenced]]
   #define __huz_reproducible__ [[reproducible]]

#else
   #define __huz_unsequenced__
   #define __huz_reproducible__

#endif

#if (                                  \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__ibmxl__) ||               \
   defined(__xlC__) ||                 \
   defined(__zig__) ||                 \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)
   #define __huz_const__   __attribute__((const))
   #define __huz_pure__    __attribute__((pure))

#elif (                          \
   defined(__INTEL_COMPILER) ||  \
   defined(__POCC__) ||          \
   defined(_MSC_VER)             \
)
   #define __huz_const__   __declspec(noalias)
   #define __huz_pure__    __declspec(noalias)

#elif (                    \
   defined(__SUNPRO_C) ||  \
   defined(__SUNPRO_CC)    \
)
   #define __huz_const__   _Pragma("no_side_effect")
   #define __huz_pure__    _Pragma("no_side_effect")

#else
   #define __huz_const__
   #define __huz_pure__

#endif
#endif /* HUZ_PURE_HINTS */



/*
 * HUZ_LIKELY_HINTS
 * ----------------
 * Branch prediction hints to guide compiler optimization.
 *    __huz_likely__(x)   - "x" is usually true  (common case)
 *    __huz_unlikely__(x) - "x" is usually false (error handling)
 * 
 * Example:
 *   if (__huz_unlikely__(error)) {
 *       handle_error();  // moved to cold section
 *   }
 * 
 * Falls back to plain expression evaluation for other compilers.
 */
#ifndef HUZ_LIKELY_HINTS
#define HUZ_LIKELY_HINTS

#if (                                  \
   defined(__clang__) ||               \
   defined(__GNUC__) ||                \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__ibmxl__) ||               \
   defined(__xlC__)                    \
)
   #define __huz_likely__(x)     __builtin_expect(!!(x), 1)
   #define __huz_unlikely__(x)   __builtin_expect(!!(x), 0)

#else
   #define __huz_likely__(x)     (x)
   #define __huz_unlikely__(x)   (x)

#endif
#endif /* HUZ_LIKELY_HINTS */



/*
 * HUZ_SHARED_HINTS
 * -----------------
 * Shared library usage with platform-specific attributes
 *    __huz_import__ - import symbols
 *    __huz_export__ - export symbols
 */
#ifndef HUZ_SHARED_HINTS
#define HUZ_SHARED_HINTS
#if (                   \
   defined(_WIN32) ||   \
   defined(_WIN64)      \
)
   #define __huz_import__   __declspec(dllimport)
   #define __huz_export__   __declspec(dllexport)

#elif (                    \
   defined(__SUNPRO_C) ||  \
   defined(__SUNPRO_CC)    \
)
   #define __huz_import__   __global
   #define __huz_export__   __global

#elif (                                                                       \
   defined(__ARMCOMPILER_VERSION) ||                                          \
   defined(__ARMCC_VERSION) ||                                                \
   (defined(__GNUC__) && (__GNUC__ >= 4)) ||                                  \
   (defined(__clang__) && (__clang_major__ >= 3)) ||                          \
   (defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 20210000)) || \
   (defined(__has_attribute) && __has_attribute(visibility))                  \
)
   #define __huz_import__  __attribute__((visibility("default")))
   #define __huz_export__  __attribute__((visibility("default")))

#else
   #define __huz_import__
   #define __huz_export__

#endif
#endif /* HUZ_SHARED_HINTS */



/*
 * huz_unreachable()
 * -----------------
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
 *   huz_unreachable();  // all enum values handled above
 */
#ifndef huz_unreachable
#if (__STDC_VERSION__ >= 202311L) || defined(unreachable)
   #include <stddef.h>
   #define huz_unreachable()  unreachable()

#elif (                                \
   defined(__GNUC__) ||                \
   defined(__clang__) ||               \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__ZIG__) ||                 \
   defined(__xlC__) ||                 \
   defined(__ibmxl__)                  \
)
   #define huz_unreachable()  __builtin_unreachable()

#elif (                 \
   defined(_MSC_VER) || \
   defined(__POCC__)    \
)
   #define huz_unreachable()  __assume(0)

#else
   #define huz_unreachable() do {               \
      volatile int *__huz_trap##__LINE__ = 0;   \
      (void)*__huz_trap##__LINE__;              \
   } while (0)

#endif
#endif /* huz_unreachable */

