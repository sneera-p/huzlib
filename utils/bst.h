#ifndef HUZLIB_BST_H
#define HUZLIB_BST_H


#ifndef HUZLIB_BST_INCLUDES
#define HUZLIB_BST_INCLUDES

#include "utils/types.h"
#include "utils/hints.h"
#include "utils/assert.h"

#endif /* HUZLIB_BST_INCLUDES */


/*
 * So I plan to have 3 BSTs in this library:
 *
 *    1. AVL tree
 *    2. Red-Black tree
 *    3. Splay tree
 *
 * All 3 share the same traversal logic. The only difference is the node type.
 *
 * I don't want to write the same foreach code 3 times. That's stupid.
 * So I make one set of macros here. The tree implementations just wrap them.
 *
 * There are 4 macros:
 *
 *    1. __bst_foreach
 *       Walk through nodes. Simple.
 *
 *    2. __bst_foreach_safe
 *       Same but you can delete the current node. Safe.
 *
 *    3. __bst_foreach_entry
 *       Walk through containers (structs that hold a node).
 *
 *    4. __bst_foreach_entry_safe
 *       Same but you can delete the current container. Safe.
 *
 *
 * How do they know which order to walk?
 * -------------------------------------------------
 *
 * They don't. You tell them.
 *
 * You give me two things:
 *    start     -> the first node
 *    traverse  -> how to get the next node from current
 *
 * That's it. I don't care if it's AVL, RB, Splay, or a damn linked list.
 *
 * Examples:
 *    Inorder:     avl_first(root)     + avl_next(node)
 *    Preorder:    root                + avl_preorder_next(node)
 *    Postorder:   avl_postorder_first + avl_postorder_next(node)
 *
 * One macro. Infinite walks. One ring to rule them all.
 *
 *
 * The dirty tricks (why the code looks weird)
 * -------------------------------------------------
 *
 * 1. The _safe trick
 * -------------------------------------------------
 * Look at this:
 *    ((node) != NULL) 
 *       && (((tmp) = (traverse)) || 1)
 *
 * Why? Because if traverse returns NULL (end of walk), the loop condition
 * becomes false and skips the last node's body. That's wrong.
 *
 * The "|| 1" forces it to be true no matter what. So the last node runs.
 *
 *
 * 2. The _entry trick
 * -------------------------------------------------
 * Look at this:
 *    (cur) = (type *)(start);               // step 1 - lie
 *    ((cur) != NULL) 
 *       && ((cur) = container_of(cur));     // step 2 - truth
 *
 * Step 1: cur pretends to be a container pointer but actually holds a node pointer.
 * Step 2: If cur isn't NULL, turn it into a real container pointer.
 *
 * We never use cur between step 1 and step 2. Only check if it's NULL.
 * Safe.
 *
 * Why the "|| 1" here again?
 *
 * container_of can return NULL. How? If the node pointer you pass in equals
 * the offset of 'member' inside the struct. Example: if 'member' is at offset 8
 * and someone hands you (void *)8, container_of subtracts 8 and gives you NULL.
 *
 * Is that likely? No. But the compiler doesn't know that. It sees a possible NULL.
 * so the whole condition could theorecticaly become false and skip the loop body.
 *
 * Therfore we slap a "|| 1" to improve compiler optimization here.
 *
 *
 * 3. The _entry_safe combo
 * -------------------------------------------------
 *    ((cur) != NULL)
 *       && (((cur) = container_of((void *)(cur), type, member)) || 1)
 *       && (((tmp) = (type *)(traverse)) || 1);
 *
 * Here we combine both tricks from (1) and (2).
 *
 * Why in this specific order? Because then the caller who passes the 'traverse' expression can easily do
 *    something like "avl_next(&(cur)->member)"
 *    instead of     "avl_next((struct avl_node *)(cur))"
 * which will utterly confuse the living shit out of everyone looking at it wondering why we are doing
 * a unsafe cast from container type to node type
 *
 *
 * Type checks (you won't shoot yourself in the foot)
 * -------------------------------------------------
 *
 * The caller puts their own type checks in __VA_ARGS__.
 * They sit at the start of the for-loop init.
 *
 * This makes sure:
 *    - Your iterator variable has the right type
 *    - traverse returns the right node type
 *    - _entry macros match container type
 *
 * The tree wrappers add these checks. You don't call these macros directly.
 * That's the deal.
 */

