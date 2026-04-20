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

#ifdef NDEBUG
   #define HUZLIB_AVL_TREE_API        __huzlib_pure__
   #define HUZLIB_AVL_TREE_API_INLINE __huzlib_inline__ __huzlib_pure__
#else
   #define HUZLIB_AVL_TREE_API        __huzlib_noinline__
   #define HUZLIB_AVL_TREE_API_INLINE __huzlib_noinline__
#endif


/*
 * Packed AVL node structure.
 * --------------------------
 * Saves memory by packing parent pointer and balance factor together.
 *
 * 64-bit:
 *        Original struct (24 bytes)          Packed struct (16 bytes)
 *        --------------------------          -----------------------
 *        left    : 8 bytes                   left    : 8 bytes
 *        right   : 8 bytes                   right   : 8 bytes
 *        parent  : 8 bytes                   __parent_vbalance : 8 bytes
 *        balance : 1 byte (+ 7 padding)
 *
 * 32-bit:
 *        Original struct (16 bytes)          Packed struct (12 bytes)
 *        --------------------------          -----------------------
 *        left    : 4 bytes                   left    : 4 bytes
 *        right   : 4 bytes                   right   : 4 bytes
 *        parent  : 4 bytes                   __parent_vbalance : 4 bytes
 *        balance : 1 byte (+ 3 padding)
 *
 * __parent_vbalance field:
 *   - bits 2..63 : parent pointer
 *   - bits 0..1  : vbalance = balance + 1
 *
 * Why vbalance instead of storing balance directly?
 *   - balance can be -1, which in two's complement is 0b11...11
 *   - -1 cannot be fitted in 2-bits and changes with type's bit-width
 *   - shifting by +1 maps balance to {0,1,2} which fits cleanly in 2 bits
 *   - extracting balance is just (vbalance - 1), no sign extension needed
 *
 * balance to vbalance mapping:
 *
 *    balance | vbalance
 *    --------|---------
 *       -1   |   0
 *        0   |   1
 *        1   |   2
 *
 * NOTE:
 * Pointer alignment guarantees the lower 2 bits are always 0.
 */
struct avl_node
{
   struct avl_node *left, *right;
   uintptr_t __parent_vbalance;
};

/*
 * linked AVL node.
 * -------------------
 * Extends avl_node with pointers to in-order predecessor and successor for O(1) traversal.
 *
 * End nodes store NULLs:
 *   first->prev = NULL
 *   last->next = NULL
 */
struct avl_node_linked
{
   struct avl_node node;
   struct avl_node *prev, *next;
};


struct avl_root
{
   struct avl_node *node;
};

struct avl_root_cached
{
   struct avl_root root;
   struct avl_node *first;
};

struct avl_root_linked
{
   struct avl_root root;
   struct avl_node_linked *first, *last;
};


#define AVL_ROOT_INIT         (struct avl_root)        { .node = NULL }
#define AVL_ROOT_CACHED_INIT  (struct avl_root_cached) { .root = AVL_ROOT_INIT, .first = NULL }
#define AVL_ROOT_LINKED_INIT  (struct avl_root_linked) { .root = AVL_ROOT_INIT, .first = NULL, .last = NULL }


/* --- initialize operations --- */
extern HUZLIB_AVL_TREE_API_INLINE void avl_node_init(struct avl_node *node);
extern HUZLIB_AVL_TREE_API_INLINE void avl_node_linked_init(struct avl_node_linked *node);
extern HUZLIB_AVL_TREE_API_INLINE void avl_root_init(struct avl_root *root);
extern HUZLIB_AVL_TREE_API_INLINE void avl_root_cached_init(struct avl_root_cached *root);
extern HUZLIB_AVL_TREE_API_INLINE void avl_root_linked_init(struct avl_root_linked *root);

