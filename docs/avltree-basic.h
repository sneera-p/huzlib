#ifndef HUZLIB_AVL_TREE_H
#define HUZLIB_AVL_TREE_H



#ifndef HUZLIB_AVL_TREE_INCLUDES
#define HUZLIB_AVL_TREE_INCLUDES

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
#define typecheck(type, expr) _Generic(   \
   (expr),                                \
   type: 1                                \
)
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
#define typecheck_expr(type, var, expr) (typecheck(type, var), (expr))
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
      typeof(((type *)0)->member) *__mcumptr = (ptr);                \
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


#endif /* HUZLIB_AVL_TREE_INCLUDES */



#include <assert.h>
#include <stdint.h>
#include <stdbool.h>


struct avl_node
{
   struct avl_node *left, *right, *parent;
   int8_t balance;
};


struct avl_root
{
   struct avl_node *node;
};

#define AVL_ROOT_INIT { \
   .node = NULL,        \
}

#define avl_root_init(root) typecheck_expr(  \
   struct avl_root, *(root),                 \
   *(root) = (struct avl_root)AVL_ROOT_INIT  \
)


#ifdef NDEBUG
   #define HUZLIB_AVL_TREE_API __huzlib_inline__ __huzlib_pure__
#else
   #define HUZLIB_AVL_TREE_API __huzlib_inline__
#endif



#ifndef NDEBUG
extern size_t avl_subtree_height(const struct avl_node *subroot);                                 // WARN: O(n) complexity, do not use in production
extern size_t avl_subtree_size(const struct avl_node *subroot);                                   // WARN: O(n) complexity, do not use in production
extern bool avl_subtree_verify(const struct avl_node *subroot);                                   // WARN: O(n) complexity, do not use in production
extern bool avl_subtree_contains(const struct avl_node *subroot, const struct avl_node *entry);   // WARN: O(n) complexity, do not use in production
extern void avl_subtree_dump(const struct avl_node *subroot, void (*dump)(struct avl_node *));    // WARN: O(n) complexity, do not use in production
#endif /* NDEBUG */

/* --- query operations --- */
extern HUZLIB_AVL_TREE_API bool avl_is_empty(const struct avl_root *root);
extern HUZLIB_AVL_TREE_API bool avl_is_first(const struct avl_root *root, const struct avl_node *entry);
extern HUZLIB_AVL_TREE_API bool avl_is_last(const struct avl_root *root, const struct avl_node *entry);

/* --- mutate operations --- */
extern HUZLIB_AVL_TREE_API void avl_link_node(struct avl_node *node, struct avl_node *parent, struct avl_node **link);
extern HUZLIB_AVL_TREE_API void avl_insert_rebalance(struct avl_root *root, struct avl_node *node, bool isleft);
extern HUZLIB_AVL_TREE_API void avl_eject(struct avl_root *root, struct avl_node *node);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_first(struct avl_root *root);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_last(struct avl_root *root);

/* --- tree traversal --- */
extern HUZLIB_AVL_TREE_API struct avl_node *avl_first(const struct avl_root *root);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_last(const struct avl_root *root);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_next(const struct avl_node *node);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_prev(const struct avl_node *node);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_preorder_next(const struct avl_node *node);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_postorder_next(const struct avl_node *node);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_levelorder_next(const struct avl_node *node, const struct avl_node **pprev);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_spiralorder_next(const struct avl_node *node, const struct avl_node **pprev);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_boundaryorder_next(const struct avl_node *node, const struct avl_node **pprev);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_verticalorder_next(const struct avl_node *node, const struct avl_node **pprev);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_diagonalorder_next(const struct avl_node *node, const struct avl_node **pprev);

#define avl_foreach(pos, root) for (                     \
   typecheck(struct avl_node, *(pos)),                   \
   typecheck(struct avl_root, *(root)),                  \
   (pos) = avl_first(root);                              \
   (pos) != NULL;                                        \
   (pos) = avl_next(pos)                                 \
)

#define avl_foreach_rev(pos, root) for (                 \
   typecheck(struct avl_node, *(pos)),                   \
   typecheck(struct avl_root, *(root)),                  \
   (pos) = avl_last(root);                               \
   (pos) != NULL;                                        \
   (pos) = avl_prev(pos)                                 \
)

#define avl_foreach_safe(pos, tmp, root) for (           \
   typecheck(struct avl_node, *(pos)),                   \
   typecheck(struct avl_node, *(tmp)),                   \
   typecheck(struct avl_root, *(root)),                  \
   (pos) = avl_first(root),                              \
   (tmp) = avl_next(pos);                                \
   (pos) != NULL;                                        \
   (pos) = (tmp),                                        \
   (tmp) = avl_next(tmp)                                 \
)

#define avl_foreach_safe_rev(pos, tmp, root) for (       \
   typecheck(struct avl_node, *(pos)),                   \
   typecheck(struct avl_node, *(tmp)),                   \
   typecheck(struct avl_root, *(root)),                  \
   (pos) = avl_last(root),                               \
   (tmp) = avl_prev(pos);                                \
   (pos) != NULL;                                        \
   (pos) = (tmp),                                        \
   (tmp) = avl_prev(tmp)                                 \
)

#define avl_foreach_from(pos) for (                      \
   typecheck(struct avl_node, *(pos));                   \
   (pos) != NULL;                                        \
   (pos) = avl_next(pos)                                 \
)

#define avl_foreach_rev_from(pos) for (                  \
   typecheck(struct avl_node, *(pos));                   \
   (pos) != NULL;                                        \
   (pos) = avl_prev(pos)                                 \
)

#define avl_foreach_preorder(pos, root) for (            \
   typecheck(struct avl_node, *(pos)),                   \
   typecheck(struct avl_root, *(root)),                  \
   (pos) = (root)->node;                                 \
   (pos) != NULL;                                        \
   (pos) = avl_preorder_next(pos)                        \
)

#define avl_foreach_postorder(pos, root) for (           \
   typecheck(struct avl_node, *(pos)),                   \
   typecheck(struct avl_root, *(root)),                  \
   (pos) = avl_first(root);                              \
   (pos) != NULL;                                        \
   (pos) = avl_postorder_next(pos)                       \
)


/* --- sub-tree traversal --- */
extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_first(const struct avl_node *subroot);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_last(const struct avl_node *subroot);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_next(const struct avl_node *subroot, const struct avl_node *node);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_prev(const struct avl_node *subroot, const struct avl_node *node);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_preorder_next(const struct avl_node *subroot, const struct avl_node *node);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_postorder_next(const struct avl_node *subroot, const struct avl_node *node);

