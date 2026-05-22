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
#define typecheck(type, expr) ((void)_Generic(  \
   (expr),                                      \
   type: 1                                      \
))
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
#define typecheck_expr(type, var, expr) ((void)typecheck(type, var), (expr))
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
      const typeof(((type *)0)->member) *__mcumptr = (ptr);          \
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
 * Append before return type:
 *    __huzlib_const__ - output depends ONLY on input (e.g., math functions)
 *    __huzlib_pure__  - no side effects, may read global memory (e.g., strlen)
 *
 * Append after parameter list:
 *    __huzlib_unsequenced__  - output depends ONLY on input (e.g., math functions)
 *    __huzlib_reproducible__ - no side effects, may read global memory (e.g., strlen)
 *
 * Falls back to empty for unsupported compilers.
 */
#ifndef HUZLIB_PURE_HINTS
#define HUZLIB_PURE_HINTS

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)

   #define __huzlib_unsequenced__  [[unsequenced]]
   #define __huzlib_reproducible__ [[reproducible]]

#else

   #define __huzlib_unsequenced__
   #define __huzlib_reproducible__

#endif

#if defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__) || defined(__xlC__) || defined(__zig__) || defined(__clang__) || defined(__GNUC__)

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
   #include <stddef.h>

#elif defined(__GNUC__) || defined(__clang__) || defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ZIG__) || defined(__xlC__) || defined(__ibmxl__)
   #define unreachable() __builtin_unreachable()

#elif defined(_MSC_VER) || defined(__POCC__)
   #define unreachable() __assume(0)

#else
   #define unreachable() do { for(;;); } while(0)

#endif
#endif /* unreachable */



#ifndef __huzlib_assert
/*
 * first, we check for NDEBUG, 
 * which means we are compiling under Optimized mode
 */
#ifdef NDEBUG

   #define __huzlib_assert(cond) ((void)0)

#else
   /*
    * now we check for -freestanding, 
    * which means <assert.h> is not available
    */
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
         } while(0)

      #else

         #define __huzlib_assert(cond) do {  \
            if (!(cond)) {                   \
               volatile int *__huz_fail = 0; \
               (void)*__huz_fail;            \
            }                                \
         } while(0)

      #endif

   #else

      #include <assert.h>
      #define __huzlib_assert(cond) assert(cond)

   #endif /* __STDC_HOSTED__ */

#endif /* NDEBUG */
#endif /* __huzlib_assert */


#endif /* HUZLIB_AVL_TREE_INCLUDES */



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
   uintptr_t __parent_vbalance; // WARN: Do not fucking change this (keep offset 0)
   struct avl_node *left, *right;
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
   struct avl_node_linked *first;
};


#define AVL_ROOT_INIT         ((struct avl_root)        { .node = NULL })
#define AVL_ROOT_CACHED_INIT  ((struct avl_root_cached) { .root = AVL_ROOT_INIT, .first = NULL })
#define AVL_ROOT_LINKED_INIT  ((struct avl_root_linked) { .root = AVL_ROOT_INIT, .first = NULL })


/*
 * avl_entry(ptr, type, avl_member)
 * ---------------------------------
 * Just like container_of but with a built-in typecheck — ptr must be
 * either struct avl_node * or struct avl_node_linked *, anything else
 * is a compile error. This stops you from accidentally passing the wrong
 * pointer type and silently getting a garbage result out of container_of.
 *
 * The _Generic is not doing any conversion here — both branches do the
 * exact same thing. It is purely acting as a type gate.
 */
#define avl_entry(ptr, type, avl_member) _Generic(*(ptr),       \
   struct avl_node:        container_of(ptr, type, avl_member), \
   struct avl_node_linked: container_of(ptr, type, avl_member)  \
)


/*
 * avl_cached(ptr) and avl_linked(ptr)
 * ------------------------------------
 * AVL trees come in 3 flavours — plain (avl_root/avl_node), cached
 * (avl_root_cached) and linked (avl_root_linked/avl_node_linked).
 * Code that works with one flavour often needs to convert to another,
 * for example passing a cached root into a function that takes a plain
 * root, or wrapping a plain node into a linked node.
 *
 * Instead of remembering a bunch of different conversion functions for
 * each direction and each type, you just pass the pointer and _Generic
 * picks the right conversion automatically based on its type. Both
 * directions are handled by the same macro.
 *
 *    avl_cached(ptr):
 *       struct avl_root *        ->  struct avl_root_cached *   (wrap)
 *       struct avl_root_cached * ->  struct avl_root *          (unwrap)
 *
 *    avl_linked(ptr):
 *       struct avl_root *        ->  struct avl_root_linked *   (wrap)
 *       struct avl_node *        ->  struct avl_node_linked *   (wrap)
 *       struct avl_root_linked * ->  struct avl_root *          (unwrap)
 *       struct avl_node_linked * ->  struct avl_node *          (unwrap)
 *
 * One subtle point — _Generic requires ALL branches to be semantically
 * valid even when not selected. So each branch explicitly casts ptr to
 * the correct struct pointer type before accessing its member, rather
 * than relying on the original ptr type. This avoids "struct X has no
 * member named Y" errors on non-selected branches.
 */
#define avl_cached(ptr) _Generic(*(ptr),                                                  \
   struct avl_root:        (container_of((void *)(ptr), struct avl_root_cached, root)),   \
   struct avl_root_cached: (&((struct avl_root_cached *)(ptr))->root)                     \
)

#define avl_linked(ptr) _Generic(*(ptr),                                                  \
   struct avl_root:        (container_of((void *)(ptr), struct avl_root_linked, root)),   \
   struct avl_node:        (container_of((void *)(ptr), struct avl_node_linked, node)),   \
   struct avl_root_linked: (&((struct avl_root_linked *)(ptr))->root),                    \
   struct avl_node_linked: (&((struct avl_node_linked *)(ptr))->node)                     \
)


/* --- sanity checks --- */
_Static_assert(offsetof(struct avl_node, __parent_vbalance) == 0, "Do not fuck with the avl_node type");
_Static_assert(offsetof(struct avl_root_cached, root) == 0, "Do not fuck with the avl_root_cached type");
_Static_assert(offsetof(struct avl_root_linked, root) == 0, "Do not fuck with the avl_root_linked type");
_Static_assert(offsetof(struct avl_node_linked, node) == 0, "Do not fuck with the avl_node_linked type");


/*
 * AVL Augmentation Callbacks
 * --------------------------
 * Some AVL tree use cases need to maintain extra data per node that
 * depends on the subtree below it — things like subtree sizes, interval
 * max endpoints, or priority sums. This is called augmentation.
 *
 * To support this, the internal tree operations (insert rebalance, eject
 * rebalance, rotation) expose hooks so the caller can keep their extra
 * data in sync as the tree changes shape. These hooks are the augmentation
 * callbacks.
 *
 * The public API (avl_insert, avl_eject etc.) uses dummy no-op callbacks
 * internally — so augmentation is truly zero cost when not needed. For
 * augmented trees, use the _augmented variants and pass your callbacks.
 *
 * All callbacks are force_inlined and called with concrete known function
 * pointers, so the compiler folds them away completely at -O2. Zero overhead.
 *
 * @update:    Recompute a node's augmented value from its children.
 *             Called on each node while walking up the tree during
 *             rebalancing. At each step, the node's children may have
 *             changed, so the augmented value must be recomputed.
 *
 *             Called during:
 *               - Walking up the tree during insert/eject rebalancing
 *
 * @copy:      Copy augmented data from one node to another.
 *             When ejecting a node with two children, the node is replaced
 *             by its inorder successor. The successor takes the ejected
 *             node's place in the tree, so its augmented data must be
 *             copied over too.
 *
 *             Called during:
 *               - Ejection of nodes with 2 children (successor replacement)
 *
 * @rotate:    Update augmented data after a rotation.
 *             Called immediately after a left or right rotation is performed.
 *             Typically just calls update on the two rotated nodes.
 *
 *             Called during:
 *               - After every rotate_left / rotate_right
 *
 * @propagate: Bulk update from a node up to a stop point (exclusive).
 *             When rebalancing finishes early (height unchanged after a
 *             rotation), the tree above the rebalancing point has not been
 *             visited yet. propagate walks from that point up to the root,
 *             calling update on each node on the way. It may early-terminate
 *             if the augmented value stops changing.
 *
 *             If @stop is NULL, propagates all the way to the root.
 *
 *             Called during:
 *               - Early exit paths in rebalancing (height unchanged after rotation)
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
 *       while (start != stop) {
 *           size_t old_size = os_size(start);
 *           os_update(start);
 *           if (os_size(start) == old_size) break; // early terminate
 *           start = avl_parent(start);
 *       }
 *   }
 *
 *   const struct avl_augment_callbacks os_callbacks = {
 *       .update    = os_update,
 *       .copy      = os_copy,
 *       .rotate    = os_rotate,
 *       .propagate = os_propagate,
 *   };
 *
 *
 * Performance note:
 *    The augmented variants (avl_eject_augmented, avl_insert_augmented etc.)
 *    are force_inlined. This means every call site gets the entire rebalance
 *    logic inlined into it with your concrete callbacks folded in — which is
 *    exactly what you want for zero overhead, but only if you call them from
 *    ONE place.
 *
 *    The recommended pattern is to wrap your augmented call in a plain
 *    non-inlined function, exactly like the library does for avl_eject:
 *
 *       void os_eject(struct avl_root *root, struct os_node *node) {
 *           avl_eject_augmented(root, &node->avl, &os_augment);
 *       }
 *
 *    avl_eject_augmented inlines once inside os_eject, the compiler sees
 *    the concrete os_augment callbacks and folds them in, and every call
 *    site just calls os_eject normally. One copy of the logic, zero
 *    function pointer overhead. If you skip this and call avl_eject_augmented
 *    directly at multiple call sites, the entire rebalance logic gets
 *    duplicated at each one and your binary size explodes.
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
extern HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ struct avl_node *avl_parent(const struct avl_node *node) __huzlib_reproducible__;
extern HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ int avl_balance(const struct avl_node *node) __huzlib_reproducible__;


/* --- mutate operations --- */
extern HUZLIB_AVL_TREE_API_INLINE void avl_link_node(struct avl_node *node, struct avl_node *parent, struct avl_node **link);
extern HUZLIB_AVL_TREE_API_INLINE void avl_link_node_linked(struct avl_node_linked *node, struct avl_node_linked *parent, struct avl_node **link, bool isleft);

