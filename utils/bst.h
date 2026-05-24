#ifndef HUZLIB_BST
#define HUZLIB_BST


#ifndef HUZLIB_BST_INCLUDES
#define HUZLIB_BST_INCLUDES

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



#ifndef HUZLIB_INTERNAL_HAS_TYPEOF_UNQUAL
#if (                                                                         \
   (defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER >= 20230000)) || \
   (defined(__ARMCOMPILER_VERSION) && (__ARMCOMPILER_VERSION >= 130000)) ||   \
   (defined(_MSC_VER) && (_MSC_VER >= 1938)) ||                               \
   (defined(__clang__) && (__clang_major__ >= 13)) ||                         \
   (defined(__GNUC__) && (__GNUC__ >= 11))                                    \
)
   #define HUZLIB_INTERNAL_HAS_TYPEOF_UNQUAL 1
#else
   #define HUZLIB_INTERNAL_HAS_TYPEOF_UNQUAL 0
#endif
#endif /* HUZLIB_INTERNAL_HAS_TYPEOF_UNQUAL */



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



#ifndef HUZLIB_INTERNAL_HAS_TYPES_COMPATIBLE
#if (                                  \
   defined(__INTEL_LLVM_COMPILER) ||   \
   defined(__INTEL_COMPILER) ||        \
   defined(__ARMCOMPILER_VERSION) ||   \
   defined(__zig__) ||                 \
   defined(__clang__) ||               \
   defined(__GNUC__)                   \
)
   #define HUZLIB_INTERNAL_HAS_TYPES_COMPATIBLE 1
#else
   #define HUZLIB_INTERNAL_HAS_TYPES_COMPATIBLE 0
#endif
#endif /* HUZLIB_INTERNAL_HAS_TYPES_COMPATIBLE */



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
 * typeof_member(type, member)
 * ---------------------------
 * Retrieves exact type of type->member
 */
#ifndef typeof_member
#define typeof_member(type, member) typeof(((type *)0)->member)
#endif /* typeof_member */



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


#endif /* HUZLIB_BST_INCLUDES */


/*
 * So I plan to have in this library about 3 specializations of BSTs namely
 *
 *    1. AVL tree
 *    2. Red-Black tree
 *    3. Splay tree
 *
 * Now these trees still share the same traversal logic which is not optimized by
 * their special properties. So in a nutshell, I may be writing 3 duplicates of the
 * same code with the only difference being the type of node (avl_node, rb_node, splay_node)
 *
 * So avoid this replication and worship the almighty DRY, We are going to create
 * an abstracted macro template for 'foreach' macros here.
 *
 * The goal is the same as before — DRY. Instead of writing avl_foreach,
 * rb_foreach, splay_foreach separately, we write one set of template macros
 * here and the tree implementations just wrap them.
 *
 * There are 4 template macros:
 *    1. __bst_foreach           - forward traversal
 *    2. __bst_foreach_safe      - same as (1) but safe to delete current node
 *    3. __bst_foreach_entry     - same as (1) but iterates the containing struct
 *    4. __bst_foreach_entry_safe- same as (3) but safe to delete current node
 *
 * Each of these 4 macros covers BOTH the whole-tree and subtree cases via
 * __bst_traverse, which uses _Generic to dispatch on the traverse function's
 * type:
 *
 *    __bst_fn(TYPE)         ->  TYPE *(*)(const TYPE *node)
 *    __bst_subtree_fn(TYPE) ->  TYPE *(*)(const TYPE *subroot, const TYPE *node)
 *
 * If traverse matches __bst_fn, __bst_traverse calls traverse(node).
 * If traverse matches __bst_subtree_fn, it calls traverse(subroot, node).
 * Anything else is a compile error — no matching _Generic branch.
 * This collapses what would have been 8 macros down to 4.
 *
 * The traversal order is determined entirely by which functions the caller
 * passes in as start and traverse. The macro itself is just a structural
 * shell — it knows nothing about order. For example:
 *
 *    avl_first(root) + avl_next                     -> inorder (ascending)
 *    avl_last(root)  + avl_prev                     -> inorder (descending)
 *    avl_root_node(root) + avl_preorder_next        -> preorder
 *    avl_postorder_first(root) + avl_postorder_next -> postorder
 *
 * So the ONE RING TO RULE THEM ALL philosophy applies twice here —
 * once across tree types (avl, rb, splay) and once across traversal
 * orders. One macro, infinite combinations.
 *
 * The _entry macros let the caller iterate directly over the containing struct
 * instead of the raw node, like list_for_each_entry in the linux kernel.
 * They use a small trick to avoid needing an extra tmp variable just to hold
 * the raw node pointer during the container_of conversion:
 *
 *    Step 1: store the raw node pointer into 'cur' via a cast. At this point
 *            'cur' is holding a node pointer disguised as a cur pointer —
 *            it is not a valid cur yet.
 *
 *    Step 2: NULL check 'cur' and if non-NULL, call container_of to convert
 *            it into a real cur pointer. Since the node member is at offset 0,
 *            container_of compiles down to a plain cast — zero overhead.
 *
 * This two step pattern is safe because we never dereference 'cur' between
 * step 1 and step 2 — we only use it as an opaque pointer for the NULL check.
 * The same pattern repeats in the increment, so the condition always sees
 * either NULL or a properly converted cur pointer.
 * 
 * The _entry macros also use a && chain in the loop condition to combine the
 * NULL check and container_of conversion without a ternary:
 *
 *    (cur) = (type *)(start);                                                <- init
 *    ((cur) != NULL) && ((cur) = container_of((void *)(cur), type, member)); <- condition
 *
 * The && short-circuits naturally: if cur is NULL the container_of is
 * skipped and the loop terminates. If cur is non-NULL, container_of runs
 * and cur becomes a valid container pointer before the loop body executes.
 *
 * The _safe variat uses this trick by hiding the pre-fetch of tmp 
 * inside the loop condition using || 1:
 *
 *    ((node) != NULL) && (((tmp) = traverse(node)) || 1);
 *
 * The && short-circuits if node is NULL — loop ends, tmp is never touched.
 * If node is non-NULL, traverse runs and stores the result in tmp. But
 * traverse may return NULL (end of traversal), which would make the
 * condition false and skip the loop body for the current node — wrong.
 *
 * The || 1 fixes this by forcing the condition true regardless of what
 * traverse returns, so the loop body always runs when node is non-NULL.
 *
 * The _entry_safe variant combines both of these tricks:
 *
 *    ((cur) != NULL)
 *       && ((cur) = container_of((void *)(cur), type, member))
 *       && (((tmp) = (type *)traverse(...)) || 1);
 *
 * container_of does not need the || 1 trick because it never returns NULL
 * — it is pure pointer arithmetic, so a non-NULL input always produces a
 * non-NULL output. The && just chains the pre-fetch of tmp cleanly onto
 * the same condition without breaking short-circuit behaviour.
 *
 * The _safe variants pre-fetch the next node into 'tmp' before the loop body
 * runs, so the caller can safely delete or modify 'cur' during iteration.
 *
 * The caller injects their own typechecks via __VA_ARGS__ which is placed at
 * the very start of the for-init clause as comma expressions. This is also why
 * these macros are not meant to be called directly
 */

#define __bst_fulltree_fn(TYPE)  TYPE *(*)(const TYPE *node)
#define __bst_subtree_fn(TYPE)   TYPE *(*)(const TYPE *subroot, const TYPE *node)

#define __bst_choose_fn(traverse, subroot, node) _Generic((subroot),                      \
   typeof(*(node)) *: ((__bst_subtree_fn(typeof(*(node))))  (traverse))(subroot, node),   \
   void *:            ((__bst_fulltree_fn(typeof(*(node)))) (traverse))(node)             \
)