#define avl_subtree_foreach(pos, subroot) for (                \
   typecheck(struct avl_node, *(pos)),                         \
   typecheck(struct avl_node, *(subroot)),                     \
   (pos) = avl_subtree_first(subroot);                         \
   (pos) != NULL;                                              \
   (pos) = avl_subtree_next(subroot, pos)                      \
)

#define avl_subtree_foreach_rev(pos, subroot) for (            \
   typecheck(struct avl_node, *(pos)),                         \
   typecheck(struct avl_node, *(subroot)),                     \
   (pos) = avl_subtree_last(subroot);                          \
   (pos) != NULL;                                              \
   (pos) = avl_subtree_prev(subroot, pos)                      \
)

#define avl_subtree_foreach_safe(pos, tmp, subroot) for (      \
   typecheck(struct avl_node, *(pos)),                         \
   typecheck(struct avl_node, *(tmp)),                         \
   typecheck(struct avl_node, *(subroot)),                     \
   (pos) = avl_subtree_first(subroot),                         \
   (tmp) = avl_subtree_next(subroot, pos);                     \
   (pos) != NULL;                                              \
   (pos) = (tmp),                                              \
   (tmp) = avl_subtree_next(subroot, tmp)                      \
)

#define avl_subtree_foreach_safe_rev(pos, tmp, subroot) for (  \
   typecheck(struct avl_node, *(pos)),                         \
   typecheck(struct avl_node, *(tmp)),                         \
   typecheck(struct avl_node, *(subroot)),                     \
   (pos) = avl_subtree_last(subroot),                          \
   (tmp) = avl_subtree_prev(subroot, pos);                     \
   (pos) != NULL;                                              \
   (pos) = (tmp),                                              \
   (tmp) = avl_subtree_prev(subroot, tmp)                      \
)

#define avl_subtree_foreach_preorder(pos, subroot) for (       \
   typecheck(struct avl_node, *(pos)),                         \
   typecheck(struct avl_node, *(subroot)),                     \
   (pos) = (struct avl_node *)(subroot);                       \
   (pos) != NULL;                                              \
   (pos) = avl_subtree_preorder_next(subroot, pos)             \
)

#define avl_subtree_foreach_postorder(pos, subroot) for (      \
   typecheck(struct avl_node, *(pos)),                         \
   typecheck(struct avl_node, *(subroot)),                     \
   (pos) = avl_subtree_first(subroot);                         \
   (pos) != NULL;                                              \
   (pos) = avl_subtree_postorder_next(subroot, pos)            \
)


/* --- container macros --- */
#define avl_entry(ptr, type, member)         container_of(ptr, type, member)
#define avl_first_entry(root, type, member)  (avl_is_empty(root) ? NULL : avl_entry(avl_first(root), type, member))
#define avl_last_entry(root, type, member)   (avl_is_empty(root) ? NULL : avl_entry(avl_last(root), type, member))
#define avl_root_entry(root, type, member)   (avl_is_empty(root) ? NULL : avl_entry((root)->node, type, member))


#define avl_foreach_entry(entr, root, type, member) for (                  \
   typecheck(type, *(entr)),                                               \
   typecheck(struct avl_root, *(root)),                                    \
   typecheck(struct avl_node, (((type *)0)->member)),                      \
   (entr) = (type *)avl_first(root);                                       \
   (entr) = ((entr) != NULL)                                               \
      ? avl_entry((struct avl_node *)(entr), type, member)                 \
      : NULL;                                                              \
   (entr) = (type *)avl_next(&(entr)->member)                              \
)

#define avl_foreach_entry_rev(entr, root, type, member) for (              \
   typecheck(type, *(entr)),                                               \
   typecheck(struct avl_root, *(root)),                                    \
   typecheck(struct avl_node, (((type *)0)->member)),                      \
   (entr) = (type *)avl_last(root);                                        \
   (entr) = ((entr) != NULL)                                               \
      ? avl_entry((struct avl_node *)(entr), type, member)                 \
      : NULL;                                                              \
   (entr) = (type *)avl_prev(&(entr)->member)                              \
)

#define avl_foreach_entry_safe(entr, temp, root, type, member) for (       \
   typecheck(type, *(entr)),                                               \
   typecheck(type, *(temp)),                                               \
   typecheck(struct avl_root, *(root)),                                    \
   typecheck(struct avl_node, (((type *)0)->member)),                      \
   (entr) = (type *)avl_first(root),                                       \
   (temp) = (__huzlib_likely__((entr) != NULL))                            \
      ? (type *)avl_next((struct avl_node *)(entr))                        \
      : NULL;                                                              \
   (entr) = ((entr) != NULL)                                               \
      ? avl_entry((struct avl_node *)(entr), type, member)                 \
      : NULL;                                                              \
   (entr) = (temp),                                                        \
   (temp) = (__huzlib_likely__((temp) != NULL))                            \
      ? (type *)avl_next((struct avl_node *)(temp))                        \
      : NULL                                                               \
)

#define avl_foreach_entry_safe_rev(entr, temp, root, type, member) for (   \
   typecheck(type, *(entr)),                                               \
   typecheck(type, *(temp)),                                               \
   typecheck(struct avl_root, *(root)),                                    \
   typecheck(struct avl_node, (((type *)0)->member)),                      \
   (entr) = (type *)avl_last(root),                                        \
   (temp) = (__huzlib_likely__((entr) != NULL))                            \
      ? (type *)avl_prev((struct avl_node *)(entr))                        \
      : NULL;                                                              \
   (entr) = ((entr) != NULL)                                               \
      ? avl_entry((struct avl_node *)(entr), type, member)                 \
      : NULL;                                                              \
   (entr) = (temp),                                                        \
   (temp) = (__huzlib_likely__((temp) != NULL))                            \
      ? (type *)avl_prev((struct avl_node *)(temp))                        \
      : NULL                                                               \
)

#define avl_foreach_entry_from(entr, root, type, member) for (             \
   typecheck(type, *(entr)),                                               \
   typecheck(struct avl_root, *(root)),                                    \
   typecheck(struct avl_node, (((type *)0)->member)),                      \
   (entr) = ((entr) != NULL)                                               \
      ? avl_entry((struct avl_node *)(entr), type, member)                 \
      : NULL;                                                              \
   (entr) = (type *)avl_next(&(entr)->member)                              \
)

#define avl_foreach_entry_rev_from(entr, root, type, member) for (         \
   typecheck(type, *(entr)),                                               \
   typecheck(struct avl_root, *(root)),                                    \
   typecheck(struct avl_node, (((type *)0)->member)),                      \
   (entr) = ((entr) != NULL)                                               \
      ? avl_entry((struct avl_node *)(entr), type, member)                 \
      : NULL;                                                              \
   (entr) = (type *)avl_prev(&(entr)->member)                              \
)

