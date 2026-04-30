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

   #define __huzlib_inline__     __attribute__((always_inline)) inline
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
   #define HUZLIB_AVL_TREE_API
   #define HUZLIB_AVL_TREE_API_INLINE __huzlib_inline__
#else
   #define HUZLIB_AVL_TREE_API        __huzlib_noinline__
   #define HUZLIB_AVL_TREE_API_INLINE inline
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
 * NOTE: in this implementation balance-factor is defined as
 *    balance = height(left) - height(right)
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
 *
 * NOTE:
 * The 'node' field MUST be at offset 0 for pointer casting to work.
 * Do not reorder fields or add fields before 'node'.
 */
struct avl_node_linked
{
   struct avl_node node; // WARN: Do not fucking change this (keep offset 0)
   struct avl_node_linked *prev, *next;
};


/*
 * AVL root - minimal tree root.
 * -----------------------------
 * Contains only the root node pointer.
 */
struct avl_root
{
   struct avl_node *node;
};

/*
 * Cached AVL root - maintains pointer to leftmost node.
 * ------------------------------------------------------
 * Extends avl_root with O(1) access to the smallest (leftmost) element.
 *
 * Why no rightmost pointer?
 *   - Most use cases only need leftmost
 *   - Maintaining rightmost adds overhead for minimal benefit
 *   - Use avl_root_linked if you need O(1) access to both ends
 *
 * NOTE:
 * The 'node' field MUST be at offset 0 for pointer casting to work.
 * Do not reorder fields or add fields before 'node'.
 */
struct avl_root_cached
{
   struct avl_root root; // WARN: Do not fucking change this (keep offset 0)
   struct avl_node *first;
};

/*
 * Linked AVL root - maintains full bidirectional traversal.
 * ---------------------------------------------------------
 * Extends avl_root with O(1) access to both smallest and largest elements,
 * plus O(1) prev/next traversal via avl_node_linked.
 *
 * NOTE:
 * The 'node' field MUST be at offset 0 for pointer casting to work.
 * Do not reorder fields or add fields before 'node'.
 */
struct avl_root_linked
{
   struct avl_root root; // WARN: Do not fucking change this (keep offset 0)
   struct avl_node_linked *first, *last;
};


#define AVL_ROOT_INIT         ((struct avl_root)        { .node = NULL })
#define AVL_ROOT_CACHED_INIT  ((struct avl_root_cached) { .root = AVL_ROOT_INIT, .first = NULL })
#define AVL_ROOT_LINKED_INIT  ((struct avl_root_linked) { .root = AVL_ROOT_INIT, .first = NULL, .last = NULL })


/*
 * AVL Augmentation Callbacks
 * --------------------------
 * Provides hooks for maintaining augmented data (e.g., subtree sizes,
 * interval max endpoints, priority sums) during tree operations.
 *
 * All callbacks are optional (may be NULL), but providing NULL for any
 * callback simply skips that augmentation step.
 *
 * The callbacks are designed to be zero-cost abstractions when inlined:
 *   - All functions are static inline in headers
 *   - const callback pointers enable constant propagation
 *   - With -O2, all indirection is eliminated
 *
 * @update:    Recompute a node's augmented value from its children.
 *             Called on a node when its children may have changed.
 *             Must read node->left and node->right, compute new value,
 *             and store it in the node's augmentation field.
 *
 *             Called during:
 *               - After linking a new leaf
 *               - During propagate walks
 *               - After rotations (via rotate callback)
 *
 * @copy:      Copy augmentation data from old node to new node.
 *             Called during ejection when a node is replaced by its
 *             successor. Must copy all augmentation data from @old to @new.
 *
 *             Called during:
 *               - Ejection of nodes with 2 children (successor replacement)
 *
 * @rotate:    Update augmentation after a rotation.
 *             Called after a rotation has been performed. Should update
 *             the two rotated nodes (typically by calling update on both).
 *
 *             Called during:
 *               - Tree rebalancing after insert/delete
 *
 * @propagate: Bulk update of a path from @start up to @stop (exclusive).
 *             Walks from @start up the parent pointers, calling update on
 *             each node, until @stop is reached. May early-terminate if
 *             augmentation values don't change.
 *
 *             If @stop is NULL, propagates all the way to the root.
 *
 *             Called during:
 *               - Early exit paths in rebalancing (height unchanged after rotation)
 *               - Path updates after successor relocation in ejection
 *
 * Typical usage for an order-statistic tree (subtree sizes):
 *
 *   static void os_update(struct avl_node *node) {
 *       struct os_node *osn = container_of(node, struct os_node, node);
 *       osn->size = 1 + size(osn->left) + size(osn->right);
 *   }
 *
 *   static void os_copy(struct avl_node *old, struct avl_node *new) {
 *       struct os_node *os_old = container_of(old, struct os_node, node);
 *       struct os_node *os_new = container_of(new, struct os_node, node);
 *       os_new->size = os_old->size;
 *   }
 *
 *   static void os_rotate(struct avl_node *old, struct avl_node *new) {
 *       os_update(old);
 *       os_update(new);
 *   }
 *
 *   static void os_propagate(struct avl_node *start, struct avl_node *stop) {
 *       struct avl_node *node = start;
 *       while (node != stop) {
 *           os_update(node);
 *           if (value unchanged) break;
 *           node = avl_parent(node);
 *       }
 *   }
 *
 *   const struct avl_augment_callbacks os_callbacks = {
 *       .update = os_update,
 *       .copy = os_copy,
 *       .rotate = os_rotate,
 *       .propagate = os_propagate,
 *   };
 */
struct avl_augment_callbacks
{
   void (*update)(struct avl_node *node);
   void (*copy)(struct avl_node *old, struct avl_node *new);
   void (*rotate)(struct avl_node *old, struct avl_node *new);
   void (*propagate)(struct avl_node *start, struct avl_node *stop);
};


/* --- initialize operations --- */
extern HUZLIB_AVL_TREE_API_INLINE void avl_node_init(struct avl_node *node);
extern HUZLIB_AVL_TREE_API_INLINE void avl_node_linked_init(struct avl_node_linked *node);

extern HUZLIB_AVL_TREE_API_INLINE void avl_root_init(struct avl_root *root);
extern HUZLIB_AVL_TREE_API_INLINE void avl_root_cached_init(struct avl_root_cached *root);
extern HUZLIB_AVL_TREE_API_INLINE void avl_root_linked_init(struct avl_root_linked *root);


/* --- getters --- */
extern HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ struct avl_node *avl_parent(const struct avl_node *node);
extern HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ int avl_balance(const struct avl_node *node);

extern HUZLIB_AVL_TREE_API_INLINE __huzlib_const__ struct avl_root *avl_cached_root(const struct avl_root_cached *root);
extern HUZLIB_AVL_TREE_API_INLINE __huzlib_const__ struct avl_root *avl_linked_root(const struct avl_root_linked *root);
extern HUZLIB_AVL_TREE_API_INLINE __huzlib_const__ struct avl_node *avl_linked_node(const struct avl_node_linked *node);


/* --- mutate operations --- */
extern HUZLIB_AVL_TREE_API_INLINE void avl_link_node(struct avl_node *node, struct avl_node *parent, struct avl_node **link);
extern HUZLIB_AVL_TREE_API_INLINE void avl_link_node_linked(struct avl_node_linked *node, struct avl_node_linked *parent, struct avl_node **link, bool isleft);

extern HUZLIB_AVL_TREE_API_INLINE void avl_insert_rebalance(struct avl_root *root, struct avl_node *node, bool isleft);
extern HUZLIB_AVL_TREE_API_INLINE void avl_insert_rebalance_augmented(struct avl_root *root, struct avl_node *node, bool isleft, const struct avl_augment_callbacks *augment);

extern HUZLIB_AVL_TREE_API void avl_eject(struct avl_root *root, struct avl_node *node);
extern HUZLIB_AVL_TREE_API void avl_eject_augmented(struct avl_root *root, struct avl_node *node, const struct avl_augment_callbacks *augment);
extern HUZLIB_AVL_TREE_API void avl_eject_cached(struct avl_root_cached *root, struct avl_node *node);
extern HUZLIB_AVL_TREE_API void avl_eject_cached_augmented(struct avl_root_cached *root, struct avl_node *node, const struct avl_augment_callbacks *augment);
extern HUZLIB_AVL_TREE_API void avl_eject_linked(struct avl_root_linked *root, struct avl_node_linked *node);
extern HUZLIB_AVL_TREE_API void avl_eject_linked_augmented(struct avl_root_linked *root, struct avl_node_linked *node, const struct avl_augment_callbacks *augment);

// extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_first(struct avl_root *root);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_first_augmented(struct avl_root *root, const struct avl_augment_callbacks *augment);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_first_cached(struct avl_root_cached *root);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_first_cached_augmented(struct avl_root_cached *root, const struct avl_augment_callbacks *augment);
// extern HUZLIB_AVL_TREE_API struct avl_node_linked *avl_eject_first_linked(struct avl_root_linked *root);
// extern HUZLIB_AVL_TREE_API struct avl_node_linked *avl_eject_first_linked_augmented(struct avl_root_linked *root, const struct avl_augment_callbacks *augment);
//
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_last(struct avl_root *root);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_last_augmented(struct avl_root *root, const struct avl_augment_callbacks *augment);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_last_cached(struct avl_root_cached *root);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_eject_last_cached_augmented(struct avl_root_cached *root, const struct avl_augment_callbacks *augment);
// extern HUZLIB_AVL_TREE_API struct avl_node_linked *avl_eject_last_linked(struct avl_root_linked *root);
// extern HUZLIB_AVL_TREE_API struct avl_node_linked *avl_eject_last_linked_augmented(struct avl_root_linked *root, const struct avl_augment_callbacks *augment);
//

/* --- debug/test operations --- */
#ifndef NDEBUG
extern size_t avl_subtree_height(const struct avl_node *subroot);
extern size_t avl_subtree_size(const struct avl_node *subroot);
extern size_t avl_subtree_dump(const struct avl_node *subroot, void (*dump_node)(const struct avl_node *), void (*dump_str)(const char *));
extern bool avl_verify(const struct avl_node *node);
#endif /* NDEBUG */


// /* --- query operations --- */
// extern HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ bool avl_is_empty(const struct avl_root *root);
// extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_first(const struct avl_root *root);
// extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_last(const struct avl_root *root);
// extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_subtree_first(const struct avl_node *subroot);
// extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_subtree_last(const struct avl_node *subroot);
//
// #define avl_is_empty_cached(root) typecheck_expr(struct avl_root_cached, *(root), avl_is_empty(&(root)->root))
// #define avl_is_empty_linked(root) typecheck_expr(struct avl_root_linked, *(root), avl_is_empty(&(root)->root))
//
// #define avl_first_cached(root)    typecheck_expr(struct avl_root_cached, *(root), ((root)->first))
// #define avl_last_cached(root)     typecheck_expr(struct avl_root_cached, *(root), avl_last(&(root)->root))
// #define avl_first_linked(root)    typecheck_expr(struct avl_root_linked, *(root), ((root)->first))
// #define avl_last_linked(root)     typecheck_expr(struct avl_root_linked, *(root), ((root)->last))
//
// /* --- tree traversal --- */
// extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_next(const struct avl_node *node);
// extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_prev(const struct avl_node *node);
// extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_preorder_next(const struct avl_node *node);
// extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_postorder_next(const struct avl_node *node);
//
// /* --- subtree traversal --- */
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_next(const struct avl_node *subroot, const struct avl_node *node);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_prev(const struct avl_node *subroot, const struct avl_node *node);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_preorder_next(const struct avl_node *subroot, const struct avl_node *node);
// extern HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_postorder_next(const struct avl_node *subroot, const struct avl_node *node);




#ifdef HUZLIB_AVL_TREE_IMPL

#define AVL_BALANCE_MASK  ((uintptr_t)0x03)

/* ------------------------------------------------- */
/* --------------- helper functions  --------------- */
/* ------------------------------------------------- */

/*
 * __avl_parent(parent_vbalance)
 * -----------------------------
 * Returns the parent pointer stored in the packed field.
 */
static HUZLIB_AVL_TREE_API_INLINE __huzlib_const__ struct avl_node *__avl_parent(const uintptr_t parent_vbalance)
{
   return (struct avl_node *)(parent_vbalance & ~AVL_BALANCE_MASK);
}

/*
 * __avl_balance(parent_vbalance)
 * ------------------------------
 * Returns the balance factor of the node.
 */
static HUZLIB_AVL_TREE_API_INLINE __huzlib_const__ int __avl_balance(const uintptr_t parent_vbalance)
{
   return (int)(parent_vbalance & AVL_BALANCE_MASK) - 1;
}

/*
 * __avl_set_parent_balance(node, parent, balance)
 * ------------------------------------------------
 * Sets both the parent pointer and balance factor of a node.
 *
 * @node:    the AVL node to update, not NULL
 * @parent:  the new parent pointer, may be NULL
 * @balance: the new balance factor (-1, 0, or +1)
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_set_parent_balance(struct avl_node *restrict node, const struct avl_node *restrict parent, const int balance)
{
   assert(node && (balance >= -1) && (balance <= 1));
   node->__parent_vbalance = ((uintptr_t)parent & ~AVL_BALANCE_MASK) | (balance + 1);
}

/*
 * __avl_set_parent(node, parent)
 * ------------------------------
 * Sets only the parent pointer. Balance factor is preserved.
 *
 * @node:   the AVL node to update, not NULL
 * @parent: the new parent pointer, may be NULL
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_set_parent(struct avl_node *restrict node, const struct avl_node *restrict parent)
{
   assert(node);
   node->__parent_vbalance = ((uintptr_t)parent) | (node->__parent_vbalance & AVL_BALANCE_MASK);
}

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
static HUZLIB_AVL_TREE_API_INLINE struct avl_node **__avl_parent_ptr(struct avl_root *restrict root, struct avl_node *restrict node, struct avl_node *restrict parent, bool *restrict isleft)
{
   assert(root && node && isleft && (avl_parent(node) == parent));

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
static HUZLIB_AVL_TREE_API_INLINE void __avl_delink_node(struct avl_node *restrict child, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   assert(link && (avl_parent(*link)) == parent && (((*link)->left == child) || ((*link)->right == child)));
   *link = child;
   if (child)
      __avl_set_parent(child, parent);
}

/*
 * __avl_replace_node(old, new, link, copy)
 * ----------------------------------------
 * Attaches 'new' to where 'old' was via 'link', the parent's pointer
 *
 *     |                              |
 *     o                              n
 *    / \                            / \
 *  (u) (v)     n         ==>      (x) (y)      o
 *             / \                             / \
 *           (x) (y)                         (u) (v)
 *
 * @old:  the node to be replaced, not NULL
 * @new:  node to replace with, not NULL
 * @link: parent's internal pointer to linking node, not NULL
 * @copy: augment callback to copy augmented data
 *
 * NOTE:
 * Will replace entire subtree on 'old' with subtree of 'new'
 * Therefore, children 'u' & 'v' (if exists) are left dangling.
 * Relinking is the caller's responsibility.
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_replace_node(struct avl_node *restrict old, struct avl_node *restrict new, struct avl_node **restrict link, void (*augment_copy)(struct avl_node *, struct avl_node *))
{
   assert(old && new && link && (*link == old) && augment_copy);
   *link = new;
   new->__parent_vbalance = old->__parent_vbalance;
   augment_copy(old, new);
}


/*
 * __avl_add_linked(new, prev, next)
 * ---------------------------------
 * Inserts 'new' between 'prev' and 'next' in a doubly-linked list.
 *
 *      prev         next                 prev         new         next
 *       o --------- o                     o --------- o --------- o
 *
 * @new:   node to insert, not NULL
 * @prev:  predecessor node, may be NULL (insert at head)
 * @next:  successor node, may be NULL (insert at tail)
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_add_linked(struct avl_node_linked *restrict new, struct avl_node_linked *restrict prev, struct avl_node_linked *restrict next)
{
   assert(new);

   if (__huzlib_likely__(prev))
      prev->next = new;

   if (__huzlib_likely__(next))
      next->prev = new;

   new->prev = prev;
   new->next = next;
}

/*
 * __avl_rm_linked(prev, next)
 * ---------------------------
 * Removes the node between 'prev' and 'next' by linking them directly.
 *
 *      prev         node         next                 prev         next
 *       o --------- o --------- o         ==>          o --------- o
 *
 * @prev:  predecessor of the node being removed, may be NULL
 * @next:  successor of the node being removed, may be NULL
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_rm_linked(struct avl_node_linked *restrict prev, struct avl_node_linked *restrict next)
{
   if (__huzlib_likely__(prev))
      prev->next = next;

   if (__huzlib_likely__(next))
      next->prev = prev;
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
 * Does not update '__parent_vbalance' fields of 'node' and 'child'.
 * Caller must update them after rotation.
 *   eg: __avl_set_parent_balance(node, child, x);
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_rotate_left(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node **restrict link)
{
   assert(node && child && link && (*link == node) && (node->right == child));

   if (child->left)
      __avl_set_parent(child->left, node);

   node->right = child->left;
   child->left = node;

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
 * Does not update '__parent_vbalance' fields of 'node' and 'child'.
 * Caller must update them after rotation.
 *    eg: __avl_set_parent_balance(node, child, x);
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_rotate_right(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node **restrict link)
{
   assert(node && child && link && (*link == node) && (node->left == child));

   if (child->right)
      __avl_set_parent(child->right, node);

   node->left = child->right;
   child->right = node;

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
 * Does not update '__parent_vbalance' fields of 'node', 'parent', and 'child'.
 * Caller must update them after rotation.
 *    eg: __avl_set_parent_balance(node, child, x);
 *        __avl_set_parent_balance(parent, child, y);
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_rotate_left_right(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   assert(node && child && parent && link && (*link == parent) && (node->right == child) && (parent->left == node));

   if (child->left) /* (y) */
      __avl_set_parent(child->left, node);

   if (child->right) /* (z) */
      __avl_set_parent(child->right, parent);

   node->right = child->left;
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
 * Does not update '__parent_vbalance' fields of 'node', 'parent', and 'child'.
 * Caller must update them after rotation.
 *    eg: __avl_set_parent_balance(node, child, x);
 *        __avl_set_parent_balance(parent, child, y);
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_rotate_right_left(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   assert(node && child && parent && link && (*link == parent) && (node->left == child) && (parent->right == node));

   if (child->right) /* (y) */
      __avl_set_parent(child->right, node);

   if (child->left) /* (z) */
      __avl_set_parent(child->left, parent);

   node->left = child->right;
   parent->right = child->left;

   child->right = node;
   child->left = parent;

   *link = child;
}