#define __bst_foreach(node, subroot, start, traverse, ...)              \
   for (                                                                \
      __VA_ARGS__, /* caller injected typechecks */                     \
      typecheck(typeof(*(node)), *(start)),                             \
      (node) = (start);                                                 \
      (node) != NULL;                                                   \
      (node) = __bst_choose_fn(traverse, subroot, node)                 \
   )

#define __bst_foreach_safe(node, tmp, subroot, start, traverse, ...)    \
   for (                                                                \
      __VA_ARGS__, /* caller injected typechecks */                     \
      typecheck(typeof(*(node)), *(start)),                             \
      typecheck(typeof(*(node)), *(tmp)),                               \
      (node) = (start);                                                 \
      ((node) != NULL)                                                  \
         && (((tmp) = __bst_choose_fn(traverse, subroot, node)) || 1);  \
      (node) = (tmp)                                                    \
   )

#define __bst_foreach_entry(cur, subroot, type, member, start, traverse, ...)             \
   for (                                                                                  \
      __VA_ARGS__, /* caller injected typechecks */                                       \
      typecheck(type, *(cur)),                                                            \
      typecheck(typeof_member(type, member), *(start)),                                   \
      (cur) = (type *)(start);                                                            \
      ((cur) != NULL) && ((cur) = container_of((void *)(cur), type, member));             \
      (cur) = (type *)__bst_choose_fn(traverse, subroot, &(cur)->member)                  \
   )

#define __bst_foreach_entry_safe(cur, tmp, subroot, type, member, start, traverse, ...)   \
   for (                                                                                  \
      __VA_ARGS__, /* caller injected typechecks */                                       \
      typecheck(type, *(cur)),                                                            \
      typecheck(type, *(tmp)),                                                            \
      typecheck(typeof_member(type, member), *(start)),                                   \
      (cur) = (type *)(start);                                                            \
      ((cur) != NULL)                                                                     \
         && ((cur) = container_of((void *)(cur), type, member))                           \
         && (((tmp) = (type *)__bst_choose_fn(traverse, subroot, &(cur)->member)) || 1);  \
      (cur) = (tmp)                                                                       \
   )




#ifdef HUZLIB_BST_IMPL


/*
 * So I plan to have in this library about 3 specializations of BSTs namely
 *
 *    1. AVL tree
 *    2. Red-Black tree
 *    3. Splay tree
 *
 * Now these trees still share the same traversal logic which is not optimized by
 * their special properties. So in a nutshell, I may be writing 3 duplicates of the
 * same code with the only difference being the type of node (avl_node, rb_node, splay_node)
 *
 * So avoid this and worship the almighty DRY, we turn to linux/min_heap.h. (I know! sounds
 * ridiculous but bear with me). linux/min_heap.h uses a fake generic internal min_heap type
 * with
 *       TYPE = void / char
 *       CAPACITY = 0 / [] (flexible array member) 
 *
 * and creates a set of functions around that type. Then it uses macros to convert and 
 * reconvert from actual type to internal type. This essentially creates one set of functions 
 * for every type of min_heap (ONE RING TO RULE THEM ALL), reducing code size massively.
 *
 * Now after a little thinking, I realised I can apply it here! by creating an internal type named
 * 'struct bst_node', which will have the traversal functions written around it. and then the AVL/RB/splay
 * implementations will use these functions internally to do traversal (ofc they will wrap it)
 *
 * But there is a problem... the parent pointer of each node. Unlike a simple linked list where
 * the parent is just a plain pointer, each tree type hides extra information inside it. AVL trees
 * pack the balance factor into the low bits of the parent pointer. Red-Black trees pack the color
 * bit. So we cannot just read the parent field directly, each tree type needs its own small function
 * to extract the real parent pointer from the packed value.
 *
 * To solve this, bst_node stores the parent as a raw uintptr_t (an integer big enough to hold any
 * pointer) at offset 0, matching the layout of avl_node, rb_node and splay_node exactly. We then
 * pass a small __xxx_parent callback function to the traversal code which knows how to unpack the
 * parent for that specific tree type. Since these callbacks are force_inlined and always called with
 * a concrete known function, the compiler folds them away completely leaving zero overhead.
 * Converting between avl_node/rb_node/splay_node and bst_node is done safely via container_of
 * on the shared first member, avoiding any strict aliasing undefined behaviour.
 *
 * And we're not done here... we can also make internal template generators for traversal macros like
 * avl_foreach, avl_foreach_safe etc.. by having additional macro parameters for __bst_first, __bst_next
 * which the tree implementations will provide their wrapped functions for and more DRY is saved
 */

#include <stdint.h>
#include <stdbool.h>

struct bst_node
{
   uintptr_t __packed_parent;
   struct bst_node *left, *right;
};

struct bst_node_linked
{
   struct bst_node node;
   struct bst_node_linked *prev, *next;
};



#ifdef NDEBUG
   #define HUZLIB_BST_INTERNAL static __huzlib_inline__
#else
   #define HUZLIB_BST_INTERNAL static inline
#endif


/* ------------------------------------------------------------- */
/* --------------------- helper functions  --------------------- */
/* ------------------------------------------------------------- */

/*
 * __huzlib_bst_parent_ptr(root_ptr, node, parent, isleft)
 * -------------------------------------------------------
 * Returns the address of the pointer in 'parent' that points to 'node',
 * i.e. either &parent->left, &parent->right, or root_link if node is root.
 *
 * @root_link: the tree root's placeholder, not NULL
 * @node:      the node to find the parent pointer for, not NULL
 * @parent:    node->parent, may be NULL if node is the root
 * @isleft:    set true if node is parent's left child, false if right.
 *             value irrelavant if node is the root (parent == NULL).
 *
 * Return: address of the parent's child pointer that references node.
 */