#define avl_foreach_entry_preorder(entr, root, type, member) for (         \
   typecheck(type, *(entr)),                                               \
   typecheck(struct avl_root, *(root)),                                    \
   typecheck(struct avl_node, (((type *)0)->member)),                      \
   (entr) = (type *)(root)->node;                                          \
   (entr) = ((entr) != NULL)                                               \
      ? avl_entry((struct avl_node *)(entr), type, member)                 \
      : NULL;                                                              \
   (entr) = (type *)avl_preorder_next(&(entr)->member)                     \
)

#define avl_foreach_entry_postorder(entr, root, type, member) for (        \
   typecheck(type, *(entr)),                                               \
   typecheck(struct avl_root, *(root)),                                    \
   typecheck(struct avl_node, (((type *)0)->member)),                      \
   (entr) = (type *)avl_first(root);                                       \
   (entr) = ((entr) != NULL)                                               \
      ? avl_entry((struct avl_node *)(entr), type, member)                 \
      : NULL;                                                              \
   (entr) = (type *)avl_postorder_next(&(entr)->member)                    \
)



#ifdef HUZLIB_AVL_TREE_IMPL

#if !defined(NDEBUG) || defined(HUZLIB_AVL_TREE_TEST)

#include <stdio.h>

size_t avl_subtree_height(const struct avl_node *restrict subroot)
{
   if (!subroot)
      return 0;

   size_t max_height = 1, height = 1;
   const struct avl_node *node = subroot, *prev = subroot->parent;
   while (node != subroot->parent) // post-order traversal
   {
      if (prev == node->parent)
      {
         /*
          * case: coming from parent
          * goto: left, if not then right
          *
          *       p
          *      /
          *     n
          *    / \
          *  (s) (t)
          */

         if (node->left)
         {
            prev = node;
            node = node->left;
            height++;
         }
         else if (node->right)
         {
            prev = node;
            node = node->right;
            height++;
         }
         else /* leaf node */
         {
            if (max_height < height)
               max_height = height;

            prev = node;
            node = node->parent;
            height--;
         }
      }
      else if (prev == node->left)
      {
         /*
          * case: coming from left child
          * goto: right child, if not parent
          *
          *     (t)
          *     /
          *    n
          *   / \
          *  p  (s)
          */

         if (node->right)
         {
            prev = node;
            node = node->right;
            height++;
         }
         else /* no right child */
         {
            prev = node;
            node = node->parent;
            height--;
         }
      }
      else
      {
         /*
          * case: coming from right child
          * goto: parent
          *
          *      (s)
          *      /
          *     n
          *    / \
          *  (x)  p
          */

         prev = node;
         node = node->parent;
         height--;
      }
   }
   return max_height;
}

size_t avl_subtree_size(const struct avl_node *restrict subroot)
{
   size_t size = 0;
   struct avl_node *restrict node;
   avl_subtree_foreach_preorder(node, subroot) // pre-order traversal takes minimum instructions comparitively
      size++;
   return size;
}

bool avl_subtree_verify(const struct avl_node *restrict node)
{
   assert(node);

   // verify linkage
   bool p = (node->parent) ? (node->parent->left == node || node->parent->right == node) : true;
   bool l = (node->left) ? (node->left->parent == node) : true;
   bool r = (node->right) ? (node->right->parent == node) : true;

   // verify balance
   int b = avl_subtree_height(node->left) - avl_subtree_height(node->right);
   bool isbalanced = (b >= -1) && (b <= 1);

   return p && l && r && isbalanced && (b == node->balance);
}

bool avl_subtree_contains(const struct avl_node *restrict subroot, const struct avl_node *restrict entry)
{
   assert(entry);

   struct avl_node *restrict node;
   avl_subtree_foreach_preorder(node, subroot)
   {
      if (node == entry)
         return true;
   }
   return false;
}

void avl_subtree_dump(const struct avl_node *restrict subroot, void (*dump)(struct avl_node *))
{
   assert(dump);

   if (!subroot)
   {
      printf("[empty tree]\n");
      return;
   }

   int indent = -1;
   struct avl_node *node, *prev = NULL;
   avl_subtree_foreach_preorder(node, subroot)
   {
      if (prev == node->parent)
      {
         /*
          * case: coming from parent
          *
          *    p
          *   /
          *  n
          */
         indent++;
      }
      else if (prev->parent == node->parent)
      {
         /*
          * case: moved to right sibling
          *
          *    x
          *   / \
          *  p   n
          */
         // indent remains the same
      }
      else
      {
         /*
          * case moving up to an ancestor, then to its right child
          *
          *      x
          *     / \
          *   ...  n
          *   /
          *  p
          */
         struct avl_node *tmp = prev;
         while (tmp != node->parent)
         {
            indent--;
            tmp = tmp->parent;
         }
         indent++;
      }

      for (int i = 0; i < indent; i++)
         printf("    ");

      if (node != subroot)
         printf("%s ", (node == node->parent->left) ? "├──" : "└──");

      dump(node);
      printf(" [%d] %s %s",
         node->balance,
         (node->left || node->right) ? "" : "🍂",
         (avl_subtree_verify(node)) ? "\033[32m" "✔" "\033[0m" : "\033[31m" "✘" "\033[0m"
      );
      putchar('\n');

      prev = node;
   }
}

#endif /* NDEBUG || HUZLIB_AVL_TREE_TEST */


/* ------------------------------------------------ */
/* --------------- query operations --------------- */
/* ------------------------------------------------ */

HUZLIB_AVL_TREE_API bool avl_is_empty(const struct avl_root *restrict root)
{
   assert(root);
   return root->node == NULL;
}

HUZLIB_AVL_TREE_API bool avl_is_first(const struct avl_root *restrict root, const struct avl_node *restrict entry)
{
   assert(root && entry);
   return entry == avl_first(root);
}

HUZLIB_AVL_TREE_API bool avl_is_last(const struct avl_root *restrict root, const struct avl_node *restrict entry)
{
   assert(root && entry);
   return entry == avl_last(root);
}


/* ------------------------------------------------- */
/* --------------- helper functions  --------------- */
/* ------------------------------------------------- */

/*
 * __avl_parent_ptr(root, node, parent, isleft)
 * ---------------------------------------------
 * Returns the address of the pointer in 'parent' that points to 'node',
 * i.e. either &parent->left, &parent->right, or &root->node if node is root.
 *
 * @root:   the tree root, not NULL
 * @node:   the node to find the parent pointer for, not NULL
 * @parent: node->parent, may be NULL if node is the root
 * @isleft: set true if node is parent's left child, false if right.
 *          retains value held if node is the root (parent == NULL).
 *
 * Return: address of the parent's child pointer that references node.
 */