#include <stddef.h>

#define __bst_foreach(node, start, traverse, ...)                       \
   for (                                                                \
      __VA_ARGS__, /* caller injected typechecks */                     \
      typecheck(typeof(*(node)), *(start)),                             \
      typecheck(typeof(*(node)), *(traverse)),                          \
      (node) = (start);                                                 \
      (node) != NULL;                                                   \
      (node) = (traverse)                                               \
   )

#define __bst_foreach_safe(node, tmp, start, traverse, ...)             \
   for (                                                                \
      __VA_ARGS__, /* caller injected typechecks */                     \
      typecheck(typeof(*(node)), *(tmp)),                               \
      typecheck(typeof(*(node)), *(start)),                             \
      typecheck(typeof(*(node)), *(traverse)),                          \
      (node) = (start);                                                 \
      ((node) != NULL)                                                  \
         && (((tmp) = (traverse)) || 1);                                \
      (node) = (tmp)                                                    \
   )

#define __bst_foreach_entry(cur, type, member, start, traverse, ...)                            \
   for (                                                                                        \
      __VA_ARGS__, /* caller injected typechecks */                                             \
      typecheck(type, *(cur)),                                                                  \
      typecheck(typeof_member(type, member), *(start)),                                         \
      typecheck(typeof_member(type, member), *(traverse)),                                      \
      (cur) = (type *)(start);                                                                  \
      ((cur) != NULL)                                                                           \
         && (((cur) = container_of((typeof_member(type, member) *)(cur), type, member)) || 1);  \
      (cur) = (type *)(traverse)                                                                \
   )

#define __bst_foreach_entry_safe(cur, tmp, type, member, start, traverse, ...)                  \
   for (                                                                                        \
      __VA_ARGS__, /* caller injected typechecks */                                             \
      typecheck(type, *(cur)),                                                                  \
      typecheck(type, *(tmp)),                                                                  \
      typecheck(typeof_member(type, member), *(start)),                                         \
      typecheck(typeof_member(type, member), *(traverse)),                                      \
      (cur) = (type *)(start);                                                                  \
      ((cur) != NULL)                                                                           \
         && (((cur) = container_of((typeof_member(type, member) *)(cur), type, member)) || 1)   \
         && (((tmp) = (type *)(traverse)) || 1);                                                \
      (cur) = (tmp)                                                                             \
   )




#ifndef HUZLIB_BST_INLINE_IMPL
#define HUZLIB_BST_INLINE_IMPL