extern HUZLIB_AVL_TREE_API_INLINE void avl_insert_rebalance(struct avl_root *root, struct avl_node *node, bool isleft);
extern HUZLIB_AVL_TREE_API_INLINE void avl_insert_rebalance_augmented(struct avl_root *root, struct avl_node *node, bool isleft, const struct avl_augment_callbacks *augment);

extern HUZLIB_AVL_TREE_API        void avl_eject(struct avl_root *root, struct avl_node *node);
extern HUZLIB_AVL_TREE_API_INLINE void avl_eject_augmented(struct avl_root *root, struct avl_node *node, const struct avl_augment_callbacks *augment);
extern HUZLIB_AVL_TREE_API        void avl_eject_cached(struct avl_root_cached *root, struct avl_node *node);
extern HUZLIB_AVL_TREE_API_INLINE void avl_eject_cached_augmented(struct avl_root_cached *root, struct avl_node *node, const struct avl_augment_callbacks *augment);
extern HUZLIB_AVL_TREE_API        void avl_eject_linked(struct avl_root_linked *root, struct avl_node_linked *node);
extern HUZLIB_AVL_TREE_API_INLINE void avl_eject_linked_augmented(struct avl_root_linked *root, struct avl_node_linked *node, const struct avl_augment_callbacks *augment);