/*
 * __avl_insert_rebalance(root, node, isleft, augment)
 * ---------------------------------------------------
 * Restores AVL balance after a node insertion by walking up the tree
 * from 'node' and fixing balance factors and rotations as needed.
 *
 * @root:   the tree root, not NULL
 * @node:   parent of the inserted node, may be NULL if tree was empty
 * @isleft: true if the insertion was in node's left subtree
 * @augment: augment callback functions
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
static HUZLIB_AVL_TREE_API_INLINE void __avl_insert_rebalance(struct avl_root *restrict root, struct avl_node *restrict node, bool isleft, const struct avl_augment_callbacks *restrict augment)
{
   assert(root && augment);
   struct avl_node *restrict parent, *restrict child, *restrict tmp;
   struct avl_node **restrict link;
   int node_balance, child_balance, tmp_balance;

   while (__huzlib_likely__(node))
   {
      if (isleft)
         node_balance = avl_balance(node) + 1;
      else
         node_balance = avl_balance(node) - 1;

      parent = avl_parent(node);
      link = __avl_parent_ptr(root, node, parent, &isleft);

      switch (node_balance)
      {
         case 0:
            __avl_set_parent_balance(node, parent, 0);
            augment->propagate(node, NULL);
            return; // height unchanged

         case 1:
         case -1:
            __avl_set_parent_balance(node, parent, node_balance);
            augment->update(node);
            node = parent;
            continue; // height increased

         case 2:
            child = node->left;
            child_balance = avl_balance(child);

            if (child_balance >= 0)
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
               augment->rotate(node, child);

               if (child_balance == 0)
               {
                  __avl_set_parent_balance(node, child, 1);
                  __avl_set_parent_balance(child, parent, -1);

                  node = parent;
                  continue; // height increased
               }
               else /* (child_balance == 1) */
               {
                  __avl_set_parent_balance(node, child, 0);
                  __avl_set_parent_balance(child, parent, 0);

                  augment->propagate(parent, NULL);
                  return; // height unchanged
               }
            }
            else /* (child_balance == -1) */
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
               tmp_balance = avl_balance(tmp);

               __avl_rotate_left_right(child, tmp, node, link);
               augment->rotate(child, tmp);
               augment->rotate(node, tmp);

               __avl_set_parent_balance(node, tmp, (tmp_balance == 1) ? -1 : 0);
               __avl_set_parent_balance(child, tmp, (tmp_balance == -1) ? 1 : 0);
               __avl_set_parent_balance(tmp, parent, 0);

               augment->propagate(parent, NULL);
               return; // height unchanged
            }
            unreachable();

         case -2:
            child = node->right;
            child_balance = avl_balance(child);

            if (child_balance <= 0)
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
               augment->rotate(node, child);

               if (child_balance == 0)
               {
                  __avl_set_parent_balance(node, child, -1);
                  __avl_set_parent_balance(child, parent, 1);

                  node = parent;
                  continue; // height increased
               }
               else /* (child_balance == -1) */
               {
                  __avl_set_parent_balance(node, child, 0);
                  __avl_set_parent_balance(child, parent, 0);

                  augment->propagate(parent, NULL);
                  return; // height unchanged
               }
            }
            else /* (child_balance == 1) */
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
               tmp_balance = avl_balance(tmp);

               __avl_rotate_right_left(child, tmp, node, link);
               augment->rotate(child, tmp);
               augment->rotate(node, tmp);

               __avl_set_parent_balance(node, tmp, (tmp_balance == -1) ? 1 : 0);
               __avl_set_parent_balance(child, tmp, (tmp_balance == 1) ? -1 : 0);
               __avl_set_parent_balance(tmp, parent, 0);

               augment->propagate(parent, NULL);
               return; // height unchanged
            }
            unreachable();
      }
      unreachable();
   }
}

/*
 * __avl_eject_rebalance(root, node, isleft)
 * ------------------------------------------
 * Restores AVL balance after a node deletion by walking up the tree
 * from 'node' and fixing balance factors and doing rotations as needed.
 *
 * @root:    the tree root, not NULL
 * @node:    parent of the ejected node, may be NULL 
 * @isleft:  true if the deletion was in node's left child
 * @augment: augment callback functions
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
 * is INVERTED because insertion increases height while deletion decreases height.
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_eject_rebalance(struct avl_root *restrict root, struct avl_node *restrict node, bool isleft, const struct avl_augment_callbacks *restrict augment)
{
   assert(root && augment);
   struct avl_node *restrict parent, *restrict child, *restrict tmp;
   struct avl_node **restrict link;
   int node_balance, child_balance, tmp_balance;

   while (__huzlib_likely__(node))
   {
      if (isleft)
         node_balance = avl_balance(node) - 1;
      else
         node_balance = avl_balance(node) + 1;

      parent = avl_parent(node);
      link = __avl_parent_ptr(root, node, parent, &isleft);

      switch (node_balance)
      {
         case 1:
         case -1:
            __avl_set_parent_balance(node, parent, node_balance);
            augment->propagate(node, NULL);
            return; // height unchanged

         case 0:
            __avl_set_parent_balance(node, parent, 0);
            augment->update(node);
            node = parent;
            continue; // height decreased

         case 2:
            child = node->left;
            child_balance = avl_balance(child);

            if (child_balance >= 0)
            {
               /* __huzlib_const__
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
               augment->rotate(node, child);

               if (child_balance == 0)
               {
                  __avl_set_parent_balance(node, child, 1);
                  __avl_set_parent_balance(child, parent, -1);

                  augment->propagate(parent, NULL);
                  return; // height unchanged
               }
               else /* (child_balance == 1) */
               {
                  __avl_set_parent_balance(node, child, 0);
                  __avl_set_parent_balance(child, parent, 0);

                  node = parent;
                  continue; // height decreased
               }
            }
            else /* (child_balance == -1) */
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
               tmp_balance = avl_balance(tmp);

               __avl_rotate_left_right(child, tmp, node, link);
               augment->rotate(child, tmp);
               augment->rotate(node, tmp);

               __avl_set_parent_balance(node, tmp, (tmp_balance == 1) ? -1 : 0);
               __avl_set_parent_balance(child, tmp, (tmp_balance == -1) ? 1 : 0);
               __avl_set_parent_balance(tmp, parent, 0);

               node = parent;
               continue; // height decreased
            }
            unreachable();

         case -2:
            child = node->right;
            child_balance = avl_balance(child);

            if (child_balance <= 0)
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
               augment->rotate(node, child);

               if (child_balance == 0)
               {
                  __avl_set_parent_balance(node, child, -1);
                  __avl_set_parent_balance(child, parent, 1);

                  augment->propagate(parent, NULL);
                  return; // height unchanged
               }
               else /* (child_balance == -1) */
               {
                  __avl_set_parent_balance(node, child, 0);
                  __avl_set_parent_balance(child, parent, 0);

                  node = parent;
                  continue; // height decreased
               }
            }
            else /* (child_balance == 1) */
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
               tmp_balance = avl_balance(tmp);

               __avl_rotate_right_left(child, tmp, node, link);
               augment->rotate(child, tmp);
               augment->rotate(node, tmp);

               __avl_set_parent_balance(node, tmp, (tmp_balance == -1) ? 1 : 0);
               __avl_set_parent_balance(child, tmp, (tmp_balance == 1) ? -1 : 0);
               __avl_set_parent_balance(tmp, parent, 0);

               node = parent;
               continue; // height decreased
            }
            unreachable();
      }
      unreachable();
   }
}