/*
 * So I plan to have 3 BSTs in this library:
 *
 *    1. AVL tree
 *    2. Red-Black tree
 *    3. Splay tree
 *
 * All 3 share the same traversal logic. The only difference is the node type.
 *
 * I don't want to write the same traversal code 3 times. That's stupid.
 *
 * So I look at linux/min_heap.h for inspiration. (I know! sounds crazy but hear me out)
 *
 * min_heap.h does this fake generic trick:
 *    - Internal type = void / char
 *    - Capacity = 0 / [] (flexible array)
 *    - Write functions around the fake type
 *    - Use macros to cast real type <-> internal type
 *
 * One set of functions. Every type of min_heap. ONE RING TO RULE THEM ALL.
 *
 * I steal this idea.
 *
 * I create an internal type called 'struct bst_node'.
 * I write all traversal functions around this type.
 * AVL/RB/splay just wrap these functions internally.
 *
 * But there's a problem...
 *
 * The parent pointer.
 *
 * In a simple linked list, parent is just a pointer. Simple.
 * But AVL packs balance factor into the low bits of parent pointer.
 * Red-Black packs the color bit.
 * Splay does its own weird shit.
 *
 * I can't just read parent directly. Each tree needs its own tiny function
 * to unpack the real parent from the packed value.
 *
 * Here's the fix:
 *
 * bst_node stores parent as a raw uintptr_t at offset 0.
 * This matches the layout of avl_node, rb_node, splay_node exactly.
 *
 * The traversal code gets a small get_parent() callback that knows how to
 * unpack parent for that specific tree type.
 *
 * These callbacks are force_inlined and always called with a concrete function.
 * The compiler folds them away. Zero overhead.
 *
 * Converting between avl_node/rb_node/splay_node and bst_node?
 * container_of on the shared first member. Safe. No strict aliasing bugs.
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
   #define HUZLIB_BST_INTERNAL static __huzlib_noinline__
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
HUZLIB_BST_INTERNAL void __huzlib_bst_delink_node(struct bst_node *restrict child, struct bst_node *restrict parent, struct bst_node **restrict link, void (*set_parent)(struct bst_node *restrict, const struct bst_node *restrict))
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
   __huzlib_assert(old && new && link && (*link == old));
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
 *   eg: __rb_set_parent_color(node, child, RB_BLACK);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_left(struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node **restrict link, void (*set_parent)(struct bst_node *restrict, const struct bst_node *restrict))
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
 *    eg: __rb_set_parent_color(node, child, RB_RED);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_right(struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node **restrict link, void (*set_parent)(struct bst_node *restrict, const struct bst_node *restrict))
{
   __huzlib_assert(node && child && link && (*link == node) && (node->left == child));

   if (child->right)
      set_parent(child->right, node);

   node->left = child->right;
   child->right = node;

   *link = child;
}

/*
 * __huzlib_bst_rotate_left_left(parent, node, child, link, set_parent)
 * ---------------------------------------------------------------------
 * Double left rotation. parent moves down-right onto node, then node
 * moves down-right onto child. child becomes the new subtree root.
 *
 *     p                             n                             c
 *    / \                          /   \                          / \
 *  (w)  n                       p       c                       n  (z)
 *      / \          ==>        / \     / \        ==>          / \
 *    (x)  c                  (w) (x) (y) (z)                  p  (y)
 *        / \                                                 / \
 *      (y) (z)                                             (w) (x)
 *
 * @parent:     the subtree root rotating down, not NULL
 * @node:       parent's right child, not NULL
 * @child:      node's right child, becomes the new subtree root, not NULL
 * @link:       parent->parent's internal pointer to parent, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Does not update '__packed_parent' fields of 'node', 'parent', and 'child'.
 * Caller must update them after rotation.
 *    eg: __splay_set_parent(node, child);
 *        __splay_set_parent(parent, node);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_left_left(struct bst_node *restrict parent, struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node **restrict link, void (*set_parent)(struct bst_node *restrict, const struct bst_node *restrict))
{
   __huzlib_assert(node && child && parent && link && (*link == parent) && (node->right == child) && (parent->right == node));

   if (node->left) /* (x) */
      set_parent(node->left, parent);

   if (child->left) /* (y) */
      set_parent(child->left, node);

   parent->right = node->left;
   node->right = child->left;

   node->left = parent;
   child->left = node;

   *link = child;
}

/*
 * __huzlib_bst_rotate_right_right(parent, node, child, link, set_parent)
 * -----------------------------------------------------------------------
 * Double right rotation. parent moves down-left onto node, then node
 * moves down-left onto child. child becomes the new subtree root.
 *
 *         p                         n                         c
 *        / \                      /   \                      / \
 *       n  (w)                  c       p                  (z)  n
 *      / \          ==>        / \     / \        ==>          / \
 *     c  (x)                 (z) (y) (x) (w)                 (y)  p
 *    / \                                                         / \
 *  (z) (y)                                                     (x) (w)
 *
 * @parent:     the subtree root rotating down, not NULL
 * @node:       parent's left child, not NULL
 * @child:      node's left child, becomes the new subtree root, not NULL
 * @link:       parent->parent's internal pointer to parent, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Does not update '__packed_parent' fields of 'node', 'parent', and 'child'.
 * Caller must update them after rotation.
 *    eg: __splay_set_parent(node, child);
 *        __splay_set_parent(parent, node);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_right_right(struct bst_node *restrict parent, struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node **restrict link, void (*set_parent)(struct bst_node *restrict, const struct bst_node *restrict))
{
   __huzlib_assert(node && child && parent && link && (*link == parent) && (node->left == child) && (parent->left == node));

   if (node->right) /* (x) */
      set_parent(node->right, parent);

   if (child->right) /* (y) */
      set_parent(child->right, node);

   parent->left = node->right;
   node->left = child->right;

   node->right = parent;
   child->right = node;

   *link = child;
}