extern HUZLIB_AVL_TREE_API        struct avl_node *avl_eject_first(struct avl_root *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_eject_first_augmented(struct avl_root *root, const struct avl_augment_callbacks *augment);
extern HUZLIB_AVL_TREE_API        struct avl_node *avl_eject_first_cached(struct avl_root_cached *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_eject_first_cached_augmented(struct avl_root_cached *root, const struct avl_augment_callbacks *augment);
extern HUZLIB_AVL_TREE_API        struct avl_node_linked *avl_eject_first_linked(struct avl_root_linked *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node_linked *avl_eject_first_linked_augmented(struct avl_root_linked *root, const struct avl_augment_callbacks *augment);

extern HUZLIB_AVL_TREE_API        struct avl_node *avl_eject_last(struct avl_root *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_eject_last_augmented(struct avl_root *root, const struct avl_augment_callbacks *augment);
extern HUZLIB_AVL_TREE_API        struct avl_node *avl_eject_last_cached(struct avl_root_cached *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_eject_last_cached_augmented(struct avl_root_cached *root, const struct avl_augment_callbacks *augment);
extern HUZLIB_AVL_TREE_API        struct avl_node_linked *avl_eject_last_linked(struct avl_root_linked *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node_linked *avl_eject_last_linked_augmented(struct avl_root_linked *root, const struct avl_augment_callbacks *augment);


/* --- debug/test operations --- */
#ifndef NDEBUG
extern size_t avl_subtree_height(const struct avl_node *subroot);
extern size_t avl_subtree_size(const struct avl_node *subroot);
extern size_t avl_subtree_dump(const struct avl_node *subroot, void (*dump_node)(const struct avl_node *), void (*dump_str)(const char *));
extern bool avl_verify(const struct avl_node *node);
#endif /* NDEBUG */


/* --- query operations --- */
extern HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ bool avl_is_empty(const struct avl_root *root) __huzlib_reproducible__;
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_first(const struct avl_root *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_last(const struct avl_root *root);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_postorder_first(const struct avl_root *root);

extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_subtree_first(const struct avl_node *subroot);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_subtree_last(const struct avl_node *subroot);
extern HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_subtree_postorder_first(const struct avl_node *subroot);


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

#define AVL_BALANCE_MASK  ((uintptr_t)0x03)

/* ------------------------------------------------- */
/* --------------- helper functions  --------------- */
/* ------------------------------------------------- */

#ifdef NDEBUG
   #define HUZLIB_AVL_TREE_INTERNAL static __huzlib_inline__
#else
   #define HUZLIB_AVL_TREE_INTERNAL static
#endif /* NDEBUG */


/*
 * __avl_parent(parent_vbalance)
 * -----------------------------
 * Returns the parent pointer stored in the packed field.
 */
HUZLIB_AVL_TREE_INTERNAL __huzlib_const__ struct avl_node *__avl_parent(const uintptr_t parent_vbalance) __huzlib_unsequenced__
{
   return (struct avl_node *)(parent_vbalance & ~AVL_BALANCE_MASK);
}

/*
 * __avl_balance(parent_vbalance)
 * ------------------------------
 * Returns the balance factor of the node.
 */
HUZLIB_AVL_TREE_INTERNAL __huzlib_const__ int __avl_balance(const uintptr_t parent_vbalance) __huzlib_unsequenced__
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
HUZLIB_AVL_TREE_INTERNAL void __avl_set_parent_balance(struct avl_node *restrict node, const struct avl_node *restrict parent, const int balance)
{
   __huzlib_assert(node && (balance >= -1) && (balance <= 1));
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
HUZLIB_AVL_TREE_INTERNAL void __avl_set_parent(struct avl_node *restrict node, const struct avl_node *restrict parent)
{
   __huzlib_assert(node);
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
HUZLIB_AVL_TREE_INTERNAL struct avl_node **__avl_parent_ptr(struct avl_root *restrict root, struct avl_node *restrict node, struct avl_node *restrict parent, bool *restrict isleft)
{
   __huzlib_assert(root && node && isleft && (avl_parent(node) == parent));

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
HUZLIB_AVL_TREE_INTERNAL void __avl_delink_node(struct avl_node *restrict child, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   __huzlib_assert(link && (avl_parent(*link)) == parent && (((*link)->left == child) || ((*link)->right == child)));
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
HUZLIB_AVL_TREE_INTERNAL void __avl_replace_node(struct avl_node *restrict old, struct avl_node *restrict new, struct avl_node **restrict link, void (*augment_copy)(struct avl_node *, struct avl_node *))
{
   __huzlib_assert(old && new && link && (*link == old) && augment_copy);
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
HUZLIB_AVL_TREE_INTERNAL void __avl_add_linked(struct avl_node_linked *restrict new, struct avl_node_linked *restrict prev, struct avl_node_linked *restrict next)
{
   __huzlib_assert(new);

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
HUZLIB_AVL_TREE_INTERNAL void __avl_rm_linked(struct avl_node_linked *restrict prev, struct avl_node_linked *restrict next)
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
HUZLIB_AVL_TREE_INTERNAL void __avl_rotate_left(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node **restrict link)
{
   __huzlib_assert(node && child && link && (*link == node) && (node->right == child));

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
HUZLIB_AVL_TREE_INTERNAL void __avl_rotate_right(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node **restrict link)
{
   __huzlib_assert(node && child && link && (*link == node) && (node->left == child));

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
HUZLIB_AVL_TREE_INTERNAL void __avl_rotate_left_right(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   __huzlib_assert(node && child && parent && link && (*link == parent) && (node->right == child) && (parent->left == node));

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
HUZLIB_AVL_TREE_INTERNAL void __avl_rotate_right_left(struct avl_node *restrict node, struct avl_node *restrict child, struct avl_node *restrict parent, struct avl_node **restrict link)
{
   __huzlib_assert(node && child && parent && link && (*link == parent) && (node->left == child) && (parent->right == node));

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
HUZLIB_AVL_TREE_INTERNAL void __avl_insert_rebalance(struct avl_root *restrict root, struct avl_node *restrict node, bool isleft, const struct avl_augment_callbacks *restrict augment)
{
   __huzlib_assert(root && augment);
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
HUZLIB_AVL_TREE_INTERNAL void __avl_eject_rebalance(struct avl_root *restrict root, struct avl_node *restrict node, bool isleft, const struct avl_augment_callbacks *restrict augment)
{
   __huzlib_assert(root && augment);
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
HUZLIB_AVL_TREE_INTERNAL struct avl_node *__avl_eject(struct avl_root *restrict root, struct avl_node *restrict node, const struct avl_augment_callbacks *restrict augment)
{
   __huzlib_assert(root && node && augment);

   bool isleft;
   struct avl_node *restrict parent = avl_parent(node);
   struct avl_node **restrict link = __avl_parent_ptr(root, node, parent, &isleft);

   if (!node->left)
   {
      /*
       * case: 'node' has only right child or is leaf
       *
       *   |                       |
       *   n           ==>        (s)
       *    \
       *    (s)
       *
       * NOTE: due to AVL properties, 's' must be a leaf node
       */

      __avl_delink_node(node->right, parent, link);
      node = (node->right) ? node->right : parent;
      // return node->right if exists or parent in case node is first-node
   }
   else if (!node->right /* && node->left */)
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
      // 'node' cannot be 'first' node in tree (due to node->left existing)
   }
   else /* (node->right && node->left) */
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
      // 'node' cannot be 'first' node in tree (due to node->left existing)
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
HUZLIB_AVL_TREE_INTERNAL void __avl_dummy_augment_update(struct avl_node *node)                             { (void)0; }
HUZLIB_AVL_TREE_INTERNAL void __avl_dummy_augment_copy(struct avl_node *old, struct avl_node *new)          { (void)0; }
HUZLIB_AVL_TREE_INTERNAL void __avl_dummy_augment_rotate(struct avl_node *old, struct avl_node *new)        { (void)0; }
HUZLIB_AVL_TREE_INTERNAL void __avl_dummy_augment_propagate(struct avl_node *node, struct avl_node *stop)   { (void)0; }

const struct avl_augment_callbacks __avl_dummy_augment = {
   .update    = __avl_dummy_augment_update,
   .copy      = __avl_dummy_augment_copy,
   .rotate    = __avl_dummy_augment_rotate,
   .propagate = __avl_dummy_augment_propagate,
};


/*
 * __avl_next(subroot_parent, node)
 * --------------------------------
 * In-order successor with subtree boundary.
 *
 * @subroot_parent: sentinel node (NULL for full tree, avl_parent(subroot) for subtree)
 * @node:           current node
 *
 * Return: next node in in-order, or NULL if none
 *
 * NOTE:
 * The compiler folds avl_parent(subroot) → subroot when subroot is a dereferenced
 * pointer, making this zero-cost for both use cases.
 */
HUZLIB_AVL_TREE_INTERNAL struct avl_node *__avl_next(const struct avl_node *restrict subroot_parent, const struct avl_node *restrict node)
{
   __huzlib_assert(node);

   if (node->right)
   {
      node = node->right;
      while (node->left)
         node = node->left;
      return (struct avl_node *)node;
   }
   else
   {
      struct avl_node *restrict parent = avl_parent(node);
      while (parent != subroot_parent && node == parent->right)
      {
         node = parent;
         parent = avl_parent(parent);
      }
      return parent;
   }
}

/*
 * __avl_prev(subroot_parent, node)
 * --------------------------------
 * In-order predecessor with subtree boundary.
 *
 * @subroot_parent: sentinel node (NULL for full tree, avl_parent(subroot) for subtree)
 * @node:           current node
 *
 * Return: prev node in in-order, or NULL if none
 *
 * NOTE:
 * The compiler folds avl_parent(subroot) → subroot when subroot is a dereferenced
 * pointer, making this zero-cost for both use cases.
 */
HUZLIB_AVL_TREE_INTERNAL struct avl_node *__avl_prev(const struct avl_node *restrict subroot_parent, const struct avl_node *restrict node)
{
   __huzlib_assert(node);

   if (node->left)
   {
      node = node->left;
      while (node->right)
         node = node->right;
      return (struct avl_node *)node;
   }
   else
   {
      struct avl_node *restrict parent = avl_parent(node);
      while (parent != subroot_parent && node == parent->left)
      {
         node = parent;
         parent = avl_parent(parent);
      }
      return parent;
   }
}

/*
 * __avl_preorder_next(subroot_parent, node)
 * -----------------------------------------
 * Pre-order successor with subtree boundary.
 *
 * @subroot_parent: sentinel node (NULL for full tree, avl_parent(subroot) for subtree)
 * @node:           current node
 *
 * Return: next node in pre-order, or NULL if none
 *
 * Pre-order traversal: node → left subtree → right subtree
 *
 * Cases:
 *   - Has left child  → return left child
 *   - Has right child → return right child
 *   - Leaf            → walk up until finding an ancestor that is a left child
 *                       and has a right child, then return that right child
 */
HUZLIB_AVL_TREE_INTERNAL struct avl_node *__avl_preorder_next(const struct avl_node *restrict subroot_parent, const struct avl_node *restrict node)
{
   __huzlib_assert(node);

   if (node->left)
      return (struct avl_node *)node->left;

   else if (node->right)
      return (struct avl_node *)node->right;

   else
      while (true)
      {
         struct avl_node *restrict parent = avl_parent(node);

         if (parent == subroot_parent)
            return NULL;

         if (node == parent->left && parent->right)
            return parent->right;

         node = parent;
      }
}

/*
 * __avl_postorder_next(subroot_parent, node)
 * ------------------------------------------
 * Post-order successor with subtree boundary.
 *
 * @subroot_parent: sentinel node (NULL for full tree, avl_parent(subroot) for subtree)
 * @node:           current node
 *
 * Return: next node in post-order, or NULL if none
 *
 * Post-order traversal: left subtree → right subtree → node
 *
 * Cases:
 *   - Current node is left child and parent has right child
 *       → first post-order node in parent's right subtree
 *   - Otherwise → parent
 *
 */
HUZLIB_AVL_TREE_INTERNAL struct avl_node *__avl_postorder_next(const struct avl_node *restrict subroot_parent, const struct avl_node *restrict node)
{
   __huzlib_assert(node);
   struct avl_node *restrict parent = avl_parent(node);

   if (parent == subroot_parent)
      return NULL;

   else if (node == parent->left && parent->right)
      return avl_subtree_postorder_first(parent->right);

   else
      return parent;
}



/* ------------------------------------------------- */
/* ------------- initialize operations ------------- */
/* ------------------------------------------------- */

HUZLIB_AVL_TREE_API_INLINE void avl_node_init(struct avl_node *restrict node)
{
   __huzlib_assert(node);
   node->left = NULL;
   node->right = NULL;
   node->__parent_vbalance = 0;
}

HUZLIB_AVL_TREE_API_INLINE void avl_node_linked_init(struct avl_node_linked *restrict node)
{
   __huzlib_assert(node);
   avl_node_init(&node->node);
   node->prev = NULL;
   node->next = NULL;
}

HUZLIB_AVL_TREE_API_INLINE void avl_root_init(struct avl_root *restrict root)
{
   __huzlib_assert(root);
   *root = AVL_ROOT_INIT;
}

HUZLIB_AVL_TREE_API_INLINE void avl_root_cached_init(struct avl_root_cached *restrict root)
{
   __huzlib_assert(root);
   *root = AVL_ROOT_CACHED_INIT;
}

HUZLIB_AVL_TREE_API_INLINE void avl_root_linked_init(struct avl_root_linked *restrict root)
{
   __huzlib_assert(root);
   *root = AVL_ROOT_LINKED_INIT;
}



/* ------------------------------------------------- */
/* --------------- getter functions  --------------- */
/* ------------------------------------------------- */

HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ struct avl_node *avl_parent(const struct avl_node *restrict node) __huzlib_reproducible__
{
   __huzlib_assert(node);
   return __avl_parent(node->__parent_vbalance);
}

HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ int avl_balance(const struct avl_node *restrict node) __huzlib_reproducible__
{
   __huzlib_assert(node);
   return __avl_balance(node->__parent_vbalance);
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
   __huzlib_assert(node && link);
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
   __huzlib_assert(node && ((!parent) || ((isleft) ? (avl_linked(parent)->left == *link) : (avl_linked(parent)->right == *link))));

   avl_link_node(avl_linked(node), avl_linked(parent), link);
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
 * @root:   the tree root, not NULL @node:   parent of the inserted node, may be NULL if tree was empty
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

HUZLIB_AVL_TREE_API_INLINE void avl_eject_augmented(struct avl_root *restrict root, struct avl_node *restrict node, const struct avl_augment_callbacks *restrict augment)
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

HUZLIB_AVL_TREE_API_INLINE void avl_eject_cached_augmented(struct avl_root_cached *restrict root, struct avl_node *restrict node, const struct avl_augment_callbacks *restrict augment)
{
   __huzlib_assert(root);
   struct avl_node *tmp = __avl_eject(avl_cached(root), node, augment);
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
HUZLIB_AVL_TREE_API void avl_eject_linked(struct avl_root_linked *restrict root, struct avl_node_linked *restrict node)
{
   avl_eject_linked_augmented(root, node, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API_INLINE void avl_eject_linked_augmented(struct avl_root_linked *restrict root, struct avl_node_linked *restrict node, const struct avl_augment_callbacks *restrict augment)
{
   __huzlib_assert(root && node && augment);

   bool isleft;
   struct avl_node *restrict base_node = avl_linked(node);
   struct avl_node *restrict base_parent = avl_parent(base_node);
   struct avl_node **restrict link = __avl_parent_ptr(avl_linked(root), base_node, base_parent, &isleft);

   if (base_node->left && base_node->right)
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

      struct avl_node *restrict succ = avl_linked(node->next);
      if (succ == base_node->right)
      {
         /*
          * case: succ's parent is node
          *
          *         |                             |
          *         n                             s
          *       /   \                         /   \
          *     w       s         ==>         w      (x)
          *    / \       \                   / \
          *  (a) (b)     (x)               (a) (b)
          */

         base_parent = succ;
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

         base_parent = avl_parent(succ);
         isleft = true;

         __avl_delink_node(succ->right, base_parent, &base_parent->left);

         __avl_set_parent(base_node->right, succ);
         succ->right = base_node->right;
      }
      __avl_set_parent(base_node->left, succ);
      succ->left = base_node->left;

      __avl_replace_node(base_node, succ, link, augment->copy);
   }
   else if (base_node->left)
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

      __avl_delink_node(base_node->left, base_parent, link);
   }
   else if (base_node->right)
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

      __avl_delink_node(base_node->right, base_parent, link);

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
   }

   __avl_rm_linked(node->prev, node->next);
   __avl_eject_rebalance(avl_linked(root), base_parent, isleft, augment);
}



/*
 * avl_eject_first(root, [augment])
 * --------------------------------
 * Removes and returns the leftmost (smallest) node from the tree.
 *
 * Finds the leftmost node by walking left children, then splices its
 * right child (if any) into its position, and restores AVL balance.
 *
 * @root:    the tree root, not NULL (tree must not be empty)
 * @augment: augment callback functions
 *
 * Return: pointer to the ejected node (caller must free if needed)
 */
HUZLIB_AVL_TREE_API struct avl_node *avl_eject_first(struct avl_root *restrict root)
{
   return avl_eject_first_augmented(root, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_eject_first_augmented(struct avl_root *restrict root, const struct avl_augment_callbacks *restrict augment)
{
   __huzlib_assert(root && augment);

   if (__huzlib_unlikely__(avl_is_empty(root)))
      return NULL;

   struct avl_node *restrict node = root->node;
   struct avl_node *restrict parent = NULL;

   if (node->left)
   {
      do {
         parent = node;
         node = node->left;
      } while (node->left);
      __avl_delink_node(node->right, parent, &parent->left);
   }
   else
   {
      __avl_delink_node(node->right, parent, &root->node);
   }

   __avl_eject_rebalance(root, parent, true, augment);
   return node;
}

/*
 * avl_eject_first_cached(root, [augment])
 * ---------------------------------------
 * Removes and returns the leftmost node from a cached AVL tree.
 *
 * Similar to avl_eject_first() but uses root->first for O(1) access
 * to the leftmost node. Updates root->first after ejection.
 *
 * @root:    the cached tree root, not NULL
 * @node:    the node to remove, not NULL
 * @augment: augment callback functions
 *
 * Return: pointer to the ejected node
 */
HUZLIB_AVL_TREE_API struct avl_node *avl_eject_first_cached(struct avl_root_cached *restrict root)
{
   return avl_eject_first_cached_augmented(root, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_eject_first_cached_augmented(struct avl_root_cached *restrict root, const struct avl_augment_callbacks *restrict augment)
{
   __huzlib_assert(root && augment);

   if (__huzlib_unlikely__(avl_is_empty(avl_cached(root))))
      return NULL;

   struct avl_node *restrict node = root->first;
   struct avl_node *restrict parent;

   if (node == avl_cached(root)->node)
   {
      parent = NULL;
      __avl_delink_node(node->right, parent, &avl_cached(root)->node);
   }
   else
   {
      parent = avl_parent(node);
      __avl_delink_node(node->right, parent, &parent->left);
   }

   __avl_eject_rebalance(avl_cached(root), parent, true, augment);
   root->first = (node->right) ? node->right : parent;

   return node;
}

/*
 * avl_eject_first_linked(root, [augment])
 * ---------------------------------------
 * Removes and returns the leftmost node from a linked AVL tree.
 *
 * Uses root->first for O(1) access to the leftmost node, then ejects
 * it while maintaining both tree balance and doubly-linked list pointers.
 * Updates root->first and root->last if the tree had only one node.
 *
 * @root:    the linked tree root, not NULL
 * @augment: augment callback functions
 *
 * Return: pointer to the ejected node (caller must free if needed)
 */
HUZLIB_AVL_TREE_API struct avl_node_linked *avl_eject_first_linked(struct avl_root_linked *restrict root)
{
   return avl_eject_first_linked_augmented(root, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node_linked *avl_eject_first_linked_augmented(struct avl_root_linked *restrict root, const struct avl_augment_callbacks *restrict augment)
{
   __huzlib_assert(root && augment);

   struct avl_root *restrict base_root = avl_linked(root);

   if (__huzlib_unlikely__(avl_is_empty(base_root)))
      return NULL;

   struct avl_node_linked *restrict node = root->first;
   struct avl_node *restrict base_node = avl_linked(node);
   struct avl_node *restrict base_parent;

   if (base_node == base_root->node)
   {
      base_parent = NULL;
      __avl_delink_node(base_node->right, base_parent, &base_root->node);
   }
   else
   {
      base_parent = avl_parent(base_node);
      __avl_delink_node(base_node->right, base_parent, &base_parent->left);
   }

   __avl_rm_linked(node->prev, node->next);
   __avl_eject_rebalance(base_root, base_parent, true, augment);

   if (node == root->first)
      root->first = node->next;

   return node;
}


/*
 * avl_eject_last(root, [augment])
 * -------------------------------
 * Removes and returns the rightmost (largest) node from the tree.
 *
 * Finds the rightmost node by walking right children, then splices its
 * left child (if any) into its position, and restores AVL balance.
 *
 * @root:    the tree root, not NULL (tree must not be empty)
 * @augment: augment callback functions
 *
 * Return: pointer to the ejected node (caller must free if needed)
 */
HUZLIB_AVL_TREE_API struct avl_node *avl_eject_last(struct avl_root *restrict root)
{
   return avl_eject_last_augmented(root, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_eject_last_augmented(struct avl_root *restrict root, const struct avl_augment_callbacks *restrict augment)
{
   __huzlib_assert(root && augment);

   if (__huzlib_unlikely__(avl_is_empty(root)))
      return NULL;

   struct avl_node *restrict node = root->node;
   struct avl_node *restrict parent = NULL;

   if (node->right)
   {
      do {
         parent = node;
         node = node->right;
      } while (node->right);
      __avl_delink_node(node->left, parent, &parent->right);
   }
   else
   {
      __avl_delink_node(node->left, parent, &root->node);
   }

   __avl_eject_rebalance(root, parent, false, augment);
   return node;
}

/*
 * avl_eject_last_cached(root, [augment])
 * --------------------------------------
 * Removes and returns the rightmost node from a cached AVL tree.
 *
 * Since cached roots do not track the rightmost node, this performs
 * a full tree traversal to find the last node. If the ejected node
 * was also the leftmost (tree had only one node), root->first is updated.
 *
 * @root:    the cached tree root, not NULL
 * @augment: augment callback functions
 *
 * Return: pointer to the ejected node
 */
HUZLIB_AVL_TREE_API struct avl_node *avl_eject_last_cached(struct avl_root_cached *restrict root)
{
   return avl_eject_last_cached_augmented(root, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_eject_last_cached_augmented(struct avl_root_cached *restrict root, const struct avl_augment_callbacks *restrict augment)
{
   struct avl_node *restrict node = avl_eject_last_augmented(avl_cached(root), augment);
   if (node == root->first)
      root->first = NULL;
   return node;
}

/*
 * avl_eject_last_linked(root, [augment])
 * --------------------------------------
 * Removes and returns the rightmost node from a linked AVL tree.
 *
 * Uses root->last for O(1) access to the rightmost node, then ejects
 * it while maintaining both tree balance and doubly-linked list pointers.
 * Updates root->first and root->last if the tree had only one node.
 *
 * @root:    the linked tree root, not NULL
 * @augment: augment callback functions
 *
 * Return: pointer to the ejected node (caller must free if needed)
 */
HUZLIB_AVL_TREE_API struct avl_node_linked *avl_eject_last_linked(struct avl_root_linked *restrict root)
{
   return avl_eject_last_linked_augmented(root, &__avl_dummy_augment);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node_linked *avl_eject_last_linked_augmented(struct avl_root_linked *restrict root, const struct avl_augment_callbacks *restrict augment)
{
   struct avl_node *restrict base_node = avl_eject_last_augmented(avl_linked(root), augment);
   struct avl_node_linked *restrict node = avl_linked(base_node);
   if (node == root->first)
      root->first = NULL;
   return node;
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
   __huzlib_assert(dump_node && dump_str);

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
   __huzlib_assert(node);

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



/* ------------------------------------------------- */
/* --------------- query operations  --------------- */
/* ------------------------------------------------- */

HUZLIB_AVL_TREE_API_INLINE __huzlib_pure__ bool avl_is_empty(const struct avl_root *restrict root) __huzlib_reproducible__
{
   __huzlib_assert(root);
   return root->node == NULL;
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_first(const struct avl_root *restrict root)
{
   __huzlib_assert(root);
   if (avl_is_empty(root))
      return NULL;
   else
      return avl_subtree_first(root->node);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_last(const struct avl_root *restrict root)
{
   __huzlib_assert(root);
   if (avl_is_empty(root))
      return NULL;
   else
      return avl_subtree_last(root->node);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_postorder_first(const struct avl_root *restrict root)
{
   __huzlib_assert(root);
   if (avl_is_empty(root))
      return NULL;
   else
      return avl_subtree_postorder_first(root->node);
}


HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_subtree_first(const struct avl_node *restrict node)
{
   __huzlib_assert(node);
   while (node->left)
      node = node->left;
   return (struct avl_node *)node;
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_subtree_last(const struct avl_node *restrict node)
{
   __huzlib_assert(node);
   while (node->right)
      node = node->right;
   return (struct avl_node *)node;
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_subtree_postorder_first(const struct avl_node *restrict node)
{
   __huzlib_assert(node);
   node = avl_subtree_first(node);

   if (node->right)
      return node->right;
   else
      return (struct avl_node *)node;
}



/* -------------------------------------------------- */
/* ---------------- tree traversals  ---------------- */
/* -------------------------------------------------- */

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_next(const struct avl_node *restrict node)
{
   return __avl_next(NULL, node);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_prev(const struct avl_node *restrict node)
{
   return __avl_prev(NULL, node);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_preorder_next(const struct avl_node *restrict node)
{
   return __avl_preorder_next(NULL, node);
}

HUZLIB_AVL_TREE_API_INLINE struct avl_node *avl_postorder_next(const struct avl_node *restrict node)
{
   return __avl_postorder_next(NULL, node);
}



/* ------------------------------------------------- */
/* -------------- subtree traversals  -------------- */
/* ------------------------------------------------- */

HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_next(const struct avl_node *restrict subroot, const struct avl_node *restrict node)
{
   __huzlib_assert(subroot);
   return __avl_next(avl_parent(subroot), node);
}

HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_prev(const struct avl_node *restrict subroot, const struct avl_node *restrict node)
{
   __huzlib_assert(subroot);
   return __avl_prev(avl_parent(subroot), node);
}

HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_preorder_next(const struct avl_node *restrict subroot, const struct avl_node *restrict node)
{
   __huzlib_assert(subroot);
   return __avl_preorder_next(avl_parent(subroot), node);
}

HUZLIB_AVL_TREE_API struct avl_node *avl_subtree_postorder_next(const struct avl_node *restrict subroot, const struct avl_node *restrict node)
{
   __huzlib_assert(subroot);
   return __avl_postorder_next(avl_parent(subroot), node);
}





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
   __huzlib_assert(node && balance >= -1 && balance <= 1);
   node->left = left;
   node->right = right;
   __avl_set_parent_balance(node, parent, balance);
}

/*
 * avl_setup_test_node_linked(node, left, right, parent, balance, prev, next)
 * --------------------------------------------------------------------------
 * setups 'node' for testing purposes with children 'left' & 'right', parent 'parent', balance 'balance'
 * and in-order 'prev' and 'next'
 *
 * @node:    test node
 * @left:    left child tobe set
 * @right:   right child tobe set
 * @parent:  parent tobe set
 * @balance: balance tobe set
 * @prev:    inorder predecessor
 * @next:    inorder successor
 */
static void avl_setup_test_node_linked(struct avl_node_linked *node, struct avl_node_linked *left, struct avl_node_linked *right, struct avl_node_linked *parent, int balance, struct avl_node_linked *prev, struct avl_node_linked *next)
{
   __huzlib_assert(node);
   avl_setup_test_node(
      avl_linked(node),
      avl_linked(left),
      avl_linked(right),
      avl_linked(parent),
      balance
   );
   __avl_add_linked(node, prev, next); 
}

/*
 * avl_verify_recursive(node, augment_verify)
 * ------------------------------------------
 * verify 'node' and it's subtree are a correct BST-nodes and AVL-nodes
 *
 * @node:           node to verify
 * @augment_verify: callback to verify augmented properties
 *
 * Return:
 *    valid node:   height of subtree starting from 'node' (h >= 0)
 *    invalid node: -1
 */
static long long avl_verify_recursive(struct avl_node *node, bool (*augment_verify)(struct avl_node *))
{
   if (!node)
      return 0;

   long long left_height = avl_verify_recursive(node->left, augment_verify);
   long long right_height = avl_verify_recursive(node->right, augment_verify);

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

   // verify augmented properties
   if (!augment_verify(node))
      return -1;

   return 1 + ((left_height > right_height) ? left_height : right_height);
}

void setUp(void) {}
void tearDown(void) {}
bool dummy_augment_verify(struct avl_node *node) { return true; }

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
    *    / \   \                            \
    *   x   y  (k)                          (k)
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

         TEST_ASSERT_EQUAL_PTR(&c, avl_parent(&n));
         TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));

         TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&c));
         TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));

         TEST_ASSERT_TRUE(avl_verify_recursive(&c, &dummy_augment_verify) != -1);
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

         TEST_ASSERT_EQUAL_PTR(&g, avl_parent(&n));
         TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));

         TEST_ASSERT_EQUAL_PTR(&g, avl_parent(&c));
         TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));

         TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&g));
         TEST_ASSERT_EQUAL(0, avl_balance(&g));

         TEST_ASSERT_TRUE(avl_verify_recursive(&g, &dummy_augment_verify) != -1);
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

         TEST_ASSERT_EQUAL_PTR(&c, avl_parent(&n));
         TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));

         TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&c));
         TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));

         TEST_ASSERT_TRUE(avl_verify_recursive(&c, &dummy_augment_verify) != -1);
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

         TEST_ASSERT_EQUAL_PTR(&g, avl_parent(&n));
         TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));

         TEST_ASSERT_EQUAL_PTR(&g, avl_parent(&c));
         TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));

         TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&g));
         TEST_ASSERT_EQUAL(0, avl_balance(&g));

         TEST_ASSERT_TRUE(avl_verify_recursive(&g, &dummy_augment_verify) != -1);
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

      TEST_ASSERT_EQUAL_PTR(&c, avl_parent(&n));
      TEST_ASSERT_EQUAL(0, avl_balance(&n));

      TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&c));
      TEST_ASSERT_EQUAL(0, avl_balance(&c));

      TEST_ASSERT_TRUE(avl_verify_recursive(&c, &dummy_augment_verify) != -1);
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

      TEST_ASSERT_EQUAL_PTR(&g, avl_parent(&n));
      TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));

      TEST_ASSERT_EQUAL_PTR(&g, avl_parent(&c));
      TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));

      TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&g));
      TEST_ASSERT_EQUAL(0, avl_balance(&g));

      TEST_ASSERT_TRUE(avl_verify_recursive(&g, &dummy_augment_verify) != -1);
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

      TEST_ASSERT_EQUAL_PTR(&c, avl_parent(&n));
      TEST_ASSERT_EQUAL(0, avl_balance(&n));

      TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&c));
      TEST_ASSERT_EQUAL(0, avl_balance(&c));

      TEST_ASSERT_TRUE(avl_verify_recursive(&c, &dummy_augment_verify) != -1);
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

      TEST_ASSERT_EQUAL_PTR(&g, avl_parent(&n));
      TEST_ASSERT_EQUAL(n_expected[i], avl_balance(&n));

      TEST_ASSERT_EQUAL_PTR(&g, avl_parent(&c));
      TEST_ASSERT_EQUAL(c_expected[i], avl_balance(&c));

      TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&g));
      TEST_ASSERT_EQUAL(0, avl_balance(&g));

      TEST_ASSERT_TRUE(avl_verify_recursive(&g, &dummy_augment_verify) != -1);
   }
}