static HUZLIB_AVL_TREE_API struct avl_node **__avl_parent_ptr(struct avl_root *restrict root, struct avl_node *restrict node, struct avl_node *restrict parent, bool *restrict isleft)
{
   assert(root && node && node->parent == parent && isleft);

   if (!parent)
   {
      /* isleft retains it's value when node is root */
      return &root->node;
   }
   else if (parent->left == node)
   {
      *isleft = true;
      return &parent->left;
   }
   else /* (parent->right == node) */
   {
      *isleft = false;
      return &parent->right;
   }
}

/*
 * __avl_rotate_left(node, child, link)
 * ------------------------------------
 * Standard BST left rotation on 'node'
 *
 *      n                         c
 *     / \                       / \
 *   (x)  c         ==>         n  (z)
 *       / \                   / \
 *     (y) (z)               (x) (y)
 *
 * @node:   the node rotating on, not NULL
 * @child:  node's right child, not NULL
 * @link:   node->parent's internal pointer to node, not NULL
 *
 * NOTE:
 * Does not update 'balance' fields.
 * Caller must update them after rotation.
 */
static HUZLIB_AVL_TREE_API void __avl_rotate_left(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node **restrict link)
{
   assert(node && child && link && *link == node && node->right == child);

   if (child->left)
      child->left->parent = node;

   node->right = child->left;
   child->left = node;

   child->parent = node->parent;
   node->parent = child;

   *link = child;
}

/*
 * __avl_rotate_right(node, child, link)
 * -------------------------------------
 * Standard BST right rotation on 'node'
 *
 *        n                      c
 *       / \                    / \
 *      c  (x)      ==>       (z)  n
 *     / \                        / \
 *   (z) (y)                    (y) (x)
 *
 * @node:   the node rotating on, not NULL
 * @child:  node's left child, not NULL
 * @link:   node->parent's internal pointer to node, not NULL
 *
 * NOTE:
 * Does not update 'balance' fields.
 * Caller must update them after rotation.
 */
static HUZLIB_AVL_TREE_API void __avl_rotate_right(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node **restrict link)
{
   assert(node && child && link && *link == node && node->left == child);

   if (child->right)
      child->right->parent = node;

   node->left = child->right;
   child->right = node;

   child->parent = node->parent;
   node->parent = child;

   *link = child;
}

/*
 * __avl_rotate_left_right(node, child, parent, link)
 * --------------------------------------------------
 * Standard BST left rotation on 'node' and then right rotation on 'parent'
 *
 *        p                         p                        c
 *       / \                       / \                     /   \
 *      n  (w)                    c  (w)                 n       p
 *     / \          ==>          / \          ==>       / \     / \
 *   (x)  c                     n  (z)                (x) (y) (z) (w)
 *       / \                   / \
 *     (y) (z)               (x) (y)
 *
 * @node:   the node rotating on, not NULL
 * @child:  node's right child, not NULL
 * @parent: node's parent (node is always parent's left child), not NULL
 * @link:   parent->parent's internal pointer to parent, not NULL
 *
 * NOTE:
 * Does not update 'balance' fields.
 * Caller must update them after rotation.
 */
static HUZLIB_AVL_TREE_API void __avl_rotate_left_right(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   assert(node && child && parent && link && *link == parent && node->right == child && parent->left == node);

   if (child->left) /* (y) */
      child->left->parent = node;

   if (child->right) /* (z) */
      child->right->parent = parent;

   node->parent = child;
   node->right = child->left;

   child->parent = parent->parent;
   parent->parent = child;
   parent->left = child->right;

   child->left = node;
   child->right = parent;

   *link = child;
}

/*
 * __avl_rotate_right_left(node, child, parent, link)
 * --------------------------------------------------
 * Standard BST right rotation on 'node' and then left rotation on 'parent'
 *
 *      p                       p                            c
 *     / \                     / \                         /   \
 *   (w)  n                  (w)  c                      p       n
 *       / \        ==>          / \          ==>       / \     / \
 *      c  (x)                 (z)  n                 (w) (z) (y) (x)
 *     / \                         / \
 *   (z) (y)                     (y) (x)
 *
 * @node:   the node rotating on, not NULL
 * @child:  node's left child, not NULL
 * @parent: node's parent (node is always parent's right child), not NULL
 * @link:   parent->parent's internal pointer to parent, not NULL
 *
 * NOTE:
 * Does not update 'balance' fields.
 * Caller must update them after rotation.
 */
static HUZLIB_AVL_TREE_API void __avl_rotate_right_left(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   assert(node && child && parent && link && *link == parent && node->left == child && parent->right == node);

   if (child->right) /* (y) */
      child->right->parent = node;

   if (child->left) /* (z) */
      child->left->parent = parent;

   node->parent = child;
   node->left = child->right;

   child->parent = parent->parent;
   parent->parent = child;
   parent->right = child->left;

   child->right = node;
   child->left = parent;

   *link = child;
}

/*
 * __avl_link_node(node, parent, link)
 * -----------------------------------
 * Attaches 'node' to 'parent' via 'link', the parent's pointer
 *
 *     p                              p
 *    / \                            / \
 *  (u) (v)     n         ==>      (u)  n      (v)
 *             / \                     / \
 *           (x) (y)                 (x) (y)
 *
 * @node:   the node to attach, not NULL
 * @parent: to be parent of 'node', may be NULL
 * @link:   parent's internal pointer to linking node, not NULL
 *
 * NOTE:
 * Other child 'v' (if it exists) is left dangling.
 * Relinking 'v' is the caller's responsibility.
 */
static HUZLIB_AVL_TREE_API void __avl_link_node(struct avl_node *restrict node, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   assert(node && link);
   node->parent = parent;
   *link = node;
}

/*
 * __avl_delink_node(child, parent, link)
 * -------------------------------------
 * Replaces node 'n' in the tree by splicing 'child' into its position via
 * 'link', the parent's pointer that previously pointed to 'n'.
 *
 *    p                           p
 *     \                           \
 *      n          ==>       n     (c)
 *     / \                  /
 *   (o) (c)              (o)
 *
 * @child:  the node to splice in, may be NULL
 * @parent: the parent of the node being removed, may be NULL
 * @link:   parent's internal pointer to delinking node, not NULL
 *
 * NOTE:
 * Other child 'o' (if it exists) is left dangling.
 * Relinking 'o' is the caller's responsibility.
 */
static HUZLIB_AVL_TREE_API void __avl_delink_node(struct avl_node *restrict child, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   assert(link && (*link)->parent == parent && ((*link)->left == child || (*link)->right == child));
   *link = child;
   if (child)
      child->parent = parent;
}