/*
 * __huzlib_bst_rotate_left_right(parent, node, child, link, set_parent)
 * ----------------------------------------------------------------------
 * Left-right double rotation. node rotates left, lifting child up between
 * node and parent, then parent rotates right, making child the new subtree
 * root with node and parent as its children.
 *
 *        p                         p                        c
 *       / \                       / \                     /   \
 *      n  (w)                    c  (w)                 n       p
 *     / \          ==>          / \          ==>       / \     / \
 *   (x)  c                     n  (z)                (x) (y) (z) (w)
 *       / \                   / \
 *     (y) (z)               (x) (y)
 *
 * @parent:     the subtree root rotating down, not NULL
 * @node:       parent's left child, not NULL
 * @child:      node's right child, becomes the new subtree root, not NULL
 * @link:       parent->parent's internal pointer to parent, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Does not update '__packed_parent' fields of 'node', 'parent', and 'child'.
 * Caller must update them after rotation.
 *    eg: __avl_set_parent_balance(node, child, x);
 *        __avl_set_parent_balance(parent, child, y);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_left_right(struct bst_node *restrict parent, struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node **restrict link, void (*set_parent)(struct bst_node *restrict, const struct bst_node *restrict))
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
 * __huzlib_bst_rotate_right_left(parent, node, child, link, set_parent)
 * ----------------------------------------------------------------------
 * Right-left double rotation. node rotates right, lifting child up between
 * node and parent, then parent rotates left, making child the new subtree
 * root with parent and node as its children.
 *
 *      p                       p                            c
 *     / \                     / \                         /   \
 *   (w)  n                  (w)  c                      p       n
 *       / \        ==>          / \          ==>       / \     / \
 *      c  (x)                 (z)  n                 (w) (z) (y) (x)
 *     / \                         / \
 *   (z) (y)                     (y) (x)
 *
 * @parent:     the subtree root rotating down, not NULL
 * @node:       parent's right child, not NULL
 * @child:      node's left child, becomes the new subtree root, not NULL
 * @link:       parent->parent's internal pointer to parent, not NULL
 * @set_parent: parent setter
 *
 * NOTE:
 * Does not update '__packed_parent' fields of 'node', 'parent', and 'child'.
 * Caller must update them after rotation.
 *    eg: __avl_set_parent_balance(node, child, x);
 *        __avl_set_parent_balance(parent, child, y);
 */
HUZLIB_BST_INTERNAL void __huzlib_bst_rotate_right_left(struct bst_node *restrict parent, struct bst_node *restrict node, struct bst_node *restrict child, struct bst_node **restrict link, void (*set_parent)(struct bst_node *restrict, const struct bst_node *restrict))
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

#define __bst_rotate_left(node, child, link, set_parent, __parent_member)                 \
   __huzlib_bst_rotate_left(                                                              \
      bst_node_cast(node, __parent_member),                                               \
      bst_node_cast(child, __parent_member),                                              \
      (struct bst_node **)(link),                                                         \
      set_parent                                                                          \
   )

#define __bst_rotate_right(node, child, link, set_parent, __parent_member)                \
   __huzlib_bst_rotate_right(                                                             \
      bst_node_cast(node, __parent_member),                                               \
      bst_node_cast(child, __parent_member),                                              \
      (struct bst_node **)(link),                                                         \
      set_parent                                                                          \
   )

#define __bst_rotate_left_left(parent, node, child, link, set_parent, __parent_member)    \
   __huzlib_bst_rotate_left_left(                                                         \
      bst_node_cast(parent, __parent_member),                                             \
      bst_node_cast(node, __parent_member),                                               \
      bst_node_cast(child, __parent_member),                                              \
      (struct bst_node **)(link),                                                         \
      set_parent                                                                          \
   )

#define __bst_rotate_right_right(parent, node, child, link, set_parent, __parent_member)  \
   __huzlib_bst_rotate_right_right(                                                       \
      bst_node_cast(parent, __parent_member),                                             \
      bst_node_cast(node, __parent_member),                                               \
      bst_node_cast(child, __parent_member),                                              \
      (struct bst_node **)(link),                                                         \
      set_parent                                                                          \
   )

#define __bst_rotate_left_right(parent, node, child, link, set_parent, __parent_member)   \
   __huzlib_bst_rotate_left_right(                                                        \
      bst_node_cast(parent, __parent_member),                                             \
      bst_node_cast(node, __parent_member),                                               \
      bst_node_cast(child, __parent_member),                                              \
      (struct bst_node **)(link),                                                         \
      set_parent                                                                          \
   )