static void test_avl_eject_2child_shallow_successor(void)
{
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

   struct avl_node *res = __avl_eject(&root, &g, &__avl_dummy_augment);

   TEST_ASSERT_EQUAL_PTR(&g, res);

   // Root unchanged
   TEST_ASSERT_EQUAL_PTR(&n, root.node);

   // ancestor's balance unchanged
   TEST_ASSERT_EQUAL(1, avl_balance(&n));
   TEST_ASSERT_EQUAL(-1, avl_balance(&c));

   // z now replaces g
   TEST_ASSERT_EQUAL_PTR(&y, z.left);
   TEST_ASSERT_EQUAL_PTR(NULL, z.right);
   TEST_ASSERT_EQUAL_PTR(&c, avl_parent(&z));
   TEST_ASSERT_EQUAL(1, avl_balance(&z));

   // y new parent is z
   TEST_ASSERT_EQUAL_PTR(&z, avl_parent(&y));

   // c's right child is now z
   TEST_ASSERT_EQUAL_PTR(&x, c.left);
   TEST_ASSERT_EQUAL_PTR(&z, c.right);
   TEST_ASSERT_EQUAL_PTR(&n, avl_parent(&c));
   TEST_ASSERT_EQUAL(-1, avl_balance(&c));
}

static void test_avl_eject_2child_deep_successor(void)
{
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

   struct avl_node *res = __avl_eject(&root, &c, &__avl_dummy_augment);

   TEST_ASSERT_EQUAL_PTR(&c, res);

   // Root unchanged
   TEST_ASSERT_EQUAL_PTR(&n, root.node);

   // y now replaces c
   TEST_ASSERT_EQUAL_PTR(&x, y.left);
   TEST_ASSERT_EQUAL_PTR(&g, y.right);
   TEST_ASSERT_EQUAL_PTR(&n, avl_parent(&y));
   TEST_ASSERT_EQUAL(-1, avl_balance(&y));

   // g new parent is y
   TEST_ASSERT_EQUAL_PTR(&y, avl_parent(&g));
   TEST_ASSERT_EQUAL_PTR(NULL, g.left);

   // n's left child is now y
   TEST_ASSERT_EQUAL_PTR(&y, n.left);
   TEST_ASSERT_EQUAL_PTR(&w, n.right);
   TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&n));
   TEST_ASSERT_EQUAL(1, avl_balance(&n));
}