/*
 * __avl_eject(root, node, augment)
 * ----------------------
 * Removes 'node' from the tree and restores AVL balance.
 *
 * @root: the tree root, not NULL
 * @node: the node to remove, not NULL
 * @augment: augment callback functions
 *
 * Return: in-order successor when 'node' might be 'first' node in tree
 *   2 children:      deleted node (cannot be 'first')
 *   1 (left) child:  deleted node (cannot be 'first')
 *   1 (right) child: node->right
 *   0 children:      parent (if 'node' is 'first', it is always the left-child of parent)
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
static HUZLIB_AVL_TREE_API_INLINE struct avl_node *__avl_eject(struct avl_root *restrict root, struct avl_node *restrict node, const struct avl_augment_callbacks *restrict augment)
{
   assert(root && node && augment);

   bool isleft;
   struct avl_node *restrict parent = avl_parent(node);
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

      struct avl_node *restrict succ = node->right;
      if (succ->left)
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

         do {
            parent = succ;
            succ = succ->left;
         } while (succ->left);

         isleft = true;
         __avl_delink_node(succ->right, parent, &parent->left);

         __avl_set_parent(node->right, succ);
         succ->right = node->right;
      }
      else
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

         parent = succ;
         isleft = false;
      }
      __avl_set_parent(node->left, succ);
      succ->left = node->left;

      __avl_replace_node(node, succ, link, augment->copy);
      // no need to update node to return in-order successor since
      // 'node' cannot be 'first' node in tree (due to node->left != NULL)
   }
   else if (node->left)
   {
      /*
       * case: 'node' has only left child
       *
       *       |                   |
       *       n       ==>         s
       *      /
       *     s
       *
       * NOTE: due to AVL properties, 's' must be a leaf node
       */

      __avl_delink_node(node->left, parent, link);
      // no need to update node to return in-order successor since
      // 'node' cannot be 'first' node in tree (due to node->left != NULL)
   }
   else if (node->right)
   {
      /*
       * case: 'node' has only right child
       *
       *   |                       |
       *   n           ==>         s
       *    \
       *     s
       *
       * NOTE: due to AVL properties, 's' must be a leaf node
       */

      __avl_delink_node(node->right, parent, link);
      node = node->right; // return node->right in case node is first-node
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
      node = parent;
   }
   __avl_eject_rebalance(root, parent, isleft, augment);
   return node; // return parent in case node is first-node
}


/*
 * augmentation Dummies
 * --------------------
 * Placeholder functions for when avl_node is NOT augmented.
 * These functions do nothing and are optimized away by the compiler.
 */
static HUZLIB_AVL_TREE_API_INLINE void __avl_dummy_augment_update(struct avl_node *)                        { (void)0; }
static HUZLIB_AVL_TREE_API_INLINE void __avl_dummy_augment_copy(struct avl_node *, struct avl_node *)       { (void)0; }
static HUZLIB_AVL_TREE_API_INLINE void __avl_dummy_augment_rotate(struct avl_node *, struct avl_node *)     { (void)0; }
static HUZLIB_AVL_TREE_API_INLINE void __avl_dummy_augment_propagate(struct avl_node *, struct avl_node *)  { (void)0; }

const struct avl_augment_callbacks __avl_dummy_augment = {
   .update    = __avl_dummy_augment_update,
   .copy      = __avl_dummy_augment_copy,
   .rotate    = __avl_dummy_augment_rotate,
   .propagate = __avl_dummy_augment_propagate,
};



/* ------------------------------------------------- */
/* ------------- initialize operations ------------- */
/* ------------------------------------------------- */

HUZLIB_AVL_TREE_API_INLINE void avl_node_init(struct avl_node *restrict node)
{
   assert(node);
   node->left = NULL;
   node->right = NULL;
   node->__parent_vbalance = 0;
}

HUZLIB_AVL_TREE_API_INLINE void avl_node_linked_init(struct avl_node_linked *restrict node)
{
   assert(node);
   avl_node_init(&node->node);
   node->prev = NULL;
   node->next = NULL;
}

HUZLIB_AVL_TREE_API_INLINE void avl_root_init(struct avl_root *restrict root)
{
   assert(root);
   *root = AVL_ROOT_INIT;
}

HUZLIB_AVL_TREE_API_INLINE void avl_root_cached_init(struct avl_root_cached *restrict root)
{
   assert(root);
   *root = AVL_ROOT_CACHED_INIT;
}

HUZLIB_AVL_TREE_API_INLINE void avl_root_linked_init(struct avl_root_linked *restrict root)
{
   assert(root);
   *root = AVL_ROOT_LINKED_INIT;
}



/* ------------------------------------------------- */
/* --------------- getter functions  --------------- */
/* ------------------------------------------------- */

HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ struct avl_node *avl_parent(const struct avl_node *node)
{
   assert(node);
   return __avl_parent(node->__parent_vbalance);
}

HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ int avl_balance(const struct avl_node *node)
{
   assert(node);
   return __avl_balance(node->__parent_vbalance);
}

HUZLIB_AVL_TREE_API_INLINE __huzlib_const__ struct avl_root *avl_cached_root(const struct avl_root_cached *root)
{
   assert(root);
   return (struct avl_root *)&root->root;
}

HUZLIB_AVL_TREE_API_INLINE __huzlib_const__ struct avl_root *avl_linked_root(const struct avl_root_linked *root)
{
   assert(root);
   return (struct avl_root *)&root->root;
}

HUZLIB_AVL_TREE_API_INLINE __huzlib_const__ struct avl_node *avl_linked_node(const struct avl_node_linked *node)
{
   assert(node);
   return (struct avl_node *)&node->node;
}



/* -------------------------------------------------- */
/* --------------- mutate operations  --------------- */
/* -------------------------------------------------- */

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
HUZLIB_AVL_TREE_API_INLINE void avl_link_node(struct avl_node *restrict node, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   assert(node && link);
   __avl_set_parent_balance(node, parent, 0);
   node->left = NULL;
   node->right = NULL;
   *link = node;
}

/*
 * avl_link_node_linked(node, parent, link, isleft)
 * ------------------------------------------------
 * Attaches 'node' to 'parent' via 'link', the parent's pointer,
 * sets 'node's children to NULL, and inserts 'node' into the
 * doubly-linked in-order list.
 *
 * Similar to 'avl_link_node' with added logic of connecting
 * the linked list pointers.
 *
 * @node:   the node to attach, not NULL
 * @parent: to be parent of 'node', may be NULL (root insertion)
 * @link:   parent's internal pointer to linking node, not NULL
 * @isleft: true if the insertion is in parent's left child
 */
HUZLIB_AVL_TREE_API_INLINE void avl_link_node_linked(struct avl_node_linked *restrict node, struct avl_node_linked *restrict parent, struct avl_node **restrict link, bool isleft)
{
   assert(node && ((isleft) ? parent != NULL : true));
   avl_link_node(&node->node, (parent) ? &parent->node : NULL, link);

   if (isleft)
      __avl_add_linked(node, parent->prev, parent);
   else if (parent) /* right-child insertion */
      __avl_add_linked(node, parent, parent->next);
}

/*
 * avl_insert_rebalance(root, node, isleft, [augment])
 * ---------------------------------------------------
 * walk-up AVL-tree restoring AVL balance after a node insertion
 * from parent of newly inserted node 
 *
 * @root:   the tree root, not NULL
 * @node:   parent of the inserted node, may be NULL if tree was empty
 * @isleft: true if the insertion was in node's left subtree
 * @augment: augment callback functions
 */