/* --- debug/test operations --- */
#ifndef NDEBUG
extern bool avl_verify(const struct avl_node *subroot);
extern size_t avl_subtree_height(const struct avl_node *subroot);
extern size_t avl_subtree_size(const struct avl_node *subroot);
extern size_t avl_subtree_dump(const struct avl_node *subroot, void (*dump)(const struct avl_node *));
#endif /* NDEBUG */

/* --- mutate operations --- */
extern HUZLIB_AVL_TREE_API_INLINE void avl_link_node(struct avl_node *node, struct avl_node *parent, struct avl_node **link);
extern HUZLIB_AVL_TREE_API_INLINE void avl_insert_rebalance(struct avl_root *root, struct avl_node *node, bool isleft);
extern HUZLIB_AVL_TREE_API void avl_eject(struct avl_root *root, struct avl_node *node);
extern HUZLIB_AVL_TREE_API void avl_eject_cached(struct avl_root_cached *root, struct avl_node *node);
extern HUZLIB_AVL_TREE_API void avl_eject_linked(struct avl_root_linked *root, struct avl_node_linked *node);

extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_first(struct avl_root *root);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_first_cached(struct avl_root_cached *root);
extern HUZLIB_AVL_TREE_API struct avl_node_linked *avl_eject_first_linked(struct avl_root_linked *root);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_last(struct avl_root *root);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_last_cached(struct avl_root_cached *root);
extern HUZLIB_AVL_TREE_API struct avl_node_linked *avl_eject_last_linked(struct avl_root_linked *root);

#define avl_insert_rebalance_cached(root, node, isleft) typecheck_expr(struct avl_root_cached, *(root), avl_insert_rebalance(&(root)->root, node, isleft))
#define avl_insert_rebalance_linked(root, node, isleft) (      \
   typecheck(struct avl_root_linked, *(root)),                 \
   typecheck(struct avl_node_linked, *(node)),                 \
   avl_insert_rebalance(&(root)->root, &(node)->node, isleft)  \
)

/* --- query operations --- */
extern HUZLIB_AVL_TREE_API_INLINE bool avl_is_empty(const struct avl_root *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_first(const struct avl_root *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_last(const struct avl_root *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_subtree_first(const struct avl_node *subroot);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_subtree_last(const struct avl_node *subroot);

#define avl_is_empty_cached(root) typecheck_expr(struct avl_root_cached, *(root), avl_is_empty(&(root)->root))
#define avl_is_empty_linked(root) typecheck_expr(struct avl_root_linked, *(root), avl_is_empty(&(root)->root))

#define avl_first_cached(root)    typecheck_expr(struct avl_root_cached, *(root), ((root)->first))
#define avl_last_cached(root)     typecheck_expr(struct avl_root_cached, *(root), avl_last(&(root)->root))
#define avl_first_linked(root)    typecheck_expr(struct avl_root_linked, *(root), ((root)->first))
#define avl_last_linked(root)     typecheck_expr(struct avl_root_linked, *(root), ((root)->last))

/* --- tree traversal --- */
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_next(const struct avl_node *node);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_prev(const struct avl_node *node);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_preorder_next(const struct avl_node *node);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_postorder_next(const struct avl_node *node);

/* --- subtree traversal --- */
extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_next(const struct avl_node *subroot, const struct avl_node *node);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_prev(const struct avl_node *subroot, const struct avl_node *node);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_preorder_next(const struct avl_node *subroot, const struct avl_node *node);
extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_postorder_next(const struct avl_node *subroot, const struct avl_node *node);



#ifdef HUZLIB_AVL_TREE_IMPL


#endif /* HUZLIB_AVL_TREE_IMPL */



#ifdef HUZLIB_AVL_TREE_TEST

#include "pcg_basic.h"
#include "unity.h"


void setUp(void) {}
void tearDown(void) {}


int main(void)
{
   UnityBegin("test/avltree.h");
   return UnityEnd();
}


#endif /* HUZLIB_AVL_TREE_TEST */


#endif /* HUZLIB_AVL_TREE_H */