static void test_avl_eject_left_child_leaf(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, w, x, u;

   /*
    * create structure
    *
    *       n
    *      / \
    *     c   w
    *    /     \
    *   x       u
    */

   root.node = &n;
   avl_setup_test_node(&n, &c, &w, NULL, 0);
   avl_setup_test_node(&c, &x, NULL,  &n, 1);
   avl_setup_test_node(&w, NULL, &u, &n, -1);

   avl_setup_test_node(&x, NULL, NULL, &c, 0);
   avl_setup_test_node(&u, NULL, NULL, &w, 0);

   struct avl_node *res = __avl_eject(&root, &c, &__avl_dummy_augment);

   TEST_ASSERT_EQUAL_PTR(res, &c);

   // Root unchanged
   TEST_ASSERT_EQUAL_PTR(&n, root.node);

   // x now replaces c
   TEST_ASSERT_EQUAL_PTR(NULL, x.left);
   TEST_ASSERT_EQUAL_PTR(NULL, x.right);
   TEST_ASSERT_EQUAL_PTR(&n, avl_parent(&x));
   TEST_ASSERT_EQUAL(0, avl_balance(&x));

   // n's left child is now x
   TEST_ASSERT_EQUAL_PTR(&x, n.left);
   TEST_ASSERT_EQUAL_PTR(&w, n.right);
   TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&n));
   TEST_ASSERT_EQUAL(-1, avl_balance(&n));
}