HUZLIB_AVL_TREE_API_INLINE void avl_insert_rebalance(struct avl_root *restrict root, struct avl_node *restrict node, bool isleft)
{
   __avl_insert_rebalance(root, node, isleft, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API_INLINE void avl_insert_rebalance_augmented(struct avl_root *restrict root, struct avl_node *restrict node, bool isleft, const struct avl_augment_callbacks *restrict augment)
{
   __avl_insert_rebalance(root, node, isleft, augment);
}


/*
 * avl_eject(root, node, [augment])
 * --------------------------------
 * Removes 'node' from the tree and restores AVL balance.
 *
 * @root:    the tree root, not NULL
 * @node:    the node to remove, not NULL
 * @augment: augment callback functions
 */
HUZLIB_AVL_TREE_API void avl_eject(struct avl_root *restrict root, struct avl_node *restrict node)
{
   avl_eject_augmented(root, node, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API void avl_eject_augmented(struct avl_root *restrict root, struct avl_node *restrict node, const struct avl_augment_callbacks *restrict augment)
{
   (void)__avl_eject(root, node, augment);
}

/*
 * avl_eject_cached(root, node, [augment])
 * ---------------------------------------
 * Removes 'node' from the cached AVL tree and restores AVL balance.
 * Maintains root->first to always point to the minimum node.
 *
 * @root:    the cached tree root, not NULL
 * @node:    the node to remove, not NULL
 * @augment: augment callback functions
 */
HUZLIB_AVL_TREE_API void avl_eject_cached(struct avl_root_cached *restrict root, struct avl_node *restrict node)
{
   avl_eject_cached_augmented(root, node, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API void avl_eject_cached_augmented(struct avl_root_cached *restrict root, struct avl_node *restrict node, const struct avl_augment_callbacks *restrict augment)
{
   assert(root);
   struct avl_node *tmp = __avl_eject(avl_cached_root(root), node, augment);
   if (node == root->first)
      root->first = tmp;
}

/*
 * avl_eject_linked(root, node, [augment])
 * -----------------------------
 * Removes 'node' from the linked AVL tree rooted at 'root' and restores
 * AVL balance. Maintains root->first and root->last, and unlinks 'node'
 * from the doubly-linked in-order list.
 *
 * similar to 'avl_eject' with added root->first / root->last maintainance,
 * and O(1) traversal on finding successor
 *
 * @root:    the cached tree root, not NULL
 * @node:    the node to remove, not NULL
 * @augment: augment callback functions
 */
HUZLIB_AVL_TREE_API void avl_eject_linked(struct avl_root_linked *restrict root, struct avl_node_linked *node)
{
   avl_eject_linked_augmented(root, node, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API void avl_eject_linked_augmented(struct avl_root_linked *restrict root, struct avl_node_linked *node, const struct avl_augment_callbacks *restrict augment)
{
   assert(root && node);

   bool isleft;
   struct avl_node *restrict n = avl_linked_node(node), *restrict parent = avl_parent(n);
   struct avl_node **restrict link = __avl_parent_ptr(avl_linked_root(root), n, parent, &isleft);

   if (n->left && n->right)
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

      struct avl_node *restrict succ = &node->next->node;
      if (succ == n->right)
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

         parent = avl_parent(succ);
         isleft = true;

         __avl_delink_node(succ->right, parent, &parent->left);

         __avl_set_parent(n->right, succ);
         succ->right = n->right;
      }
      __avl_set_parent(n->left, succ);
      succ->left = n->left;

      __avl_replace_node(n, succ, link, augment->copy);
   }
   else if (n->left)
   {
      /*
       * case: 'node' has only left child
       *
       *       |                   |
       *       n       ==>         s
       *      /
       *     s
       *
       * NOTE: due to AVL properties, 's' must be a leaf node
       */

      __avl_delink_node(n->left, parent, link);

      if (node == root->last)
         root->last = node->prev;
   }
   else if (n->right)
   {
      /*
       * case: 'node' has only right child
       *
       *   |                       |
       *   n           ==>         s
       *    \
       *     s
       *
       * NOTE: due to AVL properties, 's' must be a leaf node
       */

      __avl_delink_node(n->right, parent, link);

      if (node == root->first)
         root->first = node->next;
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

      if (node == root->first)
         root->first = node->next;

      if (node == root->last)
         root->last = node->prev;
   }

   __avl_rm_linked(node->prev, node->next);
   __avl_eject_rebalance(avl_linked_root(root), parent, isleft, augment);
}


/* -------------------------------------------------- */
/* ------------- debug/test operations  ------------- */
/* -------------------------------------------------- */

#ifndef NDEBUG

/*
 * avl_subtree_height(subroot)
 * ---------------------------
 * Computes the height of the subtree rooted at 'subroot' using balance-factor
 *
 * @subroot: root of the subtree (may be NULL)
 *
 * Return: height of the subtree (0 for empty tree)
 * Complexity: O(height) worst-case, O(log n) for balanced tree
 */
size_t avl_subtree_height(const struct avl_node *subroot)
{
   size_t height = 0;
   while (subroot)
   {
      height++;

      if (avl_balance(subroot) >= 0)
         subroot = subroot->left;
      else
         subroot = subroot->right;
   }

   return height;
}

/*
 * avl_subtree_size(subroot)
 * -------------------------
 * Counts the number of nodes in the subtree rooted at 'subroot'
 * using a non-recursive pre-order traversal.
 *
 * @subroot: root of the subtree (may be NULL)
 *
 * Return: number of nodes in the subtree (0 for empty tree)
 * Complexity: O(n) where n is the number of nodes in the subtree
 */
size_t avl_subtree_size(const struct avl_node *subroot)
{
   if (!subroot)
      return 0;

   size_t size = 0;
   const struct avl_node *node = subroot;

   while (node != avl_parent(subroot)) // pre-order traversal
   {
      size++;

      if (node->left)
      {
         node = node->left;
      }
      else if (node->right)
      {
         node = node->right;
      }
      else
      {
         while (node != avl_parent(subroot))
         {
            const struct avl_node *parent = avl_parent(node);
            if (node != subroot && parent->left == node && parent->right)
            {
               node = parent->right;
               break;
            }
            node = parent;
         }
      }
   }
   return size;
}

/*
 * avl_subtree_dump(subroot, dump_node, dump_str)
 * ----------------------------------------------
 * print subtree structure using 'dump' callbacks
 *
 * @subroot:    root of subtree to dump
 * @dump_node:  callback to dump node-specific data (gets node pointer)
 * @dump_str:   callback to output raw strings
 *
 * Return: number of nodes dumped.
 *
 * Example usage:
 *
 *   void my_node_dump(const struct avl_node *n) {
 *       struct my_node *m = container_of(n, struct my_node, node);
 *       char buf[32];
 *       itoa(m->key, buf, 10);
 *       dump_str(buf);
 *   }
 * 
 *   void my_str_dump(const char *s) {
 *       while (*s)
 *          serial_putchar(*s++);
 *   }
 *
 *   avl_subtree_dump(root, my_node_dump, my_str_dump);
 */
size_t avl_subtree_dump(const struct avl_node *subroot, void (*dump_node)(const struct avl_node *), void (*dump_str)(const char *))
{
   assert(dump_node && dump_str);

   if (!subroot)
   {
      dump_str("🌲 [empty tree]\n");
      return 0;
   }

   size_t size = 0, height = 1;
   const struct avl_node *node = subroot;

   while (node != avl_parent(subroot)) // pre-order traversal
   {
      /* --- dump node --- */
      for (size_t i = 1; i < height; i++)
         dump_str("    ");

      if (node == subroot)
         dump_str("🌲 ");
      else if (node == avl_parent(node)->left)
         dump_str("├── ");
      else
         dump_str("└── ");

      dump_node(node);

      switch (avl_balance(node))
      {
         case -1: dump_str("-1"); break;
         case  0: dump_str("0");  break;
         case  1: dump_str("1");  break;
      }

      if (node->left || node->right)
         dump_str("🍂");

      if (avl_verify(node))
         dump_str("\033[32m" "✔" "\033[0m");
      else
         dump_str("\033[31m" "✘" "\033[0m");

      size++;
 
      /* --- next node --- */
      if (node->left)
      {
         node = node->left;
         height++;
      }
      else if (node->right)
      {
         node = node->right;
         height++;
      }
      else
      {
         while (node != avl_parent(subroot))
         {
            const struct avl_node *parent = avl_parent(node);
            if (node != subroot && parent->left == node && parent->right)
            {
               node = parent->right;
               break;
            }
            node = parent;
            height--;
         }
      }
   }
   return size;
}

/*
 * avl_verify(node)
 * ----------------
 * Validates that a node and its subtree satisfy all AVL invariants.
 *
 * @node: node to verify (may be NULL? caller ensures non-NULL)
 *
 * Return: true if the node and its entire subtree are valid, false otherwise
 *
 * Checks performed:
 *   - Child linkages are correct (parent pointers match)
 *   - Parent linkage is correct (parent points back to this node)
 *   - Balance factor matches actual height difference of children
 *   - Balance factor is within [-1, 1] range (AVL property)
 */
bool avl_verify(const struct avl_node *node)
{
   assert(node);

   // verify child linkages
   bool left_valid = (node->left) ? (avl_parent(node->left) == node) : true;
   bool right_valid = (node->right) ? (avl_parent(node->right) == node) : true;

   // verify parent linkage
   struct avl_node *parent = avl_parent(node);
   bool parent_valid = (parent)
      ? (parent->left == node || parent->right == node)
      : true;

   // verify balance
   int balance = avl_subtree_height(node->left) - avl_subtree_height(node->right);
   bool balance_valid = (avl_balance(node) == balance);
   bool isbalanced = (balance >= -1) && (balance <= 1);

   return left_valid 
       && right_valid 
       && parent_valid 
       && balance_valid 
       && isbalanced;
}

#endif /* NDEBUG */



#undef AVL_BALANCE_MASK

#endif /* HUZLIB_AVL_TREE_IMPL */




#ifdef HUZLIB_AVL_TREE_TEST

#include "pcg_basic.h"
#include "unity.h"

/*
 * avl_setup_test_node(node, left, right, parent, balance)
 * -------------------------------------------------------
 * setups 'node' for testing purposes with children 'left' & 'right', parent 'parent' and balance 'balance'
 *
 * @node:    test node
 * @left:    left child tobe set
 * @right:   right child tobe set
 * @parent:  parent tobe set
 * @balance: balance tobe set
 */
static void avl_setup_test_node(struct avl_node *node, struct avl_node *left, struct avl_node *right, struct avl_node *parent, int balance)
{
   assert(node && balance >= -1 && balance <= 1);
   node->left = left;
   node->right = right;
   __avl_set_parent_balance(node, parent, balance);
}

/*
 * avl_verify_recursive(node)
 * --------------------------
 * verify 'node' and it's subtree are a correct BST-nodes and AVL-nodes
 *
 * @node: cursor node
 *
 * Return:
 *    valid node:   height of subtree starting from 'node' (h >= 0)
 *    invalid node: -1
 */
static long long avl_verify_recursive(struct avl_node *node)
{
   if (!node)
      return 0;

   long long left_height = avl_verify_recursive(node->left);
   long long right_height = avl_verify_recursive(node->right);

   // verify child linkages
   if (left_height < 0 || right_height < 0)
      return -1;

   // verify parent linkage
   struct avl_node *parent = avl_parent(node);
   if (parent && !(parent->left == node || parent->right == node))
      return -1;

   // verify balance
   int balance = left_height - right_height;
   if ((balance > 1) || (balance < -1) || (avl_balance(node) != balance))
      return -1;

   return 1 + ((left_height > right_height) ? left_height : right_height);
}

void setUp(void) {}
void tearDown(void) {}


static void test_avl_eject_rebalance_LL(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y, z, k;

   /*
    * create structure mimicking LL case with child_balance == 0
    * plus arbitary nodes (x, y, z, k, w) attached to test for corruption
    *
    *         n                          c
    *        / \                       /   \
    *       c   w                    g       n
    *      / \   \       ==>        / \     / \
    *     g   z   ☠️               x   y   z   w
    *    / \   \                           \
    *   x   y  (k)                         (k)
    */

   /*
    * c_balances[i] drives which LL sub-case we hit:
    *   0  -> child_balance == 0:  n=1, c=-1
    *   1  -> child_balance == 1:  n=0, c=0
    */
   int c_balances[2] = {  0, 1 };
   int n_expected[2] = {  1, 0 };
   int c_expected[2] = { -1, 0 };

   int w_balances[2] = { -1,  1 };
   for (size_t i = 0; i < 2; i++)
   {
      for (size_t j = 0; j < 2; j++)
      {
         root.node = &n;
         avl_setup_test_node(&n, &c, &w, NULL, 1);
         avl_setup_test_node(&c, &g, &z, &n, c_balances[i]);
         avl_setup_test_node(&g, &x, &y, &c, 0);
         avl_setup_test_node(&w, NULL, NULL, &n, w_balances[j]);

         avl_setup_test_node(&x, NULL, NULL, &g, 0);
         avl_setup_test_node(&y, NULL, NULL, &g, 0);

         switch (c_balances[i])
         {
            case 0:
               avl_setup_test_node(&z, NULL, &k, &c, -1);
               avl_setup_test_node(&k, NULL, NULL, &z, 0);
               break;
            case 1:
               avl_setup_test_node(&z, NULL, NULL, &c, 0);
               break;
         }

         __avl_eject_rebalance(&root, &w, (w_balances[j] > 0), &__avl_dummy_augment);

         TEST_ASSERT_EQUAL(&c, avl_parent(&n));
         TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));
         TEST_ASSERT_EQUAL(NULL, avl_parent(&c));
         TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));

         TEST_ASSERT_TRUE(avl_verify_recursive(&c) != -1);
      }
   }
}

static void test_avl_eject_rebalance_LR(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y, z;
   /*
    * create structure mimicking LR case (child_balance == -1)
    * plus arbitary nodes (x, y, z, w) attached to test for corruption
    *
    *       n                         g
    *      / \                      /   \
    *     c   w                   c       n
    *    / \   \      ==>        / \     / \
    *   x   g   ☠️              x  (y) (z)  w
    *      / \
    *    (y) (z)
    */

   /*
    * g_balances[i] drives which LR sub-case we hit:
    *   0  -> tmp_balance == 0:  n=0, c=0
    *  -1  -> tmp_balance == -1: n=1, c=0
    *   1  -> tmp_balance == 1:  n=0, c=-1
    */
   int g_balances[3] = {  0, -1,  1 };
   int n_expected[3] = {  0,  0, -1 };
   int c_expected[3] = {  0,  1,  0 };

   int w_balances[2] = { -1,  1 };
   for (size_t i = 0; i < 3; i++)
   {
      for (size_t j = 0; j < 2; j++)
      {
         root.node = &n;
         avl_setup_test_node(&n, &c, &w, NULL,  1);
         avl_setup_test_node(&c, &x, &g,  &n,  -1);
         avl_setup_test_node(&x, NULL, NULL, &c,  0);
         avl_setup_test_node(&w, NULL, NULL, &n,  w_balances[j]);

         switch (g_balances[i])
         {
            case 0:
               avl_setup_test_node(&g, &y, &z, &c,  0);
               avl_setup_test_node(&y, NULL, NULL, &g, 0);
               avl_setup_test_node(&z, NULL, NULL, &g, 0);
               break;
            case -1:
               avl_setup_test_node(&g, NULL, &z, &c, -1);
               avl_setup_test_node(&z, NULL, NULL, &g,  0);
               break;
            case 1:
               avl_setup_test_node(&g, &y, NULL, &c,  1);
               avl_setup_test_node(&y, NULL, NULL, &g,  0);
               break;
         }

         __avl_eject_rebalance(&root, &w, (w_balances[j] > 0), &__avl_dummy_augment);

         TEST_ASSERT_EQUAL(&g, avl_parent(&n));
         TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));
         TEST_ASSERT_EQUAL(&g, avl_parent(&c));
         TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));
         TEST_ASSERT_EQUAL(NULL, avl_parent(&g));
         TEST_ASSERT_EQUAL(0, avl_balance(&g));

         TEST_ASSERT_TRUE(avl_verify_recursive(&g) != -1);
      }
   }
}