/*
 * __avl_eject_rebalance(root, node, isleft)
 * ------------------------------------------
 * Restores AVL balance after a node deletion by walking up the tree
 * from 'node' and fixing balance factors and doing rotations as needed.
 *
 * @root:   the tree root, not NULL
 * @node:   parent of the ejected node, may be NULL 
 * @isleft: true if the deletion was in node's left child
 *
 * The following cases are handled at each level:
 *
 *   balance == 1 or -1:
 *      height unchanged, stop.
 *
 *   balance == 0:
 *      height decreased, continue up.
 *
 *   balance == 2 (left heavy):
 *      LL: right rotate.
 *             if child was balanced (0):     height unchanged, stop.
 *             if child was left heavy (1):   height decreased, continue up.
 *      LR: left-right rotate:                height decreased, continue up.
 *
 *   balance == -2 (right heavy):
 *      RR: left rotate.
 *             if child was balanced (0):     height unchanged, stop.
 *             if child was right heavy (-1): height decreased, continue up.
 *      RL: right-left rotate:                height decreased, continue up.
 *
 * NOTE:
 * For insertion (avl_insert_rebalance), every return/continue decision
 * is INVERTED because insertion adds height while deletion removes height.
 */
static HUZLIB_AVL_TREE_API void __avl_eject_rebalance(struct avl_root *restrict root, struct avl_node *restrict node, bool isleft)
{
   assert(root);
   struct avl_node *restrict parent, *restrict child, *restrict tmp;
   struct avl_node **restrict link;

   while (__huzlib_likely__(node))
   {
      if (isleft)
         node->balance--;
      else
         node->balance++;

      parent = node->parent;
      link = __avl_parent_ptr(root, node, parent, &isleft);

      switch (node->balance)
      {
         case 1:
         case -1:
            return;

         case 0:
            node = parent;
            continue;

         case 2:
            child = node->left;
            if (child->balance >= 0)
            {
               /*
                * case LL: right rotate on 'node'
                *
                *         n                          c
                *        / \                       /   \
                *       c  (w)                   g       n
                *      / \           ==>        / \     / \
                *     g  (z)                  (x) (y) (z) (w)
                *    / \
                *  (x) (y)
                */

               __avl_rotate_right(node, child, link);

               if (child->balance == 0)
               {
                  node->balance = 1;
                  child->balance = -1;

                  return; // height unchanged
               }
               else /* (child->balance == 1) */
               {
                  node->balance = 0;
                  child->balance = 0;

                  node = parent;
                  continue; // height decreased
               }
            }
            else /* (child->balance == -1) */
            {
               /*
                * case LR: left rotate on 'child' & right rotate on 'node'
                *
                *       n                         g
                *      / \                      /   \
                *     c  (w)                  c       n
                *    / \          ==>        / \     / \
                *  (x)  g                  (x) (y) (z) (w)
                *      / \
                *    (y) (z)
                */

               tmp = child->right;
               __avl_rotate_left_right(child, tmp, node, link);

               node->balance = (tmp->balance == 1) ? -1 : 0;
               child->balance = (tmp->balance == -1) ? 1 : 0;
               tmp->balance = 0;

               node = parent;
               continue; // height decreased
            }
            unreachable();

         case -2:
            child = node->right;
            if (child->balance <= 0)
            {
               /*
                * case RR: left rotate on 'node'
                *
                *     n                            c
                *    / \                         /   \
                *  (w)  c                      n       g
                *      / \          ==>       / \     / \
                *    (z)  g                 (w) (z) (y) (x)
                *        / \
                *      (y) (x)
                */

               __avl_rotate_left(node, child, link);

               if (child->balance == 0)
               {
                  node->balance = -1;
                  child->balance = 1;

                  return; // height unchanged
               }
               else /* (child->balance == -1) */
               {
                  node->balance = 0;
                  child->balance = 0;

                  node = parent;
                  continue; // height decreased
               }
            }
            else /* (child->balance == 1) */
            {
               /*
                * case RL: right rotate on 'child' & left rotate on 'node'
                *
                *     n                           g
                *    / \                        /   \
                *  (w)  c                     n       c
                *      / \        ==>        / \     / \
                *     g  (x)               (w) (z) (y) (x)
                *    / \
                *  (z) (y)
                */

               tmp = child->left;
               __avl_rotate_right_left(child, tmp, node, link);

               node->balance = (tmp->balance == -1) ? 1 : 0;
               child->balance = (tmp->balance == 1) ? -1 : 0;
               tmp->balance = 0;

               node = parent;
               continue; // height decreased
            }
            unreachable();
      }
      unreachable();
   }
}


/* ------------------------------------------------- */
/* --------------- mutate operations --------------- */
/* ------------------------------------------------- */

/*
 * avl_link_node(node, parent, link)
 * ---------------------------------
 * Attaches 'node' to 'parent' via 'link', the parent's pointer
 * and sets 'node's children to NULL.
 *
 *     p                              p
 *    / \                            / \
 *  (u) (v)     n         ==>      (u)  n      (v)
 *             / \
 *           (x) (y)                       (x)  (y)
 *
 * @node:   the node to attach, not NULL
 * @parent: to be parent of 'node', may be NULL
 * @link:   parent's internal pointer to linking node, not NULL
 *
 * NOTE:
 * Other children 'v', 'x', and 'y' (if exists) are left dangling.
 * Relinking them is the caller's responsibility.
 */
HUZLIB_AVL_TREE_API void avl_link_node(struct avl_node *restrict node, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   assert(node && link);
   __avl_link_node(node, parent, link);
   node->left = NULL;
   node->right = NULL;
   node->balance = 0;
}

/*
 * avl_insert_rebalance(root, node, isleft)
 * -----------------------------------------
 * Restores AVL balance after a node insertion by walking up the tree
 * from 'node' and fixing balance factors and rotations as needed.
 *
 * @root:   the tree root, not NULL
 * @node:   parent of the inserted node, may be NULL if tree was empty
 * @isleft: true if the insertion was in node's left subtree
 *
 * The following cases are handled at each level:
 *
 *   balance == 0:
 *      height unchanged, stop.
 *
 *   balance == 1 or -1:
 *      height increased, continue up.
 *
 *   balance == 2 (left heavy):
 *      LL: right rotate.
 *             if child was balanced (0):     height increased, continue up.
 *             if child was left heavy (1):   height unchanged, stop.
 *      LR: left-right rotate:                height unchanged, stop.
 *
 *   balance == -2 (right heavy):
 *      RR: left rotate.
 *             if child was balanced (0):     height increased, continue up.
 *             if child was right heavy (-1): height unchanged, stop.
 *      RL: right-left rotate:                height unchanged, stop.
 *
 * NOTE:
 * For deletion (__avl_eject_rebalance), every return/continue decision
 * is INVERTED because deletion removes height while insertion adds height.
 */