static void test_avl_eject_right_child_leaf(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, w, x, u;

   /*
    * create structure
    *
    *       n
    *      / \
    *     c   w
    *    /     \
    *   x       u
    */

   root.node = &n;
   avl_setup_test_node(&n, &c, &w, NULL, 0);
   avl_setup_test_node(&c, &x, NULL,  &n, 1);
   avl_setup_test_node(&w, NULL, &u, &n, -1);

   avl_setup_test_node(&x, NULL, NULL, &c, 0);
   avl_setup_test_node(&u, NULL, NULL, &w, 0);

   struct avl_node *res = __avl_eject(&root, &w, &__avl_dummy_augment);

   TEST_ASSERT_EQUAL_PTR(res, &u);

   // Root unchanged
   TEST_ASSERT_EQUAL_PTR(&n, root.node);

   // u now replaces w
   TEST_ASSERT_EQUAL_PTR(NULL, u.left);
   TEST_ASSERT_EQUAL_PTR(NULL, u.right);
   TEST_ASSERT_EQUAL_PTR(&n, avl_parent(&u));
   TEST_ASSERT_EQUAL(0, avl_balance(&u));

   // n's right child is now u
   TEST_ASSERT_EQUAL_PTR(&c, n.left);
   TEST_ASSERT_EQUAL_PTR(&u, n.right);
   TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(&n));
   TEST_ASSERT_EQUAL(1, avl_balance(&n));
}

static void test_avl_eject_leaf(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, w, x, u;

   /*
    * create structure
    *
    *       n
    *      / \
    *     c   w
    *    /     \
    *   x       u
    */

   root.node = &n;
   avl_setup_test_node(&n, &c, &w, NULL, 0);
   avl_setup_test_node(&c, &x, NULL,  &n, 1);
   avl_setup_test_node(&w, NULL, &u, &n, -1);

   avl_setup_test_node(&x, NULL, NULL, &c, 0);
   avl_setup_test_node(&u, NULL, NULL, &w, 0);

   struct avl_node *res;


   // --- TEST 1: eject (x) ---
   res = __avl_eject(&root, &x, &__avl_dummy_augment);

   TEST_ASSERT_EQUAL_PTR(res, &c);

   // Root unchanged
   TEST_ASSERT_EQUAL_PTR(&n, root.node);

   // c now leaf node
   TEST_ASSERT_EQUAL_PTR(NULL, c.left);
   TEST_ASSERT_EQUAL_PTR(NULL, c.right);
   TEST_ASSERT_EQUAL_PTR(&n, avl_parent(&c));
   TEST_ASSERT_EQUAL(0, avl_balance(&c));

   // ancestor balance changed
   TEST_ASSERT_EQUAL(-1, avl_balance(&n));


   // --- TEST 2: eject (u) ---
   res = __avl_eject(&root, &u, &__avl_dummy_augment);

   TEST_ASSERT_EQUAL_PTR(res, &w);

   // Root unchanged
   TEST_ASSERT_EQUAL_PTR(&n, root.node);

   // w now leaf node
   TEST_ASSERT_EQUAL_PTR(NULL, w.left);
   TEST_ASSERT_EQUAL_PTR(NULL, w.right);
   TEST_ASSERT_EQUAL_PTR(&n, avl_parent(&w));
   TEST_ASSERT_EQUAL(0, avl_balance(&w));

   // ancestor balance changed
   TEST_ASSERT_EQUAL(0, avl_balance(&n));
}


static void test_avl_eject_linked_2child_shallow_successor(void)
{
   struct avl_root_linked root = AVL_ROOT_LINKED_INIT;
   struct avl_node_linked n, c, g, w, x, y, z, u;

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
    *
    * inorder: x, c, y, g, z, n, w, u
    */

   root = (struct avl_root_linked) {
      .root = {
         .node = avl_linked(&n),
      },
      .first = &x,
   };

   avl_setup_test_node_linked(&n, &c, &w, NULL, 1, &z, &w);
   avl_setup_test_node_linked(&c, &x, &g,  &n, -1, &x, &y);
   avl_setup_test_node_linked(&w, NULL, &u, &n, -1, &n, &u);

   avl_setup_test_node_linked(&x, NULL, NULL, &c, 0, NULL, &c);
   avl_setup_test_node_linked(&u, NULL, NULL, &w, 0, &w, NULL);

   avl_setup_test_node_linked(&g, &y, &z, &c, 0, &y, &z);
   avl_setup_test_node_linked(&y, NULL, NULL, &g, 0, &c, &g);
   avl_setup_test_node_linked(&z, NULL, NULL, &g, 0, &g, &n);

   avl_eject_linked(&root, &g);

   // Root unchanged
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_linked(&root)->node);

   // ancestor's balance unchanged
   TEST_ASSERT_EQUAL( 1, avl_balance(avl_linked(&n)));
   TEST_ASSERT_EQUAL(-1, avl_balance(avl_linked(&c)));

   // z now replaces g
   TEST_ASSERT_EQUAL_PTR(avl_linked(&y), avl_linked(&z)->left);
   TEST_ASSERT_EQUAL_PTR(NULL,                avl_linked(&z)->right);
   TEST_ASSERT_EQUAL_PTR(avl_linked(&c), avl_parent(avl_linked(&z)));
   TEST_ASSERT_EQUAL(1, avl_balance(avl_linked(&z)));

   // y new parent is z
   TEST_ASSERT_EQUAL_PTR(avl_linked(&z), avl_parent(avl_linked(&y)));

   // c's right child is now z
   TEST_ASSERT_EQUAL_PTR(avl_linked(&x), avl_linked(&c)->left);
   TEST_ASSERT_EQUAL_PTR(avl_linked(&z), avl_linked(&c)->right);
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_parent(avl_linked(&c)));
   TEST_ASSERT_EQUAL(-1, avl_balance(avl_linked(&c)));
}

static void test_avl_eject_linked_2child_deep_successor(void)
{
   struct avl_root_linked root = AVL_ROOT_LINKED_INIT;
   struct avl_node_linked n, c, g, w, x, y, z, u;

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
    *
    * inorder: x, c, y, g, z, n, w, u
    */

   root = (struct avl_root_linked) {
      .root = {
         .node = avl_linked(&n),
      },
      .first = &x,
   };

   avl_setup_test_node_linked(&n, &c, &w, NULL, 1, &z, &w);
   avl_setup_test_node_linked(&c, &x, &g,  &n, -1, &x, &y);
   avl_setup_test_node_linked(&w, NULL, &u, &n, -1, &n, &u);

   avl_setup_test_node_linked(&x, NULL, NULL, &c, 0, NULL, &c);
   avl_setup_test_node_linked(&u, NULL, NULL, &w, 0, &w, NULL);

   avl_setup_test_node_linked(&g, &y, &z, &c, 0, &y, &z);
   avl_setup_test_node_linked(&y, NULL, NULL, &g, 0, &c, &g);
   avl_setup_test_node_linked(&z, NULL, NULL, &g, 0, &g, &n);

   avl_eject_linked(&root, &c);

   // Root unchanged
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_linked(&root)->node);

   // y now replaces c
   TEST_ASSERT_EQUAL_PTR(avl_linked(&x), avl_linked(&y)->left);
   TEST_ASSERT_EQUAL_PTR(avl_linked(&g), avl_linked(&y)->right);
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_parent(avl_linked(&y)));
   TEST_ASSERT_EQUAL(-1, avl_balance(avl_linked(&y)));

   // g new parent is y
   TEST_ASSERT_EQUAL_PTR(avl_linked(&y), avl_parent(avl_linked(&g)));
   TEST_ASSERT_EQUAL_PTR(NULL,                avl_linked(&g)->left);

   // n's left child is now y
   TEST_ASSERT_EQUAL_PTR(avl_linked(&y), avl_linked(&n)->left);
   TEST_ASSERT_EQUAL_PTR(avl_linked(&w), avl_linked(&n)->right);
   TEST_ASSERT_EQUAL_PTR(NULL,                avl_parent(avl_linked(&n)));
   TEST_ASSERT_EQUAL(1, avl_balance(avl_linked(&n)));
}