#define __bst_rotate_right_left(parent, node, child, link, set_parent, __parent_member)   \
   __huzlib_bst_rotate_right_left(                                                        \
      bst_node_cast(parent, __parent_member),                                             \
      bst_node_cast(node, __parent_member),                                               \
      bst_node_cast(child, __parent_member),                                              \
      (struct bst_node **)(link),                                                         \
      set_parent                                                                          \
   )

#define __bst_first(node, __parent_member)            \
   bst_node_recast(                                   \
      __huzlib_bst_first(                             \
         bst_node_cast(node, __parent_member)         \
      ),                                              \
      typeof(*(node)),                                \
      __parent_member                                 \
   )

#define __bst_last(node, __parent_member)             \
   bst_node_recast(                                   \
      __huzlib_bst_last(                              \
         bst_node_cast(node, __parent_member)         \
      ),                                              \
      typeof(*(node)),                                \
      __parent_member                                 \
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


#endif /* HUZLIB_BST_INLINE_IMPL */


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
   return (void *)__parent;
}

static void test_node_set_parent(struct test_node *restrict node, const struct test_node *restrict parent)
{
   __huzlib_assert(node);
   node->__parent = (uintptr_t)parent;
}

static void test_node_set_parent_bst(struct bst_node *restrict node, const struct bst_node *restrict parent)
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


void setUp(void) {}
void tearDown(void) {}


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
   bst_setup_test_node(&node, &child, NULL, &parent);
   bst_setup_test_node(&child, &t, &u, &node);
   bst_setup_test_node(&t, NULL, NULL, &child);
   bst_setup_test_node(&u, NULL, NULL, &child);

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