HUZLIB_BST_INTERNAL struct bst_node **__huzlib_bst_parent_ptr(struct bst_node **restrict root_link, struct bst_node *restrict node, struct bst_node *restrict parent, bool *restrict isleft)
{
   __huzlib_assert(root_link && node && isleft);

   if (!parent)
   {
      /* isleft's value doesn't matter when node is root */
      return root_link;
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
 * __huzlib_bst_delink_node(child, parent, link, set_parent)
 * ---------------------------------------------------------
 * Replaces node 'n' in the tree by splicing 'child' into its position via
 * 'link', the parent's pointer that previously pointed to 'n'.
 *
 *    p                           p
 *     \                           \
 *      n          ==>       n     (c)
 *     / \                  /
 *   (o) (c)              (o)
 *
 * @child:      the node to splice in, may be NULL
 * @parent:     the parent of the node being removed, may be NULL
 * @link:       parent's internal pointer to delinking node, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Other child 'o' (if it exists) is left dangling.
 * Relinking 'o' is the caller's responsibility.
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_delink_node(struct bst_node *restrict child, struct bst_node *restrict parent, struct bst_node **restrict link, void (*set_parent)(struct bst_node *, struct bst_node *))
{
   __huzlib_assert(link && ((!parent) || (parent->left == *link) || (parent->right == *link)));
   __huzlib_assert(((*link)->left == child) || ((*link)->right == child));
   *link = child;
   if (child)
      set_parent(child, parent);
}

/*
 * __huzlib_bst_replace_node(old, new, link)
 * -----------------------------------------
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
 *
 * NOTE:
 * Will replace entire subtree on 'old' with subtree of 'new'
 * Therefore, children 'u' & 'v' (if exists) are left dangling.
 * Relinking is the caller's responsibility.
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_replace_node(struct bst_node *restrict old, struct bst_node *restrict new, struct bst_node **restrict link)
{
   __huzlib_assert(old && new && link && (*link == old) && augment_copy);
   *link = new;
   new->__packed_parent = old->__packed_parent;
}



/* ------------------------------------------------------------- */
/* --------------------- linked functions  --------------------- */
/* ------------------------------------------------------------- */

/*
 * __huzlib_bst_add_linked(new, prev, next)
 * ----------------------------------------
 * Inserts 'new' between 'prev' and 'next' in a doubly-linked list.
 *
 *      prev         next                 prev         new         next
 *       o --------- o                     o --------- o --------- o
 *
 * @new:   node to insert, not NULL
 * @prev:  predecessor node, may be NULL (insert at head)
 * @next:  successor node, may be NULL (insert at tail)
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_add_linked(struct bst_node_linked *restrict new, struct bst_node_linked *restrict prev, struct bst_node_linked *restrict next)
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
 * __huzlib_bst_rm_linked(prev, next)
 * ----------------------------------
 * Removes the node between 'prev' and 'next' by linking them directly.
 *
 *      prev         node         next                 prev         next
 *       o --------- o --------- o         ==>          o --------- o
 *
 * @prev:  predecessor of the node being removed, may be NULL
 * @next:  successor of the node being removed, may be NULL
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rm_linked(struct bst_node_linked *restrict prev, struct bst_node_linked *restrict next)
{
   if (__huzlib_likely__(prev))
      prev->next = next;

   if (__huzlib_likely__(next))
      next->prev = prev;
}



/* ------------------------------------------------------------- */
/* --------------------- rotate functions  --------------------- */
/* ------------------------------------------------------------- */

/*
 * __huzlib_bst_rotate_left(node, child, link, set_parent)
 * -------------------------------------------------------
 * Standard BST left rotation on 'node'
 *
 *      n                         c
 *     / \                       / \
 *   (x)  c         ==>         n  (z)
 *       / \                   / \
 *     (y) (z)               (x) (y)
 *
 * @node:       the node rotating on, not NULL
 * @child:      node's right child, not NULL
 * @link:       node->parent's internal pointer to node, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Does not update '__packed_parent' fields of 'node' and 'child'.
 * Caller must update them after rotation.
 *   eg: __avl_set_parent_balance(node, child, x);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_left(struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node **restrict link, void (*set_parent)(struct bst_node *, struct bst_node *))
{
   __huzlib_assert(node && child && link && (*link == node) && (node->right == child));

   if (child->left)
      set_parent(child->left, node);

   node->right = child->left;
   child->left = node;

   *link = child;
}

/*
 * __huzlib_bst_rotate_right(node, child, link, set_parent)
 * --------------------------------------------------------
 * Standard BST right rotation on 'node'
 *
 *        n                      c
 *       / \                    / \
 *      c  (x)      ==>       (z)  n
 *     / \                        / \
 *   (z) (y)                    (y) (x)
 *
 * @node:       the node rotating on, not NULL
 * @child:      node's left child, not NULL
 * @link:       node->parent's internal pointer to node, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Does not update '__packed_parent' fields of 'node' and 'child'.
 * Caller must update them after rotation.
 *    eg: __avl_set_parent_balance(node, child, x);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_right(struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node **restrict link, void (*set_parent)(struct bst_node *, struct bst_node *))
{
   __huzlib_assert(node && child && link && (*link == node) && (node->left == child));

   if (child->right)
      set_parent(child->right, node);

   node->left = child->right;
   child->right = node;

   *link = child;
}

/*
 * __huzlib_bst_rotate_left_left(node, child, parent, link, set_parent)
 * --------------------------------------------------------------------
 * Standard BST left rotation on 'parent' and then left rotation on 'node'
 *
 *     p                             n                             c
 *    / \                          /   \                          / \
 *  (w)  n                       p       c                       n  (z)
 *      / \          ==>        / \     / \        ==>          / \
 *    (x)  c                  (w) (x) (y) (z)                  p  (y)
 *        / \                                                 / \
 *      (y) (z)                                             (w) (x)
 *
 * @node:       the node rotating on, not NULL
 * @child:      node's right child, not NULL
 * @parent:     node's parent (node is always parent's right child), not NULL
 * @link:       parent->parent's internal pointer to parent, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Does not update '__packed_parent' fields of 'node', 'parent', and 'child'.
 * Caller must update them after rotation.
 *    eg: __splay_set_parent(node, child);
 *        __splay_set_parent(parent, node);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_left_left(struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node *restrict parent, struct bst_node **restrict link, void (*set_parent)(struct bst_node *, struct bst_node *))
{
   __huzlib_assert(node && child && parent && link && (*link == parent) && (node->right == child) && (parent->right == node));

   if (node->left) /* (x) */
      set_parent(node->left, parent);

   if (child->left) /* (y) */
      set_parent(child->left, node);

   parent->right = node->left;
   child->left = node;

   node->left = parent;
   node->right = child->left;

   *link = child;
}

/*
 * __huzlib_bst_rotate_right_right(node, child, parent, link, set_parent)
 * ----------------------------------------------------------------------
 * Standard BST right rotation on 'parent' and then right rotation on 'node'
 *
 *         p                         n                         c
 *        / \                      /   \                      / \
 *       n  (w)                  c       p                  (z)  n
 *      / \          ==>        / \     / \        ==>          / \
 *     c  (x)                 (z) (y) (x) (w)                 (y)  p
 *    / \                                                         / \
 *  (z) (y)                                                     (x) (w)
 *
 * @node:       the node rotating on, not NULL
 * @child:      node's left child, not NULL
 * @parent:     node's parent (node is always parent's left child), not NULL
 * @link:       parent->parent's internal pointer to parent, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Does not update '__packed_parent' fields of 'node', 'parent', and 'child'.
 * Caller must update them after rotation.
 *    eg: __splay_set_parent(node, child);
 *        __splay_set_parent(parent, node);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_right_right(struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node *restrict parent, struct bst_node **restrict link, void (*set_parent)(struct bst_node *, struct bst_node *))
{
   __huzlib_assert(node && child && parent && link && (*link == parent) && (node->left == child) && (parent->left == node));

   if (node->right) /* (x) */
      set_parent(node->right, parent);

   if (child->right) /* (y) */
      set_parent(child->right, node);

   parent->left = node->right;
   child->right = node;

   node->right = parent;
   node->left = child->right;

   *link = child;
}

/*
 * __huzlib_bst_rotate_left_right(node, child, parent, link, set_parent)
 * ---------------------------------------------------------------------
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
 * @node:       the node rotating on, not NULL
 * @child:      node's right child, not NULL
 * @parent:     node's parent (node is always parent's left child), not NULL
 * @link:       parent->parent's internal pointer to parent, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Does not update '__packed_parent' fields of 'node', 'parent', and 'child'.
 * Caller must update them after rotation.
 *    eg: __avl_set_parent_balance(node, child, x);
 *        __avl_set_parent_balance(parent, child, y);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_left_right(struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node *restrict parent, struct bst_node **restrict link, void (*set_parent)(struct bst_node *, struct bst_node *))
{
   __huzlib_assert(node && child && parent && link && (*link == parent) && (node->right == child) && (parent->left == node));

   if (child->left) /* (y) */
      set_parent(child->left, node);

   if (child->right) /* (z) */
      set_parent(child->right, parent);

   node->right = child->left;
   parent->left = child->right;

   child->left = node;
   child->right = parent;

   *link = child;
}

/*
 * __huzlib_bst_rotate_right_left(node, child, parent, link, set_parent)
 * ---------------------------------------------------------------------
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
 * @node:       the node rotating on, not NULL
 * @child:      node's left child, not NULL
 * @parent:     node's parent (node is always parent's right child), not NULL
 * @link:       parent->parent's internal pointer to parent, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Does not update '__packed_parent' fields of 'node', 'parent', and 'child'.
 * Caller must update them after rotation.
 *    eg: __avl_set_parent_balance(node, child, x);
 *        __avl_set_parent_balance(parent, child, y);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_right_left(struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node *restrict parent, struct bst_node **restrict link, void (*set_parent)(struct bst_node *, struct bst_node *))
{
   __huzlib_assert(node && child && parent && link && (*link == parent) && (node->left == child) && (parent->right == node));

   if (child->right) /* (y) */
      set_parent(child->right, node);

   if (child->left) /* (z) */
      set_parent(child->left, parent);

   node->left = child->right;
   parent->right = child->left;

   child->right = node;
   child->left = parent;

   *link = child;
}