static void test_avl_eject_linked_left_child_leaf(void)
{
   struct avl_root_linked root = AVL_ROOT_LINKED_INIT;
   struct avl_node_linked n, c, w, x, u;

   /*
    * create structure
    *
    *       n
    *     /   \
    *   c       w
    *    \     /
    *     x   u
    *
    * inorder: c, x, n, u, w
    */

   root = (struct avl_root_linked) {
      .root = {
         .node = avl_linked(&n),
      },
      .first = &c,
   };

   avl_setup_test_node_linked(&n, &c, &w, NULL, 0, &x, &u);
   avl_setup_test_node_linked(&c, NULL, &x,  &n, -1, NULL, &x);
   avl_setup_test_node_linked(&w, &u, NULL, &n, 1, &u, NULL);

   avl_setup_test_node_linked(&x, NULL, NULL, &c, 0, &c, &n);
   avl_setup_test_node_linked(&u, NULL, NULL, &w, 0, &n, &w);

   avl_eject_linked(&root, &w);

   // Root unchanged
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_linked(&root)->node);

   // u now replaces w
   TEST_ASSERT_EQUAL_PTR(NULL, avl_linked(&u)->left);
   TEST_ASSERT_EQUAL_PTR(NULL, avl_linked(&u)->right);
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_parent(avl_linked(&u)));
   TEST_ASSERT_EQUAL(0, avl_balance(avl_linked(&u)));

   // n's right child is now u
   TEST_ASSERT_EQUAL_PTR(&c, avl_linked(&n)->left);
   TEST_ASSERT_EQUAL_PTR(&u, avl_linked(&n)->right);
   TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(avl_linked(&n)));
   TEST_ASSERT_EQUAL(1, avl_balance(avl_linked(&n)));
}

static void test_avl_eject_linked_right_child_leaf(void)
{
   struct avl_root_linked root = AVL_ROOT_LINKED_INIT;
   struct avl_node_linked n, c, w, x, u;

   /*
    * create structure
    *
    *       n
    *     /   \
    *   c       w
    *    \     /
    *     x   u
    *
    * inorder: c, x, n, u, w
    */

   root = (struct avl_root_linked) {
      .root = {
         .node = avl_linked(&n),
      },
      .first = &c,
   };

   avl_setup_test_node_linked(&n, &c, &w, NULL, 0, &x, &u);
   avl_setup_test_node_linked(&c, NULL, &x,  &n, -1, NULL, &x);
   avl_setup_test_node_linked(&w, &u, NULL, &n, 1, &u, NULL);

   avl_setup_test_node_linked(&x, NULL, NULL, &c, 0, &c, &n);
   avl_setup_test_node_linked(&u, NULL, NULL, &w, 0, &n, &w);

   avl_eject_linked(&root, &c);

   // Root unchanged, but 'first' changed
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_linked(&root)->node);
   TEST_ASSERT_EQUAL_PTR(&x, avl_linked(root.first));

   // x now replaces c
   TEST_ASSERT_EQUAL_PTR(NULL, avl_linked(&x)->left);
   TEST_ASSERT_EQUAL_PTR(NULL, avl_linked(&x)->right);
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_parent(avl_linked(&x)));
   TEST_ASSERT_EQUAL(0, avl_balance(avl_linked(&x)));

   // n's left child is now x
   TEST_ASSERT_EQUAL_PTR(&x, avl_linked(&n)->left);
   TEST_ASSERT_EQUAL_PTR(&w, avl_linked(&n)->right);
   TEST_ASSERT_EQUAL_PTR(NULL, avl_parent(avl_linked(&n)));
   TEST_ASSERT_EQUAL(-1, avl_balance(avl_linked(&n)));
}

static void test_avl_eject_linked_leaf(void)
{
   struct avl_root_linked root = AVL_ROOT_LINKED_INIT;
   struct avl_node_linked n, c, w, x, u;

   /*
    * create structure
    *
    *       n
    *      / \
    *     c   w
    *    /     \
    *   x       u
    *
    * inorder: x, c, n, w, u
    */

   root = (struct avl_root_linked) {
      .root = {
         .node = avl_linked(&n),
      },
      .first = &x,
   };

   avl_setup_test_node_linked(&n, &c, &w, NULL, 0, &c, &w);
   avl_setup_test_node_linked(&c, &x, NULL,  &n, 1, &x, &n);
   avl_setup_test_node_linked(&w, NULL, &u, &n, -1, &n, &u);

   avl_setup_test_node_linked(&x, NULL, NULL, &c, 0, NULL, &c);
   avl_setup_test_node_linked(&u, NULL, NULL, &w, 0, &w, NULL);


   // --- TEST 1: eject (x) ---
   avl_eject_linked(&root, &x);

   // Root unchanged, but 'first' changed
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_linked(&root)->node);
   TEST_ASSERT_EQUAL_PTR(&c, avl_linked(root.first));

   // c now leaf node
   TEST_ASSERT_EQUAL_PTR(NULL, avl_linked(&c)->left);
   TEST_ASSERT_EQUAL_PTR(NULL, avl_linked(&c)->right);
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_parent(avl_linked(&c)));
   TEST_ASSERT_EQUAL(0, avl_balance(avl_linked(&c)));

   // ancestor balance changed
   TEST_ASSERT_EQUAL(-1, avl_balance(avl_linked(&n)));


   // --- TEST 2: eject (u) ---
   avl_eject_linked(&root, &u);

   // Root unchanged
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_linked(&root)->node);

   // w now leaf node
   TEST_ASSERT_EQUAL_PTR(NULL, avl_linked(&w)->left);
   TEST_ASSERT_EQUAL_PTR(NULL, avl_linked(&w)->right);
   TEST_ASSERT_EQUAL_PTR(avl_linked(&n), avl_parent(avl_linked(&w)));
   TEST_ASSERT_EQUAL(0, avl_balance(avl_linked(&w)));

   // ancestor balance changed
   TEST_ASSERT_EQUAL(0, avl_balance(avl_linked(&n)));
}


static void test_avl_subtree_height(void)
{
#ifdef NDEBUG
   TEST_IGNORE_MESSAGE("function not available in NDEBUG");

#else
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y, z, u;
   (void)root;

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

   TEST_ASSERT_EQUAL(avl_verify_recursive(&n, &dummy_augment_verify), avl_subtree_height(&n));
   TEST_ASSERT_EQUAL(avl_verify_recursive(&c, &dummy_augment_verify), avl_subtree_height(&c));
   TEST_ASSERT_EQUAL(avl_verify_recursive(&g, &dummy_augment_verify), avl_subtree_height(&g));
   TEST_ASSERT_EQUAL(avl_verify_recursive(&w, &dummy_augment_verify), avl_subtree_height(&w));
   TEST_ASSERT_EQUAL(avl_verify_recursive(&x, &dummy_augment_verify), avl_subtree_height(&x));

#endif /* NDEBUG */
}