static void test_avl_eject_rebalance_RR(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y, z, k;

   /*
    * create structure mimicking RR case
    * plus arbitary nodes (x, y, z, k, w) attached to test for corruption
    *
    *       n                              c
    *      / \                           /   \
    *     w   c                        n       g
    *    /   / \           ==>        / \     / \
    *  ☠️   z   g                    w   z   x   y
    *      /   / \                      /
    *    (k)  x   y                   (k)
    *
    */

   /*
    * c_balances[i] drives which RR sub-case we hit:
    *   0  -> child_balance ==  0: n=-1, c=1, height unchanged, stop
    *  -1  -> child_balance == -1: n=0,  c=0, height decreased, continue
    */
   int c_balances[2] = {  0, -1 };
   int n_expected[2] = { -1,  0 };
   int c_expected[2] = {  1,  0 };

   int w_balances[2] = { -1,  1 };
   for (size_t i = 0; i < 2; i++)
   {
      for (size_t j = 0; j < 2; j++)
      {
         root.node = &n;
         avl_setup_test_node(&n, &w, &c, NULL, -1);
         avl_setup_test_node(&c, &z, &g,  &n,  c_balances[i]);
         avl_setup_test_node(&g, &x, &y,  &c,  0);
         avl_setup_test_node(&w, NULL, NULL, &n, w_balances[j]);

         avl_setup_test_node(&x, NULL, NULL, &g, 0);
         avl_setup_test_node(&y, NULL, NULL, &g, 0);

         switch (c_balances[i])
         {
            case 0:
               avl_setup_test_node(&z, &k, NULL, &c, 1);
               avl_setup_test_node(&k, NULL, NULL, &z, 0);
               break;
            case -1:
               avl_setup_test_node(&z, NULL, NULL, &c, 0);
               break;
         }

         __avl_eject_rebalance(&root, &w, (w_balances[j] > 0), &__avl_dummy_augment);

         TEST_ASSERT_EQUAL(&c, avl_parent(&n));
         TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));
         TEST_ASSERT_EQUAL(NULL, avl_parent(&c));
         TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));

         TEST_ASSERT_TRUE(avl_verify_recursive(&c) != -1);
      }
   }
}