/* ------------------------------------------------------------- */
/* --------------- full-tree / sub-tree queries  --------------- */
/* ------------------------------------------------------------- */

/*
 * __huzlib_bst_first(node)
 * ------------------------
 * first node in in-order traversal with subtree boundary.
 *
 * @node: sub-tree root node (root->node for full-tree)
 */
HUZLIB_BST_INTERNAL const struct bst_node *__huzlib_bst_first(const struct bst_node *restrict node)
{
   __huzlib_assert(node);
   while (node->left)
      node = node->left;
   return node;
}

/*
 * __huzlib_bst_last(node)
 * -----------------------
 * last node in in-order traversal with subtree boundary.
 *
 * @node: sub-tree root node (root->node for full-tree)
 */
HUZLIB_BST_INTERNAL const struct bst_node *__huzlib_bst_last(const struct bst_node *restrict node)
{
   __huzlib_assert(node);
   while (node->right)
      node = node->right;
   return node;
}

/*
 * __huzlib_bst_postorder_first(node)
 * ----------------------------------
 * first node in post-order traversal with subtree boundary.
 *
 * @node: sub-tree root node (root->node for full-tree)
 */
HUZLIB_BST_INTERNAL const struct bst_node *__huzlib_bst_postorder_first(const struct bst_node *restrict node)
{
   __huzlib_assert(node);

   node = __huzlib_bst_first(node);
   while (true)
   {
      if (node->left)
         node = node->left;

      else if (node->right)
         node = node->right;

      else
         return node;
   }
}


/* -------------------------------------------------------------- */
/* -------------- full-tree / sub-tree traversals  -------------- */
/* -------------------------------------------------------------- */

/*
 * __huzlib_bst_next(subroot_parent, node, get_parent)
 * ---------------------------------------------------
 * In-order successor with subtree boundary.
 *
 * @subroot_parent: sentinel node (NULL for full tree, bst_parent(subroot) for subtree)
 * @node:           current node
 * @get_parent:     parent getter
 *
 * Return: next node in in-order, or NULL if none
 *
 * NOTE:
 * The compiler folds bst_parent(subroot) → subroot when subroot is a dereferenced
 * pointer, making this zero-cost for both full-tree & sub-tree use cases.
 */
HUZLIB_BST_INTERNAL const struct bst_node *__huzlib_bst_next(const struct bst_node *restrict subroot_parent, const struct bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   __huzlib_assert(node && get_parent);

   if (node->right)
   {
      node = node->right;
      while (node->left)
         node = node->left;
      return node;
   }
   else
   {
      const struct bst_node *restrict parent = get_parent(node->__packed_parent);
      while (parent != subroot_parent && node == parent->right)
      {
         node = parent;
         parent = get_parent(parent->__packed_parent);
      }
      return (__huzlib_unlikely__(parent == subroot_parent)) ? NULL : parent;
   }
}

/*
 * __huzlib_bst_prev(subroot_parent, node, get_parent)
 * ---------------------------------------------------
 * In-order predecessor with subtree boundary.
 *
 * @subroot_parent: sentinel node (NULL for full tree, bst_parent(subroot) for subtree)
 * @node:           current node
 * @get_parent:     parent getter
 *
 * Return: prev node in in-order, or NULL if none
 *
 * NOTE:
 * The compiler folds bst_parent(subroot) → subroot when subroot is a dereferenced
 * pointer, making this zero-cost for both full-tree & sub-tree use cases.
 */
HUZLIB_BST_INTERNAL const struct bst_node *__huzlib_bst_prev(const struct bst_node *restrict subroot_parent, const struct bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   __huzlib_assert(node && get_parent);

   if (node->left)
   {
      node = node->left;
      while (node->right)
         node = node->right;
      return node;
   }
   else
   {
      const struct bst_node *restrict parent = get_parent(node->__packed_parent);
      while (parent != subroot_parent && node == parent->left)
      {
         node = parent;
         parent = get_parent(parent->__packed_parent);
      }
      return (__huzlib_unlikely__(parent == subroot_parent)) ? NULL : parent;
   }
}