static void test_bst_rotate_left(void)
{
   /*
    *  p                p
    *   \                \
    *    n                c
    *   / \              / \
    *  x   c     =>     n   z
    *     / \          / \
    *    y   z        x   y
    */
   struct test_node p, n, c, x, y, z;
   bst_setup_test_node(&p, NULL, &n, NULL);
   bst_setup_test_node(&n, &x,   &c, &p);
   bst_setup_test_node(&c, &y,   &z, &n);
   bst_setup_test_node(&x, NULL, NULL, &n);
   bst_setup_test_node(&y, NULL, NULL, &c);
   bst_setup_test_node(&z, NULL, NULL, &c);

   __bst_rotate_left(&n, &c, &p.right, test_node_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL(p.right, &c);

   /* c takes n's place */
   TEST_ASSERT_EQUAL(c.left,  &n);
   TEST_ASSERT_EQUAL(c.right, &z);

   /* n gets c's old left child */
   TEST_ASSERT_EQUAL(n.left,  &x);
   TEST_ASSERT_EQUAL(n.right, &y);

   /* parent pointer of adopted child updated */
   TEST_ASSERT_EQUAL(test_node_get_parent(y.__parent), &n);
}

static void test_bst_rotate_right(void)
{
   /*
    *         p            p
    *        /            /
    *       n            c
    *      / \          / \
    *     c   x   =>   z   n
    *    / \              / \
    *   z   y            y   x
    */
   struct test_node p, n, c, x, y, z;
   bst_setup_test_node(&p, &n,   NULL, NULL);
   bst_setup_test_node(&n, &c,   &x,   &p);
   bst_setup_test_node(&c, &z,   &y,   &n);
   bst_setup_test_node(&x, NULL, NULL,  &n);
   bst_setup_test_node(&y, NULL, NULL,  &c);
   bst_setup_test_node(&z, NULL, NULL,  &c);

   __bst_rotate_right(&n, &c, &p.left, test_node_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL(p.left, &c);

   /* c takes n's place */
   TEST_ASSERT_EQUAL(c.left,  &z);
   TEST_ASSERT_EQUAL(c.right, &n);

   /* n gets c's old right child */
   TEST_ASSERT_EQUAL(n.left,  &y);
   TEST_ASSERT_EQUAL(n.right, &x);

   /* parent pointer of adopted child updated */
   TEST_ASSERT_EQUAL(test_node_get_parent(y.__parent), &n);
}

static void test_bst_rotate_left_left(void)
{
   /*
    *      g                  g
    *     /                  /
    *    p                  c
    *   / \                / \
    *  w   n      =>      n   z
    *     / \            / \
    *    x   c          p   y
    *       / \        / \
    *      y   z      w   x
    */
   struct test_node g, p, n, c, w, x, y, z;
   bst_setup_test_node(&g, &p,   NULL, NULL);
   bst_setup_test_node(&p, &w,   &n,   &g);
   bst_setup_test_node(&n, &x,   &c,   &p);
   bst_setup_test_node(&c, &y,   &z,   &n);
   bst_setup_test_node(&w, NULL, NULL,  &p);
   bst_setup_test_node(&x, NULL, NULL,  &n);
   bst_setup_test_node(&y, NULL, NULL,  &c);
   bst_setup_test_node(&z, NULL, NULL,  &c);

   __bst_rotate_left_left(&p, &n, &c, &g.left, test_node_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL(g.left, &c);

   /* c is new root */
   TEST_ASSERT_EQUAL(c.left,  &n);
   TEST_ASSERT_EQUAL(c.right, &z);

   /* n gets p and y */
   TEST_ASSERT_EQUAL(n.left,  &p);
   TEST_ASSERT_EQUAL(n.right, &y);

   /* p keeps w, gets x */
   TEST_ASSERT_EQUAL(p.left,  &w);
   TEST_ASSERT_EQUAL(p.right, &x);

   /* parent pointers of adopted children updated */
   TEST_ASSERT_EQUAL(test_node_get_parent(x.__parent), &p);
   TEST_ASSERT_EQUAL(test_node_get_parent(y.__parent), &n);
}

static void test_bst_rotate_right_right(void)
{
   /*
    *      g                  g
    *       \                  \
    *        p                  c
    *       / \                / \
    *      n   w      =>      z   n
    *     / \                    / \
    *    c   x                  y   p
    *   / \                        / \
    *  z   y                      x   w
    */
   struct test_node g, p, n, c, w, x, y, z;
   bst_setup_test_node(&g, NULL, &p,   NULL);
   bst_setup_test_node(&p, &n,   &w,   &g);
   bst_setup_test_node(&n, &c,   &x,   &p);
   bst_setup_test_node(&c, &z,   &y,   &n);
   bst_setup_test_node(&w, NULL, NULL,  &p);
   bst_setup_test_node(&x, NULL, NULL,  &n);
   bst_setup_test_node(&y, NULL, NULL,  &c);
   bst_setup_test_node(&z, NULL, NULL,  &c);

   __bst_rotate_right_right(&p, &n, &c, &g.right, test_node_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL(g.right, &c);

   /* c is new root */
   TEST_ASSERT_EQUAL(c.left,  &z);
   TEST_ASSERT_EQUAL(c.right, &n);

   /* n gets y and p */
   TEST_ASSERT_EQUAL(n.left,  &y);
   TEST_ASSERT_EQUAL(n.right, &p);

   /* p keeps w, gets x */
   TEST_ASSERT_EQUAL(p.left,  &x);
   TEST_ASSERT_EQUAL(p.right, &w);

   /* parent pointers of adopted children updated */
   TEST_ASSERT_EQUAL(test_node_get_parent(x.__parent), &p);
   TEST_ASSERT_EQUAL(test_node_get_parent(y.__parent), &n);
}

static void test_bst_rotate_left_right(void)
{
   /*
    *      g                  g
    *     /                  /
    *    p                  c
    *   / \                / \
    *  n   w      =>      n   p
    *     / \            / \ / \
    *    x   c          x  y z  w
    *       / \
    *      y   z
    */
   struct test_node g, p, n, c, w, x, y, z;
   bst_setup_test_node(&g, &p,   NULL, NULL);
   bst_setup_test_node(&p, &n,   &w,   &g);
   bst_setup_test_node(&n, &x,   &c,   &p);
   bst_setup_test_node(&c, &y,   &z,   &n);
   bst_setup_test_node(&w, NULL, NULL,  &p);
   bst_setup_test_node(&x, NULL, NULL,  &n);
   bst_setup_test_node(&y, NULL, NULL,  &c);
   bst_setup_test_node(&z, NULL, NULL,  &c);

   __bst_rotate_left_right(&p, &n, &c, &g.left, test_node_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL(g.left, &c);

   /* c is new root */
   TEST_ASSERT_EQUAL(c.left,  &n);
   TEST_ASSERT_EQUAL(c.right, &p);

   /* n gets x and y */
   TEST_ASSERT_EQUAL(n.left,  &x);
   TEST_ASSERT_EQUAL(n.right, &y);

   /* p gets z and w */
   TEST_ASSERT_EQUAL(p.left,  &z);
   TEST_ASSERT_EQUAL(p.right, &w);

   /* parent pointers of adopted children updated */
   TEST_ASSERT_EQUAL(test_node_get_parent(y.__parent), &n);
   TEST_ASSERT_EQUAL(test_node_get_parent(z.__parent), &p);
}

static void test_bst_rotate_right_left(void)
{
   /*
    *    g                  g
    *     \                  \
    *      p                  c
    *     / \                / \
    *    w   n      =>      p   n
    *       / \            / \ / \
    *      c   x          w  y z  x
    *     / \
    *    y   z
    */
   struct test_node g, p, n, c, w, x, y, z;
   bst_setup_test_node(&g, NULL, &p,   NULL);
   bst_setup_test_node(&p, &w,   &n,   &g);
   bst_setup_test_node(&n, &c,   &x,   &p);
   bst_setup_test_node(&c, &y,   &z,   &n);
   bst_setup_test_node(&w, NULL, NULL,  &p);
   bst_setup_test_node(&x, NULL, NULL,  &n);
   bst_setup_test_node(&y, NULL, NULL,  &c);
   bst_setup_test_node(&z, NULL, NULL,  &c);

   __bst_rotate_right_left(&p, &n, &c, &g.right, test_node_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL(g.right, &c);

   /* c is new root */
   TEST_ASSERT_EQUAL(c.left,  &p);
   TEST_ASSERT_EQUAL(c.right, &n);

   /* p gets w and y */
   TEST_ASSERT_EQUAL(p.left,  &w);
   TEST_ASSERT_EQUAL(p.right, &y);

   /* n gets z and x */
   TEST_ASSERT_EQUAL(n.left,  &z);
   TEST_ASSERT_EQUAL(n.right, &x);

   /* parent pointers of adopted children updated */
   TEST_ASSERT_EQUAL(test_node_get_parent(y.__parent), &p);
   TEST_ASSERT_EQUAL(test_node_get_parent(z.__parent), &n);
}


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


struct test_container
{
   int padding[2];
   struct test_node bst;
   int key;
};

static struct test_node *test_node_first(const struct test_node *subroot)
{
   return (struct test_node *)__bst_first(subroot, __parent);
}

static struct test_node *test_node_next(const struct test_node *subroot, const struct test_node *node)
{
   return (struct test_node *)__bst_next(test_node_get_parent(subroot->__parent), node, test_node_get_parent, __parent);
}

static void test_node_bst_foreach(void)
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

   struct test_node *node, *tmp, *subroot = n + 10;

   // foreach unsafe
   #define test_node_foreach(node, subroot)     \
      __bst_foreach(node,                       \
         test_node_first(subroot),              \
         test_node_next(subroot, node),         \
         typecheck(struct test_node, *(node))   \
      )

   size_t i = 0;
   test_node_foreach(node, subroot)
   {
      TEST_ASSERT_EQUAL(node, n + i);
      i++;
   }

   // foreach unsafe
   #define test_node_foreach_safe(node, tmp, subroot) \
      __bst_foreach_safe(node, tmp,                   \
         test_node_first(subroot),                    \
         test_node_next(subroot, node),               \
         typecheck(struct test_node, *(node)),        \
         typecheck(struct test_node, *(tmp))          \
      )


   i = 0;
   test_node_foreach_safe(node, tmp, subroot) 
   {
      if (i < SIZE - 1)
        TEST_ASSERT_EQUAL(tmp, n + i + 1);  // tmp should be the next node
      else
        TEST_ASSERT_NULL(tmp);

      TEST_ASSERT_EQUAL(node, n + i);
      node = NULL; // simulate free()
      i++;
   }

   #undef test_node_foreach_safe
   #undef test_node_foreach
   #undef SIZE
}

static void test_node_bst_foreach_entry(void)
{
   #define SIZE 17
   struct test_container containers[SIZE];
   struct test_node *nodes[SIZE];

   /*
    * Tree structure (same as before):
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
   bst_setup_test_node(&containers[10].bst, &containers[8].bst,  &containers[13].bst, NULL);
   bst_setup_test_node(&containers[8].bst,  &containers[4].bst,  &containers[9].bst,  &containers[10].bst);
   bst_setup_test_node(&containers[13].bst, &containers[11].bst, &containers[14].bst, &containers[10].bst);
   bst_setup_test_node(&containers[4].bst,  &containers[0].bst,  &containers[5].bst,  &containers[8].bst);
   bst_setup_test_node(&containers[9].bst,  NULL,                NULL,                &containers[8].bst);
   bst_setup_test_node(&containers[11].bst, NULL,                &containers[12].bst, &containers[13].bst);
   bst_setup_test_node(&containers[14].bst, NULL,                &containers[16].bst, &containers[13].bst);
   bst_setup_test_node(&containers[0].bst,  NULL,                &containers[2].bst,  &containers[4].bst);
   bst_setup_test_node(&containers[5].bst,  NULL,                &containers[7].bst,  &containers[4].bst);
   bst_setup_test_node(&containers[12].bst, NULL,                NULL,                &containers[11].bst);
   bst_setup_test_node(&containers[16].bst, &containers[15].bst, NULL,                &containers[14].bst);
   bst_setup_test_node(&containers[2].bst,  &containers[1].bst,  &containers[3].bst,  &containers[0].bst);
   bst_setup_test_node(&containers[7].bst,  &containers[6].bst,  NULL,                &containers[5].bst);
   bst_setup_test_node(&containers[15].bst, NULL,                NULL,                &containers[16].bst);
   bst_setup_test_node(&containers[1].bst,  NULL,                NULL,                &containers[2].bst);
   bst_setup_test_node(&containers[3].bst,  NULL,                NULL,                &containers[2].bst);
   bst_setup_test_node(&containers[6].bst,  NULL,                NULL,                &containers[7].bst);

   for (int i = 0; i < SIZE; i++)
      nodes[i] = &containers[i].bst;

   struct test_container *cur, *tmp;
   struct test_node *subroot = nodes[10];

   /* For each_entry unsafe - iterates over containers in inorder */
   #define test_node_foreach_entry(cur, subroot, type, member) \
      __bst_foreach_entry(cur, type, member,                   \
         test_node_first(subroot),                             \
         test_node_next(subroot, &(cur)->bst),                 \
         typecheck(typeof_member(type, member), *(subroot))    \
      )

   size_t i = 0;
   test_node_foreach_entry(cur, subroot, struct test_container, bst)
   {
      TEST_ASSERT_EQUAL(cur, containers + i);
      i++;
   }
   TEST_ASSERT_EQUAL(i, SIZE);

   /* For each_entry_safe - safe to delete current node */
   #define test_node_foreach_entry_safe(cur, tmp, subroot, type, member)   \
      __bst_foreach_entry_safe(cur, tmp, type, member,                     \
         test_node_first(subroot),                                         \
         test_node_next(subroot, &cur->bst),                               \
         typecheck(typeof_member(type, member), *(subroot))                \
      )

   i = 0;
   test_node_foreach_entry_safe(cur, tmp, subroot, struct test_container, bst)
   {
      if (i < SIZE - 1)
         TEST_ASSERT_EQUAL(tmp, nodes[i + 1]);  /* tmp should be the next node's bst member */
      else
         TEST_ASSERT_NULL(tmp);

      TEST_ASSERT_EQUAL(cur, containers + i);

      /* Simulate deleting/freeing the current container */
      cur = NULL;
      i++;
   }

   #undef test_node_foreach_entry_safe
   #undef test_node_foreach_entry
   #undef SIZE
}


int main(void)
{
   UnityBegin("test/bst.h");

   RUN_TEST(test_bst_parent_ptr);
   RUN_TEST(test_bst_delink_node);
   RUN_TEST(test_bst_replace_node);
   RUN_TEST(test_bst_add_rm_linked);

   RUN_TEST(test_bst_rotate_left);
   RUN_TEST(test_bst_rotate_right);
   RUN_TEST(test_bst_rotate_left_left);
   RUN_TEST(test_bst_rotate_right_right);
   RUN_TEST(test_bst_rotate_left_right);
   RUN_TEST(test_bst_rotate_right_left);

   RUN_TEST(test_bst_first_last_postorder_first);
   RUN_TEST(test_bst_next_prev);
   RUN_TEST(test_bst_preorder_next);
   RUN_TEST(test_bst_postorder_next);

   RUN_TEST(test_node_bst_foreach);
   RUN_TEST(test_node_bst_foreach_entry);

   return UnityEnd();
}

#endif /* HUZLIB_BST_TEST */


#endif /* HUZLIB_BST_H */