HUZLIB_AVL_TREE_API void avl_insert_rebalance(struct avl_root *restrict root, struct avl_node *restrict node, bool isleft)
{
   assert(root);
   struct avl_node *restrict parent, *restrict child, *restrict tmp;
   struct avl_node **restrict link;

   while (__huzlib_likely__(node))
   {
      if (isleft)
         node->balance++;
      else
         node->balance--;

      parent = node->parent;
      link = __avl_parent_ptr(root, node, parent, &isleft);

      switch (node->balance)
      {
         case 0:
            return;

         case 1:
         case -1:
            node = parent;
            continue;

         case 2:
            child = node->left;
            if (child->balance >= 0)
            {
               /*
                * case LL: right rotate on 'node'
                *
                *         n                          c
                *        / \                       /   \
                *       c  (w)                   g       n
                *      / \           ==>        / \     / \
                *     g  (z)                  (x) (y) (z) (w)
                *    / \
                *  (x) (y)
                */

               __avl_rotate_right(node, child, link);

               if (child->balance == 0)
               {
                  node->balance = 1;
                  child->balance = -1;

                  node = parent;
                  continue; // height increased
               }
               else /* (child->balance == 1) */
               {
                  node->balance = 0;
                  child->balance = 0;

                  return; // height unchanged
               }
            }
            else /* (child->balance == -1) */
            {
               /*
                * case LR: left rotate on 'child' & right rotate on 'node'
                *
                *       n                         g
                *      / \                      /   \
                *     c  (w)                  c       n
                *    / \          ==>        / \     / \
                *  (x)  g                  (x) (y) (z) (w)
                *      / \
                *    (y) (z)
                */

               tmp = child->right;
               __avl_rotate_left_right(child, tmp, node, link);

               node->balance = (tmp->balance == 1) ? -1 : 0;
               child->balance = (tmp->balance == -1) ? 1 : 0;
               tmp->balance = 0;

               return; // height unchanged
            }
            unreachable();

         case -2:
            child = node->right;
            if (child->balance <= 0)
            {
               /*
                * case RR: left rotate on 'node'
                *
                *     n                            c
                *    / \                         /   \
                *  (w)  c                      n       g
                *      / \          ==>       / \     / \
                *    (z)  g                 (w) (z) (y) (x)
                *        / \
                *      (y) (x)
                */

               __avl_rotate_left(node, child, link);

               if (child->balance == 0)
               {
                  node->balance = -1;
                  child->balance = 1;

                  node = parent;
                  continue; // height increased
               }
               else /* (child->balance == -1) */
               {
                  node->balance = 0;
                  child->balance = 0;

                  return; // height unchanged
               }
            }
            else /* (child->balance == 1) */
            {
               /*
                * case RL: right rotate on 'child' & left rotate on 'node'
                *
                *     n                           g
                *    / \                        /   \
                *  (w)  c                     n       c
                *      / \        ==>        / \     / \
                *     g  (x)               (w) (z) (y) (x)
                *    / \
                *  (z) (y)
                */

               tmp = child->left;
               __avl_rotate_right_left(child, tmp, node, link);

               node->balance = (tmp->balance == -1) ? 1 : 0;
               child->balance = (tmp->balance == 1) ? -1 : 0;
               tmp->balance = 0;

               return; // height unchanged
            }
            unreachable();
      }
      unreachable();
   }
}

/*
 * avl_eject(root, node)
 * ----------------------
 * Removes 'node' from the tree and restores AVL balance.
 *
 * @root: the tree root, not NULL
 * @node: the node to remove, not NULL
 *
 * The following cases are handled:
 *
 *   2 children:
 *      Find in-order successor (leftmost node of right subtree).
 *      Splice successor into node's position, then rebalance
 *      from successor's original parent.
 *
 *      2a. successor is node's direct right child:
 *          rebalance starts from successor, isleft = false.
 *
 *      2b. successor is deeper in the right subtree:
 *          rebalance starts from successor's original parent, isleft = true.
 *
 *   1 child:
 *      Splice the child into node's position.
 *      Rebalance starts from node's parent.
 *
 *   0 children (leaf):
 *      Unlink node from parent.
 *      Rebalance starts from node's parent.
 */
HUZLIB_AVL_TREE_API void avl_eject(struct avl_root *restrict root, struct avl_node *restrict node)
{
   assert(root && node);

   bool isleft;
   struct avl_node *restrict parent = node->parent;
   struct avl_node **restrict link = __avl_parent_ptr(root, node, parent, &isleft);

   if (node->left && node->right)
   {
      /*
       * case: 'node' has 2 children
       *
       *         |
       *         n
       *       /   \
       *     w       s
       *    / \     / \
       *  (a) (b) (v) (x)
       */
      struct avl_node **restrict succ_link = &node->right;
      while ((*succ_link)->left)
         succ_link = &(*succ_link)->left;

      struct avl_node *restrict succ = *succ_link;
      node->left->parent = succ;
      succ->left = node->left;
      succ->balance = node->balance;

      if (succ == node->right)
      {
         /*
          * case: succ's parent is node
          *
          *         |                             |
          *         n                             s
          *       /   \                         /   \
          *     w       s         ==>         w     (x)
          *    / \       \                   / \
          *  (a) (b)     (x)               (a) (b)
          */

         __avl_link_node(succ, parent, link);
         parent = succ;
         isleft = false;
      }
      else
      {
         /*
          * case: succ's parent is descedant of node
          *
          *         |                             |
          *         n                             s
          *       /   \                         /   \
          *     w       v                     w       v
          *    / \     / \                   / \     / \
          *  (a) (b) ... (x)      ==>      (a) (b) ... (x)
          *          / \                           / \
          *         s  (y)                       (z) (y)
          *          \
          *          (z)
          */

         parent = succ->parent;
         isleft = true;
         __avl_delink_node(succ->right, parent, succ_link);
         __avl_link_node(succ, node->parent, link);

         node->right->parent = succ;
         succ->right = node->right;
      }
   }
   else if (node->left)
   {
      /*
       * case: 'node' has only left child
       *
       *       |                   |
       *       n                   s
       *      /                   / \
       *     s         ==>      (a) (b)
       *    / \
       *  (a) (b)
       */
      __avl_link_node(node->left, parent, link);
   }
   else if (node->right)
   {
      /*
       * case: 'node' has only right child
       *
       *   |                       |
       *   n                       s
       *    \                     / \
       *     s         ==>      (a) (b)
       *    / \
       *  (a) (b)
       */
      __avl_link_node(node->right, parent, link);
   }
   else
   {
      /*
       * case: 'node' is leaf node
       *
       *   |                       |
       *   n           ==>        ☠️
       */
      *link = NULL;
   }
   __avl_eject_rebalance(root, parent, isleft);
}