static void test_avl_eject_rebalance_RL(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y, z;

   /*
    * create structure mimicking RL case
    * plus arbitary nodes (x, y, z, w) attached to test for corruption
    *
    *       n                           g
    *      / \                        /   \
    *     w   c                     n       c
    *    /   / \        ==>        / \     / \
    *  ☠️   g   x                 w  (z) (y)  x
    *      / \
    *    (z) (y)
    */

   /*
    * g_balances[i] drives which RL sub-case we hit:
    *   0  -> tmp_balance ==  0: n=0,  c=0
    *   1  -> tmp_balance ==  1: n=-1, c=0
    *  -1  -> tmp_balance == -1: n=0,  c=1
    */
   int g_balances[3] = {  0,  1, -1 };
   int n_expected[3] = {  0,  0,  1 };
   int c_expected[3] = {  0, -1,  0 };

   int w_balances[2] = { -1,  1 };
   for (size_t i = 0; i < 3; i++)
   {
      for (size_t j = 0; j < 2; j++)
      {
         root.node = &n;
         avl_setup_test_node(&n, &w, &c, NULL, -1);
         avl_setup_test_node(&c, &g, &x,  &n,   1);
         avl_setup_test_node(&x, NULL, NULL, &c,  0);
         avl_setup_test_node(&w, NULL, NULL, &n,  w_balances[j]);

         switch (g_balances[i])
         {
            case 0:
               avl_setup_test_node(&g, &z, &y, &c,  0);
               avl_setup_test_node(&z, NULL, NULL, &g, 0);
               avl_setup_test_node(&y, NULL, NULL, &g, 0);
               break;
            case 1:
               avl_setup_test_node(&g, &z, NULL, &c,  1);
               avl_setup_test_node(&z, NULL, NULL, &g,  0);
               break;
            case -1:
               avl_setup_test_node(&g, NULL, &y, &c, -1);
               avl_setup_test_node(&y, NULL, NULL, &g,  0);
               break;
         }

         __avl_eject_rebalance(&root, &w, (w_balances[j] > 0), &__avl_dummy_augment);

         TEST_ASSERT_EQUAL(&g, avl_parent(&n));
         TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));
         TEST_ASSERT_EQUAL(&g, avl_parent(&c));
         TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));
         TEST_ASSERT_EQUAL(NULL, avl_parent(&g));
         TEST_ASSERT_EQUAL(0, avl_balance(&g));

         TEST_ASSERT_TRUE(avl_verify_recursive(&g) != -1);
      }
   }
}


static void test_avl_insert_rebalance_LL(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y;

   /*
    * create structure mimicking LL case (Y is the new node attached)
    * plus arbitrary nodes (x, y, z, w) attached to test for corruption
    *
    *         n                           c
    *        / \                        /   \
    *       c   w                     g       n
    *      / \           ==>         /       / \
    *     g   x                     Y       x   w
    *    /
    *   Y
    */

   /*
    * g_balances[i] drives which LL sub-case we hit:
    *   1  -> child_balance ==  1: n=0, c=0
    *  -1  -> child_balance == -1: n=0, c=0
    */
   int g_balances[2] = { 1, -1 };

   for (size_t i = 0; i < 2; i++)
   {
      root.node = &n;
      avl_setup_test_node(&n, &c, &w, NULL, 1);
      avl_setup_test_node(&w, NULL, NULL, &n, 0);
      avl_setup_test_node(&c, &g, &x, &n, 0);
      avl_setup_test_node(&x, NULL, NULL, &c, 0);
      avl_setup_test_node(&y, NULL, NULL, &g, 0);

      switch (g_balances[i])
      {
         case 1:
            avl_setup_test_node(&g, &y, NULL, &c, 0);
            break;

         case -1:
            avl_setup_test_node(&g, NULL, &y, &c, 0);
            break;
      }

      __avl_insert_rebalance(&root, &g, (g_balances[i] == 1), &__avl_dummy_augment);

      TEST_ASSERT_EQUAL(&c, avl_parent(&n));
      TEST_ASSERT_EQUAL(0, avl_balance(&n));
      TEST_ASSERT_EQUAL(NULL, avl_parent(&c));
      TEST_ASSERT_EQUAL(0, avl_balance(&c));

      TEST_ASSERT_TRUE(avl_verify_recursive(&c) != -1);
   }
}

static void test_avl_insert_rebalance_LR(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y;

   /*
    * create structure mimicking LR case (Y is the new node attached)
    * plus arbitrary nodes (x, y, z, w) attached to test for corruption
    *
    *         n                           g
    *        / \                        /   \
    *       c   w                     c       n
    *      / \           ==>         / \       \
    *     x   g                     x   Y       w
    *        /
    *       Y
    */

   /*
    * g_balances[i] drives which LR sub-case we hit:
    *   1  -> tmp_balance ==  1: n=-1, c=0
    *  -1  -> tmp_balance == -1: n=0,  c=1
    */
   int g_balances[2] = {  1, -1 };
   int n_expected[2] = { -1,  0 };
   int c_expected[2] = {  0,  1 };

   for (size_t i = 0; i < 2; i++)
   {
      root.node = &n;
      avl_setup_test_node(&n, &c, &w, NULL, 1);
      avl_setup_test_node(&w, NULL, NULL, &n, 0);
      avl_setup_test_node(&c, &x, &g, &n, 0);
      avl_setup_test_node(&x, NULL, NULL, &c, 0);
      avl_setup_test_node(&y, NULL, NULL, &g, 0);

      switch (g_balances[i])
      {
         case 1:
            avl_setup_test_node(&g, &y, NULL, &c, 0);
            break;

         case -1:
            avl_setup_test_node(&g, NULL, &y, &c, 0);
            break;
      }

      __avl_insert_rebalance(&root, &g, (g_balances[i] == 1), &__avl_dummy_augment);

      TEST_ASSERT_EQUAL(&g, avl_parent(&n));
      TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));
      TEST_ASSERT_EQUAL(&g, avl_parent(&c));
      TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));
      TEST_ASSERT_EQUAL(NULL, avl_parent(&g));
      TEST_ASSERT_EQUAL(0, avl_balance(&g));

      TEST_ASSERT_TRUE(avl_verify_recursive(&g) != -1);
   }
}