/*
 * __huzlib_bst_preorder_next(subroot_parent, node, get_parent)
 * ------------------------------------------------------------
 * Pre-order successor with subtree boundary.
 *
 * @subroot_parent: sentinel node (NULL for full tree, bst_parent(subroot) for subtree)
 * @node:           current node
 * @get_parent:     parent getter
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
HUZLIB_BST_INTERNAL const struct bst_node *__huzlib_bst_preorder_next(const struct bst_node *restrict subroot_parent, const struct bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   __huzlib_assert(node && get_parent);

   if (node->left)
      return node->left;

   else if (node->right)
      return node->right;

   else
      while (true)
      {
         const struct bst_node *restrict parent = get_parent(node->__packed_parent);

         if (__huzlib_unlikely__(parent == subroot_parent))
            return NULL;

         else if (node == parent->left && parent->right)
            return parent->right;

         else
            node = parent;
      }
}

/*
 * __huzlib_bst_postorder_next(subroot_parent, node, get_parent)
 * -------------------------------------------------------------
 * Post-order successor with subtree boundary.
 *
 * @subroot_parent: sentinel node (NULL for full tree, bst_parent(subroot) for subtree)
 * @node:           current node
 * @get_parent:     parent getter
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
HUZLIB_BST_INTERNAL const struct bst_node *__huzlib_bst_postorder_next(const struct bst_node *restrict subroot_parent, const struct bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   __huzlib_assert(node && get_parent);
   const struct bst_node *restrict parent = get_parent(node->__packed_parent);

   if (__huzlib_unlikely__(parent == subroot_parent))
      return NULL;

   else if (node == parent->left && parent->right)
      return __huzlib_bst_postorder_first(parent->right);

   else
      return parent;
}




#define bst_node_cast(node, parent_member)         container_of(&(node)->parent_member, struct bst_node, __packed_parent)
#define bst_node_recast(node, type, parent_member) container_of(&(node)->__packed_parent, type, parent_member)

#define bst_node_linked_cast(_node, parent_member)          container_of(bst_node_cast(&(_node)->node, parent_member), struct bst_node_linked, node)
#define bst_node_linked_recast(_node, type, parent_member)  container_of(bst_node_recast(&(_node)->node, typeof_member(type, node), parent_member), type, node)


#define __bst_parent_ptr(root_link, node, parent, isleft, __parent_member) \
   ((typeof(*(node)) **)__huzlib_bst_parent_ptr(                           \
      (struct bst_node **)(root_link),                                     \
      bst_node_cast(node, __parent_member),                                \
      bst_node_cast(parent, __parent_member),                              \
      isleft                                                               \
   ))

#define __bst_delink_node(child, parent, link, set_parent, __parent_member)   \
   __huzlib_bst_delink_node(                                                  \
      bst_node_cast(child, __parent_member),                                  \
      bst_node_cast(parent, __parent_member),                                 \
      (struct bst_node **)(link),                                             \
      set_parent                                                              \
   )

#define __bst_replace_node(old, new, link, __parent_member) \
   __huzlib_bst_replace_node(                               \
      bst_node_cast(old, __parent_member),                  \
      bst_node_cast(new, __parent_member),                  \
      (struct bst_node **)(link)                            \
   )

#define __bst_add_linked(new, prev, next, __parent_member)  \
   __huzlib_bst_add_linked(                                 \
      bst_node_linked_cast(new, __parent_member),           \
      bst_node_linked_cast(prev, __parent_member),          \
      bst_node_linked_cast(next, __parent_member)           \
   )

#define __bst_rm_linked(prev, next, __parent_member)        \
   __huzlib_bst_rm_linked(                                  \
      bst_node_linked_cast(prev, __parent_member),          \
      bst_node_linked_cast(next, __parent_member)           \
   )

#define __bst_rotate_left(node, child, link, set_parent, __parent_member)  \
   __huzlib_bst_rotate_left(                                               \
      bst_node_cast(node, __parent_member),                                \
      bst_node_cast(child, __parent_member),                               \
      (struct bst_node **)(link),                                          \
      set_parent                                                           \
   )

#define __bst_rotate_right(node, child, link, set_parent, __parent_member) \
   __huzlib_bst_rotate_right(                                              \
      bst_node_cast(node, __parent_member),                                \
      bst_node_cast(child, __parent_member),                               \
      (struct bst_node **)(link),                                          \
      set_parent                                                           \
   )

#define __bst_rotate_left_left(node, child, parent, link, set_parent, __parent_member)    \
   __huzlib_bst_rotate_left_left(                                                         \
      bst_node_cast(node, __parent_member),                                               \
      bst_node_cast(child, __parent_member),                                              \
      bst_node_cast(parent, __parent_member),                                             \
      (struct bst_node **)(link),                                                         \
      set_parent                                                                          \
   )

#define __bst_rotate_right_right(node, child, parent, link, set_parent, __parent_member)  \
   __huzlib_bst_rotate_right_right(                                                       \
      bst_node_cast(node, __parent_member),                                               \
      bst_node_cast(child, __parent_member),                                              \
      bst_node_cast(parent, __parent_member),                                             \
      (struct bst_node **)(link),                                                         \
      set_parent                                                                          \
   )


#define __bst_rotate_left_right(node, child, parent, link, set_parent, __parent_member)   \
   __huzlib_bst_rotate_left_right(                                                        \
      bst_node_cast(node, __parent_member),                                               \
      bst_node_cast(child, __parent_member),                                              \
      bst_node_cast(parent, __parent_member),                                             \
      (struct bst_node **)(link),                                                         \
      set_parent                                                                          \
   )

#define __bst_rotate_right_left(node, child, parent, link, set_parent, __parent_member)   \
   __huzlib_bst_rotate_right_left(                                                        \
      bst_node_cast(node, __parent_member),                                               \
      bst_node_cast(child, __parent_member),                                              \
      bst_node_cast(parent, __parent_member),                                             \
      (struct bst_node **)(link),                                                         \
      set_parent                                                                          \
   )

#define __bst_first(node, __parent_member)      \
   bst_node_recast(                             \
      __huzlib_bst_first(                       \
         bst_node_cast(node, __parent_member)   \
      ),                                        \
      typeof(*(node)),                          \
      __parent_member                           \
   )

#define __bst_last(node, __parent_member)       \
   bst_node_recast(                             \
      __huzlib_bst_last(                        \
         bst_node_cast(node, __parent_member)   \
      ),                                        \
      typeof(*(node)),                          \
      __parent_member                           \
   )

#define __bst_postorder_first(node, __parent_member)  \
   bst_node_recast(                                   \
      __huzlib_bst_postorder_first(                   \
         bst_node_cast(node, __parent_member)         \
      ),                                              \
      typeof(*(node)),                                \
      __parent_member                                 \
   )

#define __bst_next(subroot_parent, node, get_parent, __parent_member)            \
   bst_node_recast(                                                              \
      __huzlib_bst_next(                                                         \
         bst_node_cast((typeof((node)))(subroot_parent), __parent_member),       \
         bst_node_cast(node, __parent_member),                                   \
         get_parent                                                              \
      ),                                                                         \
      typeof(*(node)),                                                           \
      __parent_member                                                            \
   )

#define __bst_prev(subroot_parent, node, get_parent, __parent_member)            \
   bst_node_recast(                                                              \
      __huzlib_bst_prev(                                                         \
         bst_node_cast((typeof((node)))(subroot_parent), __parent_member),       \
         bst_node_cast(node, __parent_member),                                   \
         get_parent                                                              \
      ),                                                                         \
      typeof(*(node)),                                                           \
      __parent_member                                                            \
   )

#define __bst_preorder_next(subroot_parent, node, get_parent, __parent_member)   \
   bst_node_recast(                                                              \
      __huzlib_bst_preorder_next(                                                \
         bst_node_cast((typeof((node)))(subroot_parent), __parent_member),       \
         bst_node_cast(node, __parent_member),                                   \
         get_parent                                                              \
      ),                                                                         \
      typeof(*(node)),                                                           \
      __parent_member                                                            \
   )

#define __bst_postorder_next(subroot_parent, node, get_parent, __parent_member)  \
   bst_node_recast(                                                              \
      __huzlib_bst_postorder_next(                                               \
         bst_node_cast((typeof((node)))(subroot_parent), __parent_member),       \
         bst_node_cast(node, __parent_member),                                   \
         get_parent                                                              \
      ),                                                                         \
      typeof(*(node)),                                                           \
      __parent_member                                                            \
   )


#endif /* HUZLIB_BST_IMPL */


#ifdef HUZLIB_BST_TEST

#include "unity.h"
#include "pcg_basic.h"

struct test_node
{
   uintptr_t __parent;
   struct test_node *left, *right;
};

struct test_node_linked
{
   struct test_node node;
   struct test_node_linked *prev, *next;
};


static void *test_node_get_parent(const uintptr_t __parent)
{
   __huzlib_assert(node);
   return (void *)__parent;
}

static void test_node_set_parent(struct test_node *node, struct test_node *parent)
{
   __huzlib_assert(node);
   node->__parent = (uintptr_t)parent;
}

static void test_node_set_parent_bst(struct bst_node *node, struct bst_node *parent)
{
   test_node_set_parent(
      bst_node_recast(node, struct test_node, __parent),
      bst_node_recast(parent, struct test_node, __parent)
   );
}

/*
 * bst_setup_test_node(node, left, right, parent)
 * ----------------------------------------------
 * setups 'node' for testing purposes with children 'left' & 'right' and, parent 'parent
 *
 * @node:    test node
 * @left:    left child tobe set
 * @right:   right child tobe set
 * @parent:  parent tobe set
 */
static void bst_setup_test_node(struct test_node *node, struct test_node *left, struct test_node *right, struct test_node *parent)
{
   __huzlib_assert(node);
   node->left = left;
   node->right = right;
   test_node_set_parent(node, parent);
}

/*
 * bst_setup_test_node_linked(node, left, right, parent, prev, next)
 * --------------------------------------------------------------------------
 * setups 'node' for testing purposes with children 'left' & 'right', parent 'parent',
 * and in-order 'prev' and 'next'
 *
 * @node:    test node
 * @left:    left child tobe set
 * @right:   right child tobe set
 * @parent:  parent tobe set
 * @prev:    inorder predecessor
 * @next:    inorder successor
 */
static void bst_setup_test_node_linked(struct test_node_linked *node, struct test_node_linked *left, struct test_node_linked *right, struct test_node_linked *parent, struct test_node_linked *prev, struct test_node_linked *next)
{
   __huzlib_assert(node);
   bst_setup_test_node(
      &node->node,
      &left->node,
      &right->node,
      &parent->node
   );
   node->next = next;
   node->prev = prev;
}


void setUp() {}
void tearDown() {}