/*
 * avl_eject_first(root)
 * ----------------------
 * Removes and returns the first (leftmost) node from the tree and restores AVL balance.
 * The first node has no left child, so only its right child (if any) is spliced in.
 *
 * @root:   the tree root, not NULL. tree must not be empty.
 *
 * Return: pointer to the removed node. caller is responsible for freeing it.
 */
HUZLIB_AVL_TREE_API struct avl_node *avl_eject_first(struct avl_root *restrict root)
{
   assert(root && !avl_is_empty(root));

   /*
    * find leftmost node with link
    *
    *      r
    *     / \
    *   ... (y)
    *   / \
    *  n  (x)
    */
   struct avl_node **restrict link = &root->node;
   while ((*link)->left)
      link = &(*link)->left;

   struct avl_node *restrict node = *link;
   __avl_delink_node(node->right, node->parent, link);
   __avl_eject_rebalance(root, node->parent, true);

   return node;
}

/*
 * avl_eject_last(root)
 * ---------------------
 * Removes and returns the last (rightmost) node from the tree and restores AVL balance.
 * The last node has no right child, so only its left child (if any) is spliced in.
 *
 * @root:   the tree root, not NULL. tree must not be empty.
 *
 * Return: pointer to the removed node. caller is responsible for freeing it.
 */
HUZLIB_AVL_TREE_API struct avl_node *avl_eject_last(struct avl_root *restrict root)
{
   assert(root && !avl_is_empty(root));

   /*
    * find rightmost node with link
    *
    *      r
    *     / \
    *   (y) ...
    *       / \
    *     (x)  n
    */
   struct avl_node **restrict link = &root->node;
   while ((*link)->right)
      link = &(*link)->right;

   struct avl_node *restrict node = *link;
   __avl_delink_node(node->left, node->parent, link);
   __avl_eject_rebalance(root, node->parent, false);

   return node;
}


/* ------------------------------------------------ */
/* ---------------- tree traversal ---------------- */
/* ------------------------------------------------ */

HUZLIB_AVL_TREE_API struct avl_node *avl_first(const struct avl_root *restrict root)
{
   assert(root);
   struct avl_node *restrict node = root->node;
   if (node)
   {
      /*
       * find leftmost node
       *
       *      r
       *     / \
       *   ... (y)
       *   / \
       *  n  (x)
       */
      while (node->left)
         node = node->left;
   }
   return node;
}

HUZLIB_AVL_TREE_API struct avl_node *avl_last(const struct avl_root *restrict root)
{
   assert(root);
   struct avl_node *restrict node = root->node;
   if (node)
   {
      /*
       * find rightmost node
       *
       *      r
       *     / \
       *   (y) ...
       *       / \
       *     (x)  n
       */
      while (node->right)
         node = node->right;
   }
   return node;
}