static void test_avl_subtree_size(void)
{
#ifdef NDEBUG
   TEST_IGNORE_MESSAGE("function not available in NDEBUG");

#else
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y, z, u;
   (void)root;

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

static void test_avl_verify(void)
{
#ifdef NDEBUG
   TEST_IGNORE_MESSAGE("function not available in NDEBUG");

#else
   struct avl_root root = AVL_ROOT_INIT;
   struct avl_node n, c, g, w, x, y, z, u;
   (void)root;

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

#ifndef AVL_AUGMENT_TEST
#define AVL_AUGMENT_TEST

#define AVL_AUGMENT_TEST_HELPER
#define AVL_AUGMENT_TEST_RAND_SIZE  100u
#define AVL_AUGMENT_TEST_RAND_OPS   300u
#define AVL_AUGMENT_TEST_RAND_SEED1 1672u
#define AVL_AUGMENT_TEST_RAND_SEED2 67420u

struct os_node
{
   struct avl_node avl;
   size_t size;
   int key;
};

static AVL_AUGMENT_TEST_HELPER void os_augment_update(struct avl_node *node)
{
   __huzlib_assert(node);
   struct os_node *osn = avl_entry(node, struct os_node, avl);

   size_t left_size = (node->left)
      ? avl_entry(node->left, struct os_node, avl)->size
      : 0;

   size_t right_size = (node->right)
      ? avl_entry(node->right, struct os_node, avl)->size
      : 0;

   osn->size = left_size + right_size + 1;
}

static AVL_AUGMENT_TEST_HELPER void os_augment_copy(struct avl_node *old, struct avl_node *new)
{
   __huzlib_assert(old && new);

   struct os_node *os_old = avl_entry(old, struct os_node, avl);
   struct os_node *os_new = avl_entry(new, struct os_node, avl);

   os_new->size = os_old->size;
}

static AVL_AUGMENT_TEST_HELPER void os_augment_rotate(struct avl_node *old, struct avl_node *new)
{
   os_augment_update(old);
   os_augment_update(new);
}

static AVL_AUGMENT_TEST_HELPER void os_augment_propagate(struct avl_node *node, struct avl_node *stop)
{
   while (node != stop)
   {
      os_augment_update(node);
      node = avl_parent(node);
   }
}

const struct avl_augment_callbacks os_callbacks = {
   .update    = os_augment_update,
   .copy      = os_augment_copy,
   .rotate    = os_augment_rotate,
   .propagate = os_augment_propagate,
};

static AVL_AUGMENT_TEST_HELPER void os_insert(struct avl_root *root, struct os_node *osn)
{
   __huzlib_assert(root && osn);

   struct avl_node **link = &root->node;
   struct avl_node *parent = NULL;
   bool isleft;

   while (*link)
   {
      parent = *link;
      int key = avl_entry(parent, struct os_node, avl)->key;

      if (osn->key < key)
      {
         link = &parent->left;
         isleft = true;
      }
      else
      {
         link = &parent->right;
         isleft = false;
      }
   }

   osn->size = 1;
   avl_link_node(&osn->avl, parent, link);
   avl_insert_rebalance_augmented(root, parent, isleft, &os_callbacks);
}

static AVL_AUGMENT_TEST_HELPER void os_eject(struct avl_root *root, struct os_node *osn)
{
   avl_eject_augmented(root, &osn->avl, &os_callbacks);
}

static AVL_AUGMENT_TEST_HELPER bool os_verify(struct avl_node *subroot)
{
   if (!subroot)
      return true;

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

   return size == avl_entry(subroot, struct os_node, avl)->size;
}

static void test_avl_augmented_insert_eject(void)
{
   struct avl_root root = AVL_ROOT_INIT;
   struct os_node nodes[AVL_AUGMENT_TEST_RAND_SIZE];
   bool intree[AVL_AUGMENT_TEST_RAND_SIZE] = {0};

   pcg32_random_t rng;
   pcg32_srandom_r(&rng, AVL_AUGMENT_TEST_RAND_SEED1, AVL_AUGMENT_TEST_RAND_SEED2);

   for (size_t i = 0; i < AVL_AUGMENT_TEST_RAND_SIZE; i++)
   {
      nodes[i].key = pcg32_random_r(&rng);
      avl_node_init(&nodes[i].avl);
   }

   for (size_t i = 0; i < AVL_AUGMENT_TEST_RAND_OPS; i++)
   {
      size_t k = pcg32_boundedrand_r(&rng, AVL_AUGMENT_TEST_RAND_SIZE);

      if (intree[k])
      {
         os_eject(&root, &nodes[k]);
         intree[k] = false;
      }
      else
      {
         os_insert(&root, &nodes[k]);
         intree[k] = true;
      }

      TEST_ASSERT_TRUE(avl_verify_recursive(root.node, &os_verify) != -1);
   }
}


struct os_node_linked
{
   struct avl_node_linked avl;
   size_t size;
   int key;
};

static AVL_AUGMENT_TEST_HELPER void os_linked_augment_update(struct avl_node *node)
{
   __huzlib_assert(node);

   struct os_node_linked *osn = avl_entry(avl_linked(node), struct os_node_linked, avl);
   size_t size = 1;

   if (node->left)
      size += avl_entry(avl_linked(node->left), struct os_node_linked, avl)->size;

   if (node->right)
      size += avl_entry(avl_linked(node->right), struct os_node_linked, avl)->size;

   osn->size = size;
}

static AVL_AUGMENT_TEST_HELPER void os_linked_augment_copy(struct avl_node *old, struct avl_node *new)
{
   __huzlib_assert(old && new);

   struct os_node_linked *os_old = avl_entry(avl_linked(old), struct os_node_linked, avl);
   struct os_node_linked *os_new = avl_entry(avl_linked(new), struct os_node_linked, avl);

   os_new->size = os_old->size;
}

static AVL_AUGMENT_TEST_HELPER void os_linked_augment_rotate(struct avl_node *old, struct avl_node *new)
{
   os_linked_augment_update(old);
   os_linked_augment_update(new);
}

static AVL_AUGMENT_TEST_HELPER void os_linked_augment_propagate(struct avl_node *node, struct avl_node *stop)
{
   while (node != stop)
   {
      os_linked_augment_update(node);
      node = avl_parent(node);
   }
}

const struct avl_augment_callbacks os_linked_callbacks = {
   .update    = os_linked_augment_update,
   .copy      = os_linked_augment_copy,
   .rotate    = os_linked_augment_rotate,
   .propagate = os_linked_augment_propagate,
};

static AVL_AUGMENT_TEST_HELPER void os_linked_insert(struct avl_root_linked *root, struct os_node_linked *osn)
{
   __huzlib_assert(root && osn);

   struct avl_node **link = &avl_linked(root)->node;
   struct avl_node_linked *parent = NULL;
   bool isleft = false, isfirst = true;

   while (*link)
   {
      parent = avl_linked(*link);
      int key = avl_entry(parent, struct os_node_linked, avl)->key;

      if (osn->key < key)
      {
         link = &avl_linked(parent)->left;
         isleft = true;
      }
      else
      {
         link = &avl_linked(parent)->right;
         isleft = false;
         isfirst = false;
      }
   }

   if (isfirst)
      root->first = &osn->avl;

   osn->size = 1;
   avl_link_node_linked(&osn->avl, parent, link, isleft);
   avl_insert_rebalance_augmented(
      avl_linked(root),
      avl_linked(parent),
      isleft,
      &os_linked_callbacks
   );
}

static AVL_AUGMENT_TEST_HELPER void os_linked_eject(struct avl_root_linked *root, struct os_node_linked *osn)
{
   avl_eject_linked_augmented(root, &osn->avl, &os_linked_callbacks);
}

static AVL_AUGMENT_TEST_HELPER bool os_linked_verify(struct avl_node *subroot)
{
   if (!subroot)
      return true;

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

   return size == avl_entry(avl_linked(subroot), struct os_node_linked, avl)->size;
}

static void test_avl_linked_augmented_insert_eject(void)
{
   struct avl_root_linked root = AVL_ROOT_LINKED_INIT;
   struct os_node_linked nodes[AVL_AUGMENT_TEST_RAND_SIZE];
   bool intree[AVL_AUGMENT_TEST_RAND_SIZE] = {0};

   pcg32_random_t rng;
   pcg32_srandom_r(&rng, AVL_AUGMENT_TEST_RAND_SEED1, AVL_AUGMENT_TEST_RAND_SEED2);

   for (size_t i = 0; i < AVL_AUGMENT_TEST_RAND_SIZE; i++)
   {
      nodes[i].key = pcg32_random_r(&rng);
      avl_node_linked_init(&nodes[i].avl);
   }

   for (size_t i = 0; i < AVL_AUGMENT_TEST_RAND_OPS; i++)
   {
      size_t k = pcg32_boundedrand_r(&rng, AVL_AUGMENT_TEST_RAND_SIZE);

      if (intree[k])
      {
         os_linked_eject(&root, &nodes[k]);
         intree[k] = false;
      }
      else
      {
         os_linked_insert(&root, &nodes[k]);
         intree[k] = true;
      }

      TEST_ASSERT_TRUE(avl_verify_recursive(avl_linked(&root)->node, &os_linked_verify) != -1);
   }
}

#undef AVL_AUGMENT_TEST_RAND_SEED1
#undef AVL_AUGMENT_TEST_RAND_SEED2
#undef AVL_AUGMENT_TEST_RAND_SIZE
#undef AVL_AUGMENT_TEST_RAND_OPS
#undef AVL_AUGMENT_TEST_HELPER

#endif /* AVL_AUGMENT_TEST */


int main(void)
{
   UnityBegin("test/avltree.h");

   RUN_TEST(test_avl_eject_rebalance_LL);
   RUN_TEST(test_avl_eject_rebalance_LR);
   RUN_TEST(test_avl_eject_rebalance_RR);
   RUN_TEST(test_avl_eject_rebalance_RL);

   RUN_TEST(test_avl_insert_rebalance_LL);
   RUN_TEST(test_avl_insert_rebalance_LR);
   RUN_TEST(test_avl_insert_rebalance_RR);
   RUN_TEST(test_avl_insert_rebalance_RL);

   RUN_TEST(test_avl_eject_2child_shallow_successor);
   RUN_TEST(test_avl_eject_2child_deep_successor);
   RUN_TEST(test_avl_eject_left_child_leaf);
   RUN_TEST(test_avl_eject_right_child_leaf);
   RUN_TEST(test_avl_eject_leaf);

   RUN_TEST(test_avl_eject_linked_2child_shallow_successor);
   RUN_TEST(test_avl_eject_linked_2child_deep_successor);
   RUN_TEST(test_avl_eject_linked_left_child_leaf);
   RUN_TEST(test_avl_eject_linked_right_child_leaf);
   RUN_TEST(test_avl_eject_linked_leaf);

   RUN_TEST(test_avl_subtree_height);
   RUN_TEST(test_avl_subtree_size);
   RUN_TEST(test_avl_verify);

   RUN_TEST(test_avl_augmented_insert_eject);
   RUN_TEST(test_avl_linked_augmented_insert_eject);

   // TODO: test avl_eject_first*_augmented varitants (x3)

   // TODO: test avl_eject_last_augmented (no need to test varitants)

   // TODO: test avl_subtree_first, avl_subtree_last, avl_subtree_postorder_first

   // TODO: test __avl_next, __avl_prev, __avl_preorder_next, __avl_postorder_first

   return UnityEnd();
}


#endif /* HUZLIB_AVL_TREE_TEST */


#endif /* HUZLIB_AVL_TREE_H */