static void test_bst_parent_ptr(void)
{
   struct test_node node, parent, *root, **res;
   bool isleft;


   // CASE 1: node is root
   bst_setup_test_node(&node, NULL, NULL, NULL);

   res = __bst_parent_ptr(&root, &node, (struct test_node *)NULL, &isleft, __parent);
   TEST_ASSERT_EQUAL(res, &root);


   // CASE 2: node is left child
   bst_setup_test_node(&node, NULL, NULL, &parent);
   bst_setup_test_node(&parent, &node, NULL, NULL);

   res = __bst_parent_ptr(&root, &node, &parent, &isleft, __parent);
   TEST_ASSERT_EQUAL(res, &parent.left);
   TEST_ASSERT_TRUE(isleft);


   // CASE 3: node is right child
   bst_setup_test_node(&node, NULL, NULL, &parent);
   bst_setup_test_node(&parent, NULL, &node, NULL);

   res = __bst_parent_ptr(&root, &node, &parent, &isleft, __parent);
   TEST_ASSERT_EQUAL(res, &parent.right);
   TEST_ASSERT_FALSE(isleft);
}

static void test_bst_delink_node(void)
{
   struct test_node node, parent, child, t, u, g;

   /*
    *      g
    *     /
    *    p
    *     \
    *      n
    *     /
    *    c
    *   / \
    *  t   u
    */
   bst_setup_test_node(&g, &parent, NULL, NULL);
   bst_setup_test_node(&parent, NULL, &node, &g);
   bst_setup_test_node(&node, &t, &u, &parent);
   bst_setup_test_node(&t, NULL, NULL, &node);
   bst_setup_test_node(&u, NULL, NULL, &node);

   __bst_delink_node(&child, &parent, &parent.right, test_node_set_parent_bst, __parent);

   TEST_ASSERT_EQUAL(parent.right, &child);
   TEST_ASSERT_EQUAL(test_node_get_parent((uintptr_t)child.__parent), &parent);
}

static void test_bst_replace_node(void)
{
   struct test_node node, succ, parent, tmp, w;


   /*
    * CASE 1:
    *
    *   p
    *    \
    *     n
    *    / \
    *  (w)  t
    *      /
    *     s
    */
   bst_setup_test_node(&parent, NULL, &node, NULL);
   bst_setup_test_node(&node, &w, &tmp, &parent);
   bst_setup_test_node(&w, NULL, NULL, &node);
   bst_setup_test_node(&tmp, &succ, NULL, &node);
   bst_setup_test_node(&succ, NULL, NULL, &tmp);

   __bst_replace_node(&node, &succ, &parent.right, __parent);

   TEST_ASSERT_EQUAL(test_node_get_parent((uintptr_t)succ.__parent), &parent);
   TEST_ASSERT_EQUAL(parent.right, &succ);


   /*
    * CASE 2:
    *
    *   p
    *    \
    *     n
    *    / \
    *  (w)  s
    *        \
    *         t
    */
   bst_setup_test_node(&parent, NULL, &node, NULL);
   bst_setup_test_node(&node, &w, &tmp, &parent);
   bst_setup_test_node(&w, NULL, NULL, &node);
   bst_setup_test_node(&succ, NULL, &tmp, &node);
   bst_setup_test_node(&tmp, NULL, NULL, &succ);

   __bst_replace_node(&node, &succ, &parent.right, __parent);

   TEST_ASSERT_EQUAL(test_node_get_parent((uintptr_t)succ.__parent), &parent);
   TEST_ASSERT_EQUAL(parent.right, &succ);
   TEST_ASSERT_EQUAL(succ.right, &tmp);
}

static void test_bst_add_rm_linked(void)
{
   struct test_node_linked a, b, n;

   /*
    * TEST ADD:
    *
    *   a -- b
    */
   bst_setup_test_node_linked(&a, NULL, NULL, NULL, NULL, &b);
   bst_setup_test_node_linked(&b, NULL, NULL, NULL, &a, NULL);
   bst_setup_test_node_linked(&n, NULL, NULL, NULL, NULL, NULL);

   __bst_add_linked(&n, &a, &b, __parent);

   TEST_ASSERT_EQUAL(n.prev, &a);
   TEST_ASSERT_EQUAL(n.next, &b);

   TEST_ASSERT_EQUAL(a.next, &n);
   TEST_ASSERT_EQUAL(a.prev, NULL);

   TEST_ASSERT_EQUAL(b.next, NULL);
   TEST_ASSERT_EQUAL(b.prev, &n);

   /*
    * TEST RM:
    *
    *  a -- n -- b
    */

   __bst_rm_linked(&a, &b, __parent);

   TEST_ASSERT_EQUAL(a.next, &b);
   TEST_ASSERT_EQUAL(a.prev, NULL);

   TEST_ASSERT_EQUAL(b.next, NULL);
   TEST_ASSERT_EQUAL(b.prev, &a);
}

#ifndef TMP_GUARD
#define TMP_GUARD

// static void test_bst_rotate_left(void)
// {
//     /*
//      *        p               p
//      *         \               \
//      *          n               c
//      *         / \             / \
//      *        a   c     =>    n   z
//      *           / \         / \
//      *          b   z       a   b
//      */
//     struct test_node p, n, c, a, b, z;
//     bst_setup_test_node(&p, NULL, &n, NULL);
//     bst_setup_test_node(&n, &a, &c, &p);
//     bst_setup_test_node(&c, &b, &z, &n);
//     bst_setup_test_node(&a, NULL, NULL, &n);
//     bst_setup_test_node(&b, NULL, NULL, &c);
//     bst_setup_test_node(&z, NULL, NULL, &c);
//
//     __bst_rotate_left(&n, &c, &p.right, test_node_set_parent_bst, __parent);
//
//     /* Check parent pointers */
//     TEST_ASSERT_EQUAL(test_node_get_parent(&c), &p);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&n), &c);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&a), &n);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&b), &n);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&z), &c);
//
//     /* Check child pointers */
//     TEST_ASSERT_EQUAL(p.right, &c);
//     TEST_ASSERT_EQUAL(c.left, &n);
//     TEST_ASSERT_EQUAL(c.right, &z);
//     TEST_ASSERT_EQUAL(n.left, &a);
//     TEST_ASSERT_EQUAL(n.right, &b);
// }