HUZLIB_AVL_TREE_API struct avl_node *avl_next(const struct avl_node *restrict node)
{
   assert(node);

   if (node->right)
   {
      /*
       * find leftmost node in node->right subtree
       *
       *    n
       *     \
       *     (z)
       *     / \
       *   ... (y)
       *   / \
       *  s  (x)
       */
      node = node->right;
      while (node->left)
         node = node->left;
      return (struct avl_node *)node;
   }
   else
   {
      /*
       * walk up the tree until node is left child of parent
       *
       *     (s)
       *    /   \
       *  ...   (x)
       *    \
       *     p
       *      \
       *       n
       */
      struct avl_node *restrict parent = node->parent;
      while (parent && node == parent->right)
      {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}

HUZLIB_AVL_TREE_API struct avl_node *avl_prev(const struct avl_node *restrict node)
{
   assert(node);

   if (node->left)
   {
      /*
       * find rightmost node in node->left subtree
       *
       *        n
       *       /
       *     (z)
       *     / \
       *   (y) ...
       *       / \
       *     (x)  s
       */
      node = node->left;
      while (node->right)
         node = node->right;
      return (struct avl_node *)node;
   }
   else
   {
      /*
       * walk up the tree until node is right child of parent
       *
       *     (s)
       *    /   \
       *  (x)   ...
       *        /
       *       p
       *      /
       *     n
       */
      struct avl_node *restrict parent = node->parent;
      while (parent && node == parent->left)
      {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}

HUZLIB_AVL_TREE_API struct avl_node *avl_preorder_next(const struct avl_node *restrict node)
{
   assert(node);

   if (node->left)
   {
      /*
       * case: enter left child of 'node'
       *
       *     n
       *    / \
       *   s  (x)
       */
      return node->left;
   }
   else if (node->right)
   {
      /*
       * case: enter right child of 'node'
       *
       *     n
       *      \
       *       s
       */
      return node->right;
   }
   else
   {
      /*
       * case: walk up 2nd inorder successor
       *
       *        (x)
       *       /   \
       *    ...      s
       *      \     / \
       *       n  (a) (b)
       */
      struct avl_node *restrict parent = node->parent;
      while (parent)
      {
         if (node == parent->left && parent->right)
            return parent->right;
         node = parent;
         parent = parent->parent;
      }
      return NULL;
   }
}

HUZLIB_AVL_TREE_API struct avl_node *avl_postorder_next(const struct avl_node *restrict node)
{
   assert(node);
   const struct avl_node *parent = node->parent;

   if (parent && node == parent->left && parent->right)
   {
      /*
       * case: coming from left child, parent has right subtree
       * goto: deepest left of right subtree
       *
       *     (t)
       *     /
       *    n
       *   / \
       *  p  (s)
       */
      node = parent->right;
      while (node->left)
         node = node->left;
      return (struct avl_node *)node;
   }
   else
   {
      /*
       * case: coming from right child (or leaf)
       * goto: parent
       *
       *      (s)
       *      /
       *     n
       *    / \
       *  (x)  p
       */
      return (struct avl_node *)parent;
   }
}



/* ------------------------------------------------ */
/* -------------- sub-tree traversal -------------- */
/* ------------------------------------------------ */

HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_first(const struct avl_node *restrict subroot)
{
   if (subroot)
   {
      /*
       * find leftmost node
       *
       *      r
       *     / \
       *   ... (y)
       *   / \
       *  n  (x)
       */
      while (subroot->left)
         subroot = subroot->left;
   }
   return (struct avl_node *)subroot;
}

HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_last(const struct avl_node *restrict subroot)
{
   if (subroot)
   {
      /*
       * find rightmost node
       *
       *      r
       *     / \
       *   (y) ...
       *       / \
       *     (x)  n
       */
      while (subroot->right)
         subroot = subroot->right;
   }
   return (struct avl_node *)subroot;
}

HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_next(const struct avl_node *restrict subroot, const struct avl_node *restrict node)
{
   assert(subroot && node);

   if (node->right)
   {
      /*
       * find leftmost node in node->right subtree
       *
       *    n
       *     \
       *     (z)
       *     / \
       *   ... (y)
       *   / \
       *  s  (x)
       */
      node = node->right;
      while (node->left)
         node = node->left;
      return (struct avl_node *)node;
   }
   else
   {
      /*
       * walk up the tree until node is left child of parent
       *
       *     (s)
       *    /   \
       *  ...   (x)
       *    \
       *     p
       *      \
       *       n
       */
      struct avl_node *restrict parent = node->parent;
      while (true)
      {
         if (parent == subroot->parent)
            return NULL;

         if (node != parent->right)
            return parent;

         node = parent;
         parent = parent->parent;
      }
      unreachable();
   }
}

HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_prev(const struct avl_node *restrict subroot, const struct avl_node *restrict node)
{
   assert(subroot && node);

   if (node->left)
   {
      /*
       * find rightmost node in node->left subtree
       *
       *        n
       *       /
       *     (z)
       *     / \
       *   (y) ...
       *       / \
       *     (x)  s
       */
      node = node->left;
      while (node->right)
         node = node->right;
      return (struct avl_node *)node;
   }
   else
   {
      /*
       * walk up the tree until node is right child of parent
       *
       *     (s)
       *    /   \
       *  (x)   ...
       *        /
       *       p
       *      /
       *     n
       */
      struct avl_node *restrict parent = node->parent;
      while (true)
      {
         if (parent == subroot->parent)
            return NULL;

         if (node != parent->left)
            return parent;

         node = parent;
         parent = parent->parent;
      }
      unreachable();
   }
}

HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_preorder_next(const struct avl_node *restrict subroot, const struct avl_node *restrict node)
{
   assert(subroot && node);

   if (node->left)
   {
      /*
       * case: enter left child of 'node'
       *
       *     n
       *    / \
       *   s  (x)
       */
      return node->left;
   }
   else if (node->right)
   {
      /*
       * case: enter right child of 'node'
       *
       *     n
       *      \
       *       s
       */
      return node->right;
   }
   else
   {
      /*
       * case: walk up 2nd inorder successor
       *
       *        (x)
       *       /   \
       *    ...      s
       *      \     / \
       *       n  (a) (b)
       */
      struct avl_node *restrict parent = node->parent;
      while (parent != subroot->parent)
      {
         if (node == parent->left && parent->right)
            return parent->right;
         node = parent;
         parent = parent->parent;
      }
      return NULL;
   }
}

HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_postorder_next(const struct avl_node *restrict subroot, const struct avl_node *restrict node)
{
   assert(subroot && node);
   const struct avl_node *parent = node->parent;

   if (parent && node == parent->left && parent->right)
   {
      /*
       * case: coming from left child, parent has right subtree
       * goto: deepest left of right subtree
       *
       *     (t)
       *     /
       *    n
       *   / \
       *  p  (s)
       */
      node = parent->right;
      while (node->left)
         node = node->left;
      return (struct avl_node *)node;
   }
   else
   {
      /*
       * case: coming from right child (or leaf)
       * goto: parent, or NULL if we're done with the subtree
       *
       *      (s)
       *      /
       *     n
       *    / \
       *  (x)  p
       */
      return (node != subroot) ? (struct avl_node *)parent : NULL;
   }
}



#endif /* HUZLIB_AVL_TREE_IMPL */

#ifdef HUZLIB_AVL_TREE_TEST

#include <stdio.h>
#include <time.h>
#include "pcg_basic.h"

struct data
{
   struct avl_node avl;
   int key;
};

void dump_data(struct avl_node *node)
{
   assert(node);
   printf("%5d", avl_entry(node, struct data, avl)->key);
}

int avl_insert_data(struct avl_root *root, struct data *data)
{
   assert(root && data);
   struct avl_node **link = &root->node;
   struct avl_node *parent = NULL;
   bool isleft;

   while (*link)
   {
      parent = *link;
      int key = avl_entry(parent, struct data, avl)->key;

      if (data->key < key)
      {
         link = &parent->left;
         isleft = true;
      }
      else if (data->key > key)
      {
         link = &parent->right;
         isleft = false;
      }
      else
         return 0;
   }
   avl_link_node(&data->avl, parent, link);
   avl_insert_rebalance(root, parent, isleft);
   return 1;
}

int main()
{
   pcg32_random_t rng;
   pcg32_srandom_r(&rng, time(NULL), 88u);

   struct avl_root tree = AVL_ROOT_INIT;

   struct data data[250];
   size_t size = 0;
   for (int i = 0; i < 250; i++)
   {
      data[i].key = pcg32_boundedrand_r(&rng, 10000) - 999;
      if (!avl_insert_data(&tree, &data[i]))
         puts("Duplicate");
      else
        size++;
   }

   avl_subtree_dump(tree.node, dump_data);
   printf("sizecheck: [%d] %s\n", size, (size == avl_subtree_size(tree.node)) ? "\033[32m" "✔" "\033[0m" : "\033[31m" "✘" "\033[0m");

   for (int i = 0; i < 600; i++)
   {
      struct data *d = &data[pcg32_boundedrand_r(&rng, 250)];
      if (avl_subtree_contains(tree.node, &d->avl))
      {
         printf("Deleting: %d\n", d->key);
         avl_eject(&tree, &d->avl);
         size--;
      }
      else
      {
         puts("Not in tree");
      }
   }

   avl_subtree_dump(tree.node, dump_data);
   printf("sizecheck: [%d] %s\n", size, (size == avl_subtree_size(tree.node)) ? "\033[32m" "✔" "\033[0m" : "\033[31m" "✘" "\033[0m");

   avl_eject_first(&tree);
   size--;
   avl_subtree_dump(tree.node, dump_data);
   printf("sizecheck: [%d] %s\n", size, (size == avl_subtree_size(tree.node)) ? "\033[32m" "✔" "\033[0m" : "\033[31m" "✘" "\033[0m");

   avl_eject_last(&tree);
   size--;
   avl_subtree_dump(tree.node, dump_data);
   printf("sizecheck: [%d] %s\n", size, (size == avl_subtree_size(tree.node)) ? "\033[32m" "✔" "\033[0m" : "\033[31m" "✘" "\033[0m");

   return 0;
}

#endif /* HUZLIB_AVL_TREE_TEST */


#endif /* HUZLIB_AVL_TREE_H */