static void test_avl_insert_rebalance_RR(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y;

   /*
    * create structure mimicking RL case (Y is the new node attached)
    * plus arbitary nodes (x, y, z, w) attached to test for corruption
    *
    *       n                           c
    *      / \                        /   \
    *     w   c                     n       g
    *        / \        ==>        / \       \
    *       x   g                 w   x       Y
    *            \
    *             Y
    */

   /*
    * g_balances[i] drives which RL sub-case we hit:
    *   1  -> tmp_balance ==  1: n=0, c=0
    *  -1  -> tmp_balance == -1: n=0, c=0
    */
   int g_balances[2] = { 1, -1 };

   for (size_t i = 0; i < 2; i++)
   {
      root.node = &n;
      avl_setup_test_node(&n, &w, &c, NULL, -1);
      avl_setup_test_node(&w, NULL, NULL, &n, 0);
      avl_setup_test_node(&c, &x, &g, &n, 0);
      avl_setup_test_node(&x, NULL, NULL, &c, 0);
      avl_setup_test_node(&y, NULL, NULL, &g, 0);

      switch (g_balances[i])
      {
         case 1:
            avl_setup_test_node(&g, &y, NULL, &c, 0);
            break;

         case -1:
            avl_setup_test_node(&g, NULL, &y, &c, 0);
            break;
      }

      __avl_insert_rebalance(&root, &g, (g_balances[i] == 1), &__avl_dummy_augment);

      TEST_ASSERT_EQUAL(&c, avl_parent(&n));
      TEST_ASSERT_EQUAL(0, avl_balance(&n));
      TEST_ASSERT_EQUAL(NULL, avl_parent(&c));
      TEST_ASSERT_EQUAL(0, avl_balance(&c));

      TEST_ASSERT_TRUE(avl_verify_recursive(&c) != -1);
   }
}

static void test_avl_insert_rebalance_RL(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y;

   /*
    * create structure mimicking RL case (Y is the new node attached)
    * plus arbitary nodes (x, y, z, w) attached to test for corruption
    *
    *       n                           g
    *      / \                        /   \
    *     w   c                     n       c
    *        / \        ==>        /       / \
    *       g   x                 w       Y   x
    *        \
    *         Y
    */

   /*
    * g_balances[i] drives which RL sub-case we hit:
    *   1  -> tmp_balance ==  1: n=0, c=-1
    *  -1  -> tmp_balance == -1: n=1, c=0
    */
   int g_balances[2] = {  1, -1 };
   int n_expected[2] = {  0,  1 };
   int c_expected[2] = { -1,  0 };

   for (size_t i = 0; i < 2; i++)
   {
      root.node = &n;
      avl_setup_test_node(&n, &w, &c, NULL, -1);
      avl_setup_test_node(&w, NULL, NULL, &n, 0);
      avl_setup_test_node(&c, &g, &x, &n, 0);
      avl_setup_test_node(&x, NULL, NULL, &c, 0);
      avl_setup_test_node(&y, NULL, NULL, &g, 0);

      switch (g_balances[i])
      {
         case 1:
            avl_setup_test_node(&g, &y, NULL, &c, 0);
            break;

         case -1:
            avl_setup_test_node(&g, NULL, &y, &c, 0);
            break;
      }

      __avl_insert_rebalance(&root, &g, (g_balances[i] == 1), &__avl_dummy_augment);

      TEST_ASSERT_EQUAL(&g, avl_parent(&n));
      TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));
      TEST_ASSERT_EQUAL(&g, avl_parent(&c));
      TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));
      TEST_ASSERT_EQUAL(NULL, avl_parent(&g));
      TEST_ASSERT_EQUAL(0, avl_balance(&g));

      TEST_ASSERT_TRUE(avl_verify_recursive(&g) != -1);
   }
}


static void test_avl_subtree_height()
{
#ifdef NDEBUG
   TEST_IGNORE();

#else
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y, z, u;

   /*
    * create structure
    *
    *       n
    *      / \
    *     c   w
    *    / \   \
    *   x   g   u
    *      / \
    *     y   z
    */

   root.node = &n;
   avl_setup_test_node(&n, &c, &w, NULL, 1);
   avl_setup_test_node(&c, &x, &g,  &n, -1);
   avl_setup_test_node(&w, NULL, &u, &n, -1);

   avl_setup_test_node(&x, NULL, NULL, &c, 0);
   avl_setup_test_node(&u, NULL, NULL, &w, 0);

   avl_setup_test_node(&g, &y, &z, &c,  0);
   avl_setup_test_node(&y, NULL, NULL, &g, 0);
   avl_setup_test_node(&z, NULL, NULL, &g, 0);

   TEST_ASSERT_EQUAL(avl_verify_recursive(&n), avl_subtree_height(&n));
   TEST_ASSERT_EQUAL(avl_verify_recursive(&c), avl_subtree_height(&c));
   TEST_ASSERT_EQUAL(avl_verify_recursive(&g), avl_subtree_height(&g));
   TEST_ASSERT_EQUAL(avl_verify_recursive(&w), avl_subtree_height(&w));
   TEST_ASSERT_EQUAL(avl_verify_recursive(&x), avl_subtree_height(&x));

#endif /* NDEBUG */
}

static void test_avl_subtree_size()
{
#ifdef NDEBUG
   TEST_IGNORE();

#else
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y, z, u;

   /*
    * create structure
    *
    *       n
    *      / \
    *     c   w
    *    / \   \
    *   x   g   u
    *      / \
    *     y   z
    */

   root.node = &n;
   avl_setup_test_node(&n, &c, &w, NULL, 1);
   avl_setup_test_node(&c, &x, &g,  &n, -1);
   avl_setup_test_node(&w, NULL, &u, &n, -1);

   avl_setup_test_node(&x, NULL, NULL, &c, 0);
   avl_setup_test_node(&u, NULL, NULL, &w, 0);

   avl_setup_test_node(&g, &y, &z, &c,  0);
   avl_setup_test_node(&y, NULL, NULL, &g, 0);
   avl_setup_test_node(&z, NULL, NULL, &g, 0);

   TEST_ASSERT_EQUAL(8, avl_subtree_size(&n));
   TEST_ASSERT_EQUAL(5, avl_subtree_size(&c));
   TEST_ASSERT_EQUAL(3, avl_subtree_size(&g));
   TEST_ASSERT_EQUAL(2, avl_subtree_size(&w));
   TEST_ASSERT_EQUAL(1, avl_subtree_size(&x));

#endif /* NDEBUG */
}

static void test_avl_verify()
{
#ifdef NDEBUG
   TEST_IGNORE();

#else
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y, z, u;

   /*
    * create structure
    *
    *       n
    *      / \
    *     c   w
    *    / \   \
    *   x   g   u
    *      / \
    *     y   z
    */

   root.node = &n;
   avl_setup_test_node(&n, &c, &w, NULL, 1);
   avl_setup_test_node(&c, &x, &g,  &n, -1);
   avl_setup_test_node(&w, NULL, &u, &n, -1);

   avl_setup_test_node(&x, NULL, NULL, &c, 0);
   avl_setup_test_node(&u, NULL, NULL, &w, 0);

   avl_setup_test_node(&g, &y, &z, &c,  0);
   avl_setup_test_node(&y, NULL, NULL, &g, 0);
   avl_setup_test_node(&z, NULL, NULL, &g, 0);

   TEST_ASSERT_TRUE(avl_verify(&n));
   TEST_ASSERT_TRUE(avl_verify(&c));
   TEST_ASSERT_TRUE(avl_verify(&g));
   TEST_ASSERT_TRUE(avl_verify(&w));
   TEST_ASSERT_TRUE(avl_verify(&x));

   /*
    * edit structure
    *
    *       n
    *      / \
    *     c   w
    *    / \
    *   x   g
    *      /
    *     y
    */

   avl_setup_test_node(&w, NULL, NULL, &n, -1);
   avl_setup_test_node(&u, NULL, NULL, NULL, 0);

   avl_setup_test_node(&g, &y, NULL, &c,  0);
   avl_setup_test_node(&z, NULL, NULL, NULL, 0);

   TEST_ASSERT_FALSE(avl_verify(&n));
   TEST_ASSERT_TRUE(avl_verify(&c));
   TEST_ASSERT_FALSE(avl_verify(&g));
   TEST_ASSERT_FALSE(avl_verify(&w));
   TEST_ASSERT_TRUE(avl_verify(&x));

#endif /* NDEBUG */
}

int main(void)
{
   UnityBegin("test/bit.h");

   RUN_TEST(test_avl_eject_rebalance_LL);
   RUN_TEST(test_avl_eject_rebalance_LR);
   RUN_TEST(test_avl_eject_rebalance_RR);
   RUN_TEST(test_avl_eject_rebalance_RL);

   RUN_TEST(test_avl_insert_rebalance_LL);
   RUN_TEST(test_avl_insert_rebalance_LR);
   RUN_TEST(test_avl_insert_rebalance_RR);
   RUN_TEST(test_avl_insert_rebalance_RL);

   RUN_TEST(test_avl_subtree_height);
   RUN_TEST(test_avl_subtree_size);
   RUN_TEST(test_avl_verify);

   return UnityEnd();
}


#endif /* HUZLIB_AVL_TREE_TEST */


#endif /* HUZLIB_AVL_TREE_H */