// static void test_bst_rotate_right(void)
// {
//     /*
//      *          p           p
//      *         /           /
//      *        n           c
//      *       / \         / \
//      *      c   a   =>  z   n
//      *     / \             / \
//      *    z   b           b   a
//      */
//     struct test_node p, n, c, a, b, z;
//     bst_setup_test_node(&p, &n, NULL, NULL);
//     bst_setup_test_node(&n, &c, &a, &p);
//     bst_setup_test_node(&c, &z, &b, &n);
//     bst_setup_test_node(&a, NULL, NULL, &n);
//     bst_setup_test_node(&b, NULL, NULL, &c);
//     bst_setup_test_node(&z, NULL, NULL, &c);
//
//     __bst_rotate_right(&n, &c, &p.left, test_node_set_parent_bst, __parent);
//
//     TEST_ASSERT_EQUAL(test_node_get_parent(&c), &p);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&n), &c);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&a), &n);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&b), &n);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&z), &c);
//
//     TEST_ASSERT_EQUAL(p.left, &c);
//     TEST_ASSERT_EQUAL(c.left, &z);
//     TEST_ASSERT_EQUAL(c.right, &n);
//     TEST_ASSERT_EQUAL(n.left, &b);
//     TEST_ASSERT_EQUAL(n.right, &a);
// }
//
// static void test_bst_rotate_left_left(void)
// {
//     /*
//      *        g               g
//      *       /               /
//      *      p               c
//      *       \             / \
//      *        n     =>    n   z
//      *         \         / \
//      *          c       p   y
//      *         / \       \
//      *        y   z       x
//      */
//     struct test_node g, p, n, c, x, y, z;
//     bst_setup_test_node(&g, &p, NULL, NULL);
//     bst_setup_test_node(&p, NULL, &n, &g);
//     bst_setup_test_node(&n, &x, &c, &p);
//     bst_setup_test_node(&c, &y, &z, &n);
//     bst_setup_test_node(&x, NULL, NULL, &n);
//     bst_setup_test_node(&y, NULL, NULL, &c);
//     bst_setup_test_node(&z, NULL, NULL, &c);
//
//     __bst_rotate_left_left(&n, &c, &p, &g.left, test_node_set_parent_bst, __parent);
//
//     TEST_ASSERT_EQUAL(test_node_get_parent(&c), &g);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&n), &c);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&p), &n);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&x), &p);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&y), &n);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&z), &c);
//
//     TEST_ASSERT_EQUAL(g.left, &c);
//     TEST_ASSERT_EQUAL(c.left, &n);
//     TEST_ASSERT_EQUAL(c.right, &z);
//     TEST_ASSERT_EQUAL(n.left, &p);
//     TEST_ASSERT_EQUAL(n.right, &y);
//     TEST_ASSERT_EQUAL(p.right, &x);
// }
//
// static void test_bst_rotate_right_right(void)
// {
//     /*
//      *        g               g
//      *         \               \
//      *          p               c
//      *         /               / \
//      *        n       =>      y   n
//      *       /                   / \
//      *      c                   x   p
//      *     / \                 /
//      *    y   z               z
//      */
//     struct test_node g, p, n, c, x, y, z;
//     bst_setup_test_node(&g, NULL, &p, NULL);
//     bst_setup_test_node(&p, &n, NULL, &g);
//     bst_setup_test_node(&n, &c, &x, &p);
//     bst_setup_test_node(&c, &y, &z, &n);
//     bst_setup_test_node(&x, NULL, NULL, &n);
//     bst_setup_test_node(&y, NULL, NULL, &c);
//     bst_setup_test_node(&z, NULL, NULL, &c);
//
//     __bst_rotate_right_right(&n, &c, &p, &g.right, test_node_set_parent_bst, __parent);
//
//     TEST_ASSERT_EQUAL(test_node_get_parent(&c), &g);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&n), &c);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&p), &n);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&x), &p);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&y), &c);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&z), &n);
//
//     TEST_ASSERT_EQUAL(g.right, &c);
//     TEST_ASSERT_EQUAL(c.left, &y);
//     TEST_ASSERT_EQUAL(c.right, &n);
//     TEST_ASSERT_EQUAL(n.left, &z);
//     TEST_ASSERT_EQUAL(n.right, &p);
//     TEST_ASSERT_EQUAL(p.left, &x);
// }
//
// static void test_bst_rotate_left_right(void)
// {
//     /*
//      *        g               g
//      *       /               /
//      *      p               c
//      *     /               / \
//      *    n       =>      n   p
//      *     \             / \   \
//      *      c           x   y   z
//      *     / \
//      *    x   y
//      *         \
//      *          z
//      */
//     struct test_node g, p, n, c, x, y, z;
//     bst_setup_test_node(&g, &p, NULL, NULL);
//     bst_setup_test_node(&p, &n, NULL, &g);
//     bst_setup_test_node(&n, NULL, &c, &p);
//     bst_setup_test_node(&c, &x, &y, &n);
//     bst_setup_test_node(&x, NULL, NULL, &c);
//     bst_setup_test_node(&y, NULL, &z, &c);
//     bst_setup_test_node(&z, NULL, NULL, &y);
//
//     __bst_rotate_left_right(&n, &c, &p, &g.left, test_node_set_parent_bst, __parent);
//
//     TEST_ASSERT_EQUAL(test_node_get_parent(&c), &g);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&n), &c);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&p), &c);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&x), &n);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&y), &n);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&z), &p);
//
//     TEST_ASSERT_EQUAL(g.left, &c);
//     TEST_ASSERT_EQUAL(c.left, &n);
//     TEST_ASSERT_EQUAL(c.right, &p);
//     TEST_ASSERT_EQUAL(n.left, &x);
//     TEST_ASSERT_EQUAL(n.right, &y);
//     TEST_ASSERT_EQUAL(p.right, &z);
// }
//
// static void test_bst_rotate_right_left(void)
// {
//     /*
//      *        g               g
//      *         \               \
//      *          p               c
//      *           \             / \
//      *            n    =>     p   n
//      *           /           /   /
//      *          c           x   y
//      *         / \             /
//      *        x   y           z
//      *           /
//      *          z
//      */
//     struct test_node g, p, n, c, x, y, z;
//     bst_setup_test_node(&g, NULL, &p, NULL);
//     bst_setup_test_node(&p, NULL, &n, &g);
//     bst_setup_test_node(&n, &c, NULL, &p);
//     bst_setup_test_node(&c, &x, &y, &n);
//     bst_setup_test_node(&x, NULL, NULL, &c);
//     bst_setup_test_node(&y, &z, NULL, &c);
//     bst_setup_test_node(&z, NULL, NULL, &y);
//
//     __bst_rotate_right_left(&n, &c, &p, &g.right, test_node_set_parent_bst, __parent);
//
//     TEST_ASSERT_EQUAL(test_node_get_parent(&c), &g);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&p), &c);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&n), &c);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&x), &p);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&y), &n);
//     TEST_ASSERT_EQUAL(test_node_get_parent(&z), &y);
//
//     TEST_ASSERT_EQUAL(g.right, &c);
//     TEST_ASSERT_EQUAL(c.left, &p);
//     TEST_ASSERT_EQUAL(c.right, &n);
//     TEST_ASSERT_EQUAL(p.left, &x);
//     TEST_ASSERT_EQUAL(n.left, &y);
//     TEST_ASSERT_EQUAL(y.left, &z);
// }

#endif /* TMP_GUARD */

static void test_bst_first_last_postorder_first(void)
{
   /*
    * Tree:
    *           7
    *         /   \
    *       5       8
    *     /   \       \
    *    1     6       9
    *     \
    *      3
    *     / \
    *    2   4
    */
   struct test_node n1, n2, n3, n4, n5, n6, n7, n8, n9;

   bst_setup_test_node(&n7, &n5, &n8, NULL);
   bst_setup_test_node(&n5, &n1, &n6, &n7);
   bst_setup_test_node(&n8, NULL, &n9, &n7);
   bst_setup_test_node(&n1, NULL, &n3, &n5);
   bst_setup_test_node(&n6, NULL, NULL, &n5);
   bst_setup_test_node(&n9, NULL, NULL, &n8);
   bst_setup_test_node(&n3, &n2, &n4, &n1);
   bst_setup_test_node(&n2, NULL, NULL, &n3);
   bst_setup_test_node(&n4, NULL, NULL, &n3);

   const struct test_node *res;

   res = __bst_first(&n7, __parent);
   TEST_ASSERT_EQUAL(res, &n1);

   res = __bst_last(&n7, __parent);
   TEST_ASSERT_EQUAL(res, &n9);

   res = __bst_postorder_first(&n7, __parent);
   TEST_ASSERT_EQUAL(res, &n2);
}

static void test_bst_next_prev(void)
{
   #define SIZE 17
   struct test_node n[SIZE];

   /*
    *                  10
    *             /          \
    *           8            13
    *        /    \        /    \
    *       4      9     11      14
    *     /  \             \       \
    *    0    5             12      16
    *     \    \                    /
    *     2     7                 15
    *    / \   /
    *   1  3  6
    */
   bst_setup_test_node(n + 10, n + 8,  n + 13, NULL);
   bst_setup_test_node(n + 8,  n + 4,  n + 9,  n + 10);
   bst_setup_test_node(n + 13, n + 11, n + 14, n + 10);
   bst_setup_test_node(n + 4,  n + 0,  n + 5,  n + 8);
   bst_setup_test_node(n + 9,  NULL,   NULL,   n + 8);
   bst_setup_test_node(n + 11, NULL,   n + 12, n + 13);
   bst_setup_test_node(n + 14, NULL,   n + 16, n + 13);
   bst_setup_test_node(n + 0,  NULL,   n + 2,  n + 4);
   bst_setup_test_node(n + 5,  NULL,   n + 7,  n + 4);
   bst_setup_test_node(n + 12, NULL,   NULL,   n + 11);
   bst_setup_test_node(n + 16, n + 15, NULL,   n + 14);
   bst_setup_test_node(n + 2,  n + 1,  n + 3,  n + 0);
   bst_setup_test_node(n + 7,  n + 6,  NULL,   n + 5);
   bst_setup_test_node(n + 15, NULL,   NULL,   n + 16);
   bst_setup_test_node(n + 1,  NULL,   NULL,   n + 2);
   bst_setup_test_node(n + 3,  NULL,   NULL,   n + 2);
   bst_setup_test_node(n + 6,  NULL,   NULL,   n + 7);


   const struct test_node *res;

   // next
   for (size_t i = 0; i < SIZE - 1; i++)
   {
      res = __bst_next(NULL, n + i, test_node_get_parent, __parent);
      TEST_ASSERT_EQUAL(res, n + i + 1);
   }

   res = __bst_next(NULL, n + SIZE - 1, test_node_get_parent, __parent);
   TEST_ASSERT_NULL(res);


   // prev
   for (size_t i = SIZE - 1; i > 0; i--)
   {
      res = __bst_prev(NULL, n + i, test_node_get_parent, __parent);
      TEST_ASSERT_EQUAL(res, n + i - 1);
   }

   res = __bst_prev(NULL, n + 0, test_node_get_parent, __parent);
   TEST_ASSERT_NULL(res);

   #undef SIZE
}

static void test_bst_preorder_next(void)
{
   #define SIZE 17
   struct test_node n[SIZE];

   /*
    *                  10
    *             /          \
    *           8            13
    *        /    \        /    \
    *       4      9     11      14
    *     /  \             \       \
    *    0    5             12      16
    *     \    \                    /
    *     2     7                 15
    *    / \   /
    *   1  3  6
    */
   bst_setup_test_node(n + 10, n + 8,  n + 13, NULL);
   bst_setup_test_node(n + 8,  n + 4,  n + 9,  n + 10);
   bst_setup_test_node(n + 13, n + 11, n + 14, n + 10);
   bst_setup_test_node(n + 4,  n + 0,  n + 5,  n + 8);
   bst_setup_test_node(n + 9,  NULL,   NULL,   n + 8);
   bst_setup_test_node(n + 11, NULL,   n + 12, n + 13);
   bst_setup_test_node(n + 14, NULL,   n + 16, n + 13);
   bst_setup_test_node(n + 0,  NULL,   n + 2,  n + 4);
   bst_setup_test_node(n + 5,  NULL,   n + 7,  n + 4);
   bst_setup_test_node(n + 12, NULL,   NULL,   n + 11);
   bst_setup_test_node(n + 16, n + 15, NULL,   n + 14);
   bst_setup_test_node(n + 2,  n + 1,  n + 3,  n + 0);
   bst_setup_test_node(n + 7,  n + 6,  NULL,   n + 5);
   bst_setup_test_node(n + 15, NULL,   NULL,   n + 16);
   bst_setup_test_node(n + 1,  NULL,   NULL,   n + 2);
   bst_setup_test_node(n + 3,  NULL,   NULL,   n + 2);
   bst_setup_test_node(n + 6,  NULL,   NULL,   n + 7);

   /* Preorder sequence: 10,8,4,0,2,1,3,5,7,6,9,13,11,12,14,16,15 */
   const struct test_node *preorder[SIZE] = {
      n + 10, n + 8,  n + 4, n + 0, n + 2,  n + 1,  n + 3,
      n + 5,  n + 7,  n + 6, n + 9, n + 13, n + 11, n + 12,
      n + 14, n + 16, n + 15
   };

   const struct test_node *res;
   for (size_t i = 0; i < SIZE - 1; i++)
   {
      res = __bst_preorder_next(NULL, preorder[i], test_node_get_parent, __parent);
      TEST_ASSERT_EQUAL(res, preorder[i + 1]);
   }

   res = __bst_preorder_next(NULL, preorder[SIZE - 1], test_node_get_parent, __parent);
   TEST_ASSERT_NULL(res);

   #undef SIZE
}

static void test_bst_postorder_next(void)
{
   #define SIZE 17
   struct test_node n[SIZE];

   /*
    *                  10
    *             /          \
    *           8            13
    *        /    \        /    \
    *       4      9     11      14
    *     /  \             \       \
    *    0    5             12      16
    *     \    \                    /
    *     2     7                 15
    *    / \   /
    *   1  3  6
    */
   bst_setup_test_node(n + 10, n + 8,  n + 13, NULL);
   bst_setup_test_node(n + 8,  n + 4,  n + 9,  n + 10);
   bst_setup_test_node(n + 13, n + 11, n + 14, n + 10);
   bst_setup_test_node(n + 4,  n + 0,  n + 5,  n + 8);
   bst_setup_test_node(n + 9,  NULL,   NULL,   n + 8);
   bst_setup_test_node(n + 11, NULL,   n + 12, n + 13);
   bst_setup_test_node(n + 14, NULL,   n + 16, n + 13);
   bst_setup_test_node(n + 0,  NULL,   n + 2,  n + 4);
   bst_setup_test_node(n + 5,  NULL,   n + 7,  n + 4);
   bst_setup_test_node(n + 12, NULL,   NULL,   n + 11);
   bst_setup_test_node(n + 16, n + 15, NULL,   n + 14);
   bst_setup_test_node(n + 2,  n + 1,  n + 3,  n + 0);
   bst_setup_test_node(n + 7,  n + 6,  NULL,   n + 5);
   bst_setup_test_node(n + 15, NULL,   NULL,   n + 16);
   bst_setup_test_node(n + 1,  NULL,   NULL,   n + 2);
   bst_setup_test_node(n + 3,  NULL,   NULL,   n + 2);
   bst_setup_test_node(n + 6,  NULL,   NULL,   n + 7);

   /* Postorder sequence: 1,3,2,0,6,7,5,4,9,8,12,11,15,16,14,13,10 */
   const struct test_node *postorder[SIZE] = {
      n + 1, n + 3, n + 2,  n + 0,  n + 6,  n + 7,  n + 5,  n + 4,
      n + 9, n + 8, n + 12, n + 11, n + 15, n + 16, n + 14, n + 13, n + 10
   };

   const struct test_node *res;
   for (size_t i = 0; i < SIZE - 1; i++)
   {
      res = __bst_postorder_next(NULL, postorder[i], test_node_get_parent, __parent);
      TEST_ASSERT_EQUAL(res, postorder[i + 1]);
   }

   res = __bst_postorder_next(NULL, postorder[SIZE - 1], test_node_get_parent, __parent);
   TEST_ASSERT_NULL(res);

   #undef SIZE
}


int main()
{
   UnityBegin("test/bst.h");

   RUN_TEST(test_bst_parent_ptr);
   RUN_TEST(test_bst_delink_node);
   RUN_TEST(test_bst_replace_node);
   RUN_TEST(test_bst_add_rm_linked);

   // RUN_TEST(test_bst_rotate_left);
   // RUN_TEST(test_bst_rotate_right);
   // RUN_TEST(test_bst_rotate_left_left);
   // RUN_TEST(test_bst_rotate_right_right);
   // RUN_TEST(test_bst_rotate_left_right);
   // RUN_TEST(test_bst_rotate_right_left);

   RUN_TEST(test_bst_first_last_postorder_first);
   RUN_TEST(test_bst_next_prev);
   RUN_TEST(test_bst_preorder_next);
   RUN_TEST(test_bst_postorder_next);

   return UnityEnd();
}

#endif /* HUZLIB_BST_TEST */


#endif /* HUZLIB_BST */
