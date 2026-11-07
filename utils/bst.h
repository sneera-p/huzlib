#ifndef HUZ_BST_H
#define HUZ_BST_H


#if !(                              \
   defined(huz_typeof) &&           \
   defined(huz_typeof_member) &&    \
   defined(huz_typecheck) &&        \
   defined(huz_container_of) &&     \
   defined(huz_assert) &&           \
   defined(__huz_pure__) &&         \
   defined(__huz_reproducible__) && \
   defined(__huz_likely__) &&       \
   defined(__huz_unlikely__) &&     \
   defined(HUZ_INTERNAL_API)        \
)
   #error "bst required macros not available"
#endif



/*
 * Generic BST traversal macros.
 * -----------------------------
 * These macros provide shared traversal logic for AVL, Red-Black, Splay,
 * and other tree implementations. The tree implementations supply the
 * starting node and traversal function, so the macros don't care about
 * the underlying data structure or traversal order.
 *
 * There are four variants:
 *    __huz_bst_foreach
 *    __huz_bst_foreach_safe
 *    __huz_bst_foreach_entry
 *    __huz_bst_foreach_entry_safe
 *
 * The caller supplies:
 *    start     -> the first node
 *    traverse  -> how to get the next node from current
 *
 * Examples:
 *    Inorder:     avl_first(root)     + avl_next(node)
 *    Preorder:    root                + avl_preorder_next(node)
 *    Postorder:   avl_postorder_first + avl_postorder_next(node)
 */

#include <stddef.h>

/*
 * __huz_bst_foreach(node, start, traverse, ...typechecks)
 * -------------------------------------------------------
 * Walk through nodes using the supplied start and traversal expressions.
 *
 * The traversal order is entirely determined by the caller.
 * This works with inorder, preorder, postorder, or even a linked list.
 *
 * The caller injects type checks through __VA_ARGS__, ensuring that:
 *    - The iterator has the expected node type.
 *    - The start expression returns the expected node type.
 *    - The traversal expression returns the expected node type.
 */
#define __huz_bst_foreach(node, start, traverse, ...) \
   for (                                              \
      __VA_ARGS__, /* caller injected typechecks */   \
      huz_types_equal(*(node), *(start)),             \
      huz_types_equal(*(node), *(traverse)),          \
      (node) = (start);                               \
      (node) != NULL;                                 \
      (node) = (traverse)                             \
   )

/*
 * __huz_bst_foreach_safe(node, tmp, start, traverse, ...typechecks)
 * -----------------------------------------------------------------
 * Same as __huz_bst_foreach, but allows the current node to be deleted
 * during iteration.
 *
 * The next node is stored in tmp before entering the loop body.
 * This ensures that deleting the current node doesn't invalidate
 * the iterator.
 *
 * The ", 1" trick is important:
 *
 *    ((tmp) = (traverse)), 1
 *
 * Without it, if traverse returns NULL, the loop condition becomes false
 * and the final node's body is skipped.
 *
 * The comma operator forces the condition to evaluate to true after
 * assigning tmp, allowing the current node's body to execute.
 *
 * eg:
 *    __huz_bst_foreach_safe(cur, tmp, avl_first(root), avl_prev(cur))
 */
#define __huz_bst_foreach_safe(node, tmp, start, traverse, ...)   \
   for (                                                          \
      __VA_ARGS__, /* caller injected typechecks */               \
      huz_types_equal(*(node), *(tmp)),                           \
      huz_types_equal(*(node), *(start)),                         \
      huz_types_equal(*(node), *(traverse)),                      \
      (node) = (start);                                           \
      ((node) != NULL)                                            \
         && (((tmp) = (traverse)), 1);                            \
      (node) = (tmp)                                              \
   )

/*
 * __huz_bst_foreach_entry(cur, type, member, start, traverse, ...typechecks)
 * --------------------------------------------------------------------------
 * Walk through containers instead of directly iterating over nodes.
 *
 * The container is a struct that embeds a tree node as one of its members.
 *
 * The iterator initially receives the node pointer, pretending to be
 * a container pointer:
 *
 *    (cur) = (type *)(start);
 *
 * This is intentional. At this point, cur is only used for the NULL check.
 *
 * Once the NULL check passes, container_of() converts the node pointer
 * into its actual containing struct:
 *
 *    cur = container_of(node, type, member);
 *
 * The ", 1" ensures that the loop body is not skipped if container_of()
 * theoretically returns NULL.
 *
 * This can happen if the node pointer equals the offset of member:
 *
 *    node - offsetof(type, member) = NULL
 *
 * Unlikely in practice, but the compiler cannot necessarily prove
 * otherwise.
 *
 * Type checks ensure that:
 *    - cur has the expected container type.
 *    - start returns the expected node type.
 *    - traverse returns the expected node type.
 *
 * eg:
 *    __huz_bst_foreach_entry(cur, type, member, avl_first(root), avl_next(&(cur)->node))
 */
#define __huz_bst_foreach_entry(cur, type, member, start, traverse, ...)                              \
   for (                                                                                              \
      __VA_ARGS__, /* caller injected typechecks */                                                   \
      huz_typecheck(type, *(cur)),                                                                    \
      huz_typecheck(huz_typeof_member(type, member), *(start)),                                       \
      huz_typecheck(huz_typeof_member(type, member), *(traverse)),                                    \
      (cur) = (type *)(start);                                                                        \
      ((cur) != NULL)                                                                                 \
         && (((cur) = huz_container_of((huz_typeof_member(type, member) *)(cur), type, member)), 1);  \
      (cur) = (type *)(traverse)                                                                      \
   )

/*
 * __huz_bst_foreach_entry_safe(cur, tmp, type, member, start, traverse, ...typechecks)
 * ------------------------------------------------------------------------------------
 * Combines __huz_bst_foreach_safe and __huz_bst_foreach_entry.
 *
 * Walk through containers while allowing the current container to be
 * deleted during iteration.
 *
 * The condition deliberately performs these operations in order:
 *
 *    1. Check whether cur is NULL.
 *    2. Convert the node pointer into its containing struct.
 *    3. Calculate and store the next container pointer in tmp.
 *
 * The order is important because the caller can write:
 *
 *    avl_next(&(cur)->member)
 *
 * Instead of:
 *
 *    avl_next((struct avl_node *)(cur))
 *
 * This keeps the traversal expression consistent with the container
 * iterator and avoids confusing casts in the caller's code.
 *
 * Both ", 1" expressions are intentional:
 *
 *    ((cur) = container_of(...), 1)
 *    ((tmp) = (type *)(traverse), 1)
 *
 * They ensure that neither a NULL result from container_of() nor a NULL
 * result from traverse() causes the current loop body to be skipped.
 *
 * The next container is saved before entering the body, allowing cur
 * to be safely deleted.
 *
 * eg:
 *    __huz_bst_foreach_entry_safe(cur, tmp, type, member, avl_first(root), avl_next(&(cur)->node))
 */
#define __huz_bst_foreach_entry_safe(cur, tmp, type, member, start, traverse, ...)                    \
   for (                                                                                              \
      __VA_ARGS__, /* caller injected typechecks */                                                   \
      huz_typecheck(type, *(cur)),                                                                    \
      huz_typecheck(type, *(tmp)),                                                                    \
      huz_typecheck(huz_typeof_member(type, member), *(start)),                                       \
      huz_typecheck(huz_typeof_member(type, member), *(traverse)),                                    \
      (cur) = (type *)(start);                                                                        \
      ((cur) != NULL)                                                                                 \
         && (((cur) = huz_container_of((huz_typeof_member(type, member) *)(cur), type, member)), 1)   \
         && (((tmp) = (type *)(traverse)), 1);                                                        \
      (cur) = (tmp)                                                                                   \
   )



/*
 * Generic BST traversal types
 * ---------------------------
 * AVL, Red-Black, and Splay trees share the same traversal logic.
 * Rather than implementing traversal functions three times, we use
 * a common internal node type: struct huz_bst_node.
 *
 * Each tree implementation wraps the shared traversal functions and
 * supplies its own parent-unpacking callback.
 *
 * The internal node layout is designed to match the first member
 * of each tree's node structure.
 *
 * This is inspired by the generic-type trick used in linux/min_heap.h.
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * struct huz_bst_node
 * -------------------
 * Internal representation shared by all BST implementations.
 *
 * The first member stores the packed parent pointer as a raw uintptr_t.
 * This is intentional: AVL and Red-Black trees use the low bits of
 * the parent pointer to store additional metadata.
 *
 * Keeping the raw representation allows the shared traversal code
 * to work without knowing how each tree packs its parent pointer.
 *
 * Each tree supplies its own get_parent() callback to unpack it.
 *
 * The layout must match the corresponding first member in:
 *    - struct avl_node
 *    - struct rb_node
 *    - struct splay_node
 *
 * Conversion between the concrete node types and bst_node is done
 * through their shared first member.
 */
struct huz_bst_node
{
   uintptr_t __packed_parent;
   struct huz_bst_node *left, *right;
};

#define huz_bst_node_cast(node, parent_field)         huz_container_of(&(node)->parent_field, struct huz_bst_node, __packed_parent)
#define huz_bst_node_recast(type, node, parent_field) huz_container_of(&(node)->__packed_parent, type, parent_field)


/*
 * struct huz_bst_node_linked
 * --------------------------
 * Extended BST node with linked-list pointers.
 *
 * Useful when a BST implementation also needs to maintain a linked
 * list of nodes, for example, to support ordered iteration without
 * repeatedly traversing the tree.
 *
 * The bst_node remains the first member, preserving compatibility
 * with the shared BST traversal infrastructure.
 */
struct huz_bst_node_linked
{
   struct huz_bst_node node;
   struct huz_bst_node_linked *prev, *next;
};

#define huz_bst_node_linked_cast(_node, parent_field)          huz_container_of(&(_node)->node, struct huz_bst_node_linked, node)
#define huz_bst_node_linked_recast(type, _node, parent_field)  huz_container_of(&(_node)->node, type, node)



/* ------------------------------------------------------------- */
/* --------------------- helper functions  --------------------- */
/* ------------------------------------------------------------- */

/*
 * __huz_bst_parent_ptr(root_ptr, node, parent, isleft)
 * ----------------------------------------------------
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
HUZ_INTERNAL_API __huz_pure__
struct huz_bst_node **__huz_bst_parent_ptr(struct huz_bst_node **restrict root_link, const struct huz_bst_node *restrict node, const struct huz_bst_node *restrict parent, bool *restrict isleft)
__huz_reproducible__
{
   huz_assert(root_link && node && isleft);

   if (!parent)
   {
      // isleft's value doesn't matter when node is root
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

#define huz_bst_parent_ptr(root_link, node, parent, isleft, parent_field)  \
   ((huz_typeof(*(node)) **)__huz_bst_parent_ptr(                          \
      (struct huz_bst_node **)(root_link),                                 \
      huz_bst_node_cast(node, __parent_field),                             \
      huz_bst_node_cast(parent, __parent_field),                           \
      isleft                                                               \
   ))


/*
 * __huz_bst_delink_node(child, parent, link, set_parent)
 * ------------------------------------------------------
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
HUZ_INTERNAL_API
void __huz_bst_delink_node(struct huz_bst_node *restrict child, struct huz_bst_node *restrict parent, struct huz_bst_node **restrict link, void (*set_parent)(struct huz_bst_node *restrict, const struct huz_bst_node *restrict))
{
   huz_assert(link && ((!parent) || (parent->left == *link) || (parent->right == *link)));
   huz_assert(((*link)->left == child) || ((*link)->right == child));
   *link = child;
   if (child)
      set_parent(child, parent);
}

#define huz_bst_delink_node(child, parent, link, set_parent, parent_field) \
   __huz_bst_delink_node(                                                  \
      huz_bst_node_cast(child, parent_field),                              \
      huz_bst_node_cast(parent, parent_field),                             \
      (struct huz_bst_node **)(link),                                      \
      set_parent,                                                          \
   )


/*
 * __huz_bst_replace_node(old, new, link)
 * --------------------------------------
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
HUZ_INTERNAL_API
void __huz_bst_replace_node(struct huz_bst_node *restrict old, struct huz_bst_node *restrict new, struct huz_bst_node **restrict link)
{
   huz_assert(old && new && link && (*link == old));
   *link = new;
   new->__packed_parent = old->__packed_parent;
}

#define huz_bst_replace_node(old, new, link, parent_field)  \
   __huz_bst_replace_node(                                  \
      huz_bst_node_cast(old, parent_field),                 \
      huz_bst_node_cast(new, parent_field),                 \
      (struct huz_bst_node **)(link)                        \
   )



/* ------------------------------------------------------------- */
/* --------------------- linked functions  --------------------- */
/* ------------------------------------------------------------- */

/*
 * __huz_bst_add_linked(new, prev, next)
 * -------------------------------------
 * Inserts 'new' between 'prev' and 'next' in a doubly-linked list.
 *
 *      prev         next                 prev        new         next
 *       o --------- o                     o --------- o --------- o
 *
 * @new:   node to insert, not NULL
 * @prev:  predecessor node, may be NULL (insert at head)
 * @next:  successor node, may be NULL (insert at tail)
 */
HUZ_INTERNAL_API
void __huz_bst_add_linked(struct huz_bst_node_linked *restrict new, struct huz_bst_node_linked *restrict prev, struct huz_bst_node_linked *restrict next)
{
   huz_assert(new);

   if (__huz_likely__(prev))
      prev->next = new;

   if (__huz_likely__(next))
      next->prev = new;

   new->prev = prev;
   new->next = next;
}

#define huz_bst_add_linked(new, prev, next, parent_field)   \
   __huz_bst_add_linked(                                    \
      huz_bst_node_linked_cast(new, parent_field),          \
      huz_bst_node_linked_cast(prev, parent_field),         \
      huz_bst_node_linked_cast(next, parent_field)          \
   )


/*
 * __huz_bst_rm_linked(prev, next)
 * -------------------------------
 * Removes the node between 'prev' and 'next' by linking them directly.
 *
 *     prev         node        next                  prev         next
 *       o --------- o --------- o         ==>          o --------- o
 *
 * @prev:  predecessor of the node being removed, may be NULL
 * @next:  successor of the node being removed, may be NULL
 */
HUZ_INTERNAL_API
void __huz_bst_rm_linked(struct huz_bst_node_linked *restrict prev, struct huz_bst_node_linked *restrict next)
{
   if (__huz_likely__(prev))
      prev->next = next;

   if (__huz_likely__(next))
      next->prev = prev;
}

#define huz_rm_add_linked(prev, next, parent_field)   \
   __huz_bst_rm_linked(                               \
      huz_bst_node_linked_cast(prev, parent_field),   \
      huz_bst_node_linked_cast(next, parent_field)    \
   )



/* ------------------------------------------------------------- */
/* --------------------- rotate functions  --------------------- */
/* ------------------------------------------------------------- */

/*
 * __huz_bst_rotate_left(node, child, link, set_parent)
 * ----------------------------------------------------
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
HUZ_INTERNAL_API
void __huz_bst_rotate_left(struct huz_bst_node *restrict node, struct huz_bst_node *restrict child, struct huz_bst_node **restrict link, void (*set_parent)(struct huz_bst_node *restrict, const struct huz_bst_node *restrict))
{
   huz_assert(node && child && link && (*link == node) && (node->right == child));

   if (child->left)
      set_parent(child->left, node);

   node->right = child->left;
   child->left = node;

   *link = child;
}

#define huz_bst_rotate_left(node, child, link, set_parent, parent_field)   \
   __huz_bst_rotate_left(                                                  \
      huz_bst_node_cast(node, parent_field),                               \
      huz_bst_node_cast(child, parent_field),                              \
      (struct huz_bst_node **)(link),                                      \
      set_parent                                                           \
   )


/*
 * __huz_bst_rotate_right(node, child, link, set_parent)
 * -----------------------------------------------------
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
HUZ_INTERNAL_API
void __huz_bst_rotate_right(struct huz_bst_node *restrict node, struct huz_bst_node *restrict child, struct huz_bst_node **restrict link, void (*set_parent)(struct huz_bst_node *restrict, const struct huz_bst_node *restrict))
{
   huz_assert(node && child && link && (*link == node) && (node->left == child));

   if (child->right)
      set_parent(child->right, node);

   node->left = child->right;
   child->right = node;

   *link = child;
}

/*
 * __huz_bst_rotate_left_left(parent, node, child, link, set_parent)
 * -----------------------------------------------------------------
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
HUZ_INTERNAL_API
void __huz_bst_rotate_left_left(struct huz_bst_node *restrict parent, struct huz_bst_node *restrict node, struct huz_bst_node *restrict child, struct huz_bst_node **restrict link, void (*set_parent)(struct huz_bst_node *restrict, const struct huz_bst_node *restrict))
{
   huz_assert(node && child && parent && link && (*link == parent) && (node->right == child) && (parent->right == node));

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
 * __huz_bst_rotate_right_right(parent, node, child, link, set_parent)
 * -------------------------------------------------------------------
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
HUZ_INTERNAL_API
void __huz_bst_rotate_right_right(struct huz_bst_node *restrict parent, struct huz_bst_node *restrict node, struct huz_bst_node *restrict child, struct huz_bst_node **restrict link, void (*set_parent)(struct huz_bst_node *restrict, const struct huz_bst_node *restrict))
{
   huz_assert(node && child && parent && link && (*link == parent) && (node->left == child) && (parent->left == node));

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
 * __huz_bst_rotate_left_right(parent, node, child, link, set_parent)
 * ------------------------------------------------------------------
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
HUZ_INTERNAL_API
void __huz_bst_rotate_left_right(struct huz_bst_node *restrict parent, struct huz_bst_node *restrict node, struct huz_bst_node *restrict child, struct huz_bst_node **restrict link, void (*set_parent)(struct huz_bst_node *restrict, const struct huz_bst_node *restrict))
{
   huz_assert(node && child && parent && link && (*link == parent) && (node->right == child) && (parent->left == node));

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
 * __huz_bst_rotate_right_left(parent, node, child, link, set_parent)
 * ------------------------------------------------------------------
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
HUZ_INTERNAL_API
void __huz_bst_rotate_right_left(struct huz_bst_node *restrict parent, struct huz_bst_node *restrict node, struct huz_bst_node *restrict child, struct huz_bst_node **restrict link, void (*set_parent)(struct huz_bst_node *restrict, const struct huz_bst_node *restrict))
{
   huz_assert(node && child && parent && link && (*link == parent) && (node->left == child) && (parent->right == node));

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
 * __huz_bst_first(node)
 * ------------------------
 * first node in in-order traversal with subtree boundary.
 *
 * @node: sub-tree root node (root->node for full-tree)
 */
HUZ_BST_INTERNAL const struct bst_node *__huz_bst_first(const struct bst_node *restrict node)
{
   __huz_assert(node);
   while (node->left)
      node = node->left;
   return node;
}

/*
 * __huz_bst_last(node)
 * -----------------------
 * last node in in-order traversal with subtree boundary.
 *
 * @node: sub-tree root node (root->node for full-tree)
 */
HUZ_BST_INTERNAL const struct bst_node *__huz_bst_last(const struct bst_node *restrict node)
{
   __huz_assert(node);
   while (node->right)
      node = node->right;
   return node;
}

/*
 * __huz_bst_postorder_first(node)
 * ----------------------------------
 * first node in post-order traversal with subtree boundary.
 *
 * @node: sub-tree root node (root->node for full-tree)
 */
HUZ_BST_INTERNAL const struct bst_node *__huz_bst_postorder_first(const struct bst_node *restrict node)
{
   __huz_assert(node);

   node = __huz_bst_first(node);
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
 * __huz_bst_next(subroot_parent, node, get_parent)
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
HUZ_BST_INTERNAL const struct bst_node *__huz_bst_next(const struct bst_node *restrict subroot_parent, const struct bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   __huz_assert(node && get_parent);

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
      return (__huz_unlikely__(parent == subroot_parent)) ? NULL : parent;
   }
}

/*
 * __huz_bst_prev(subroot_parent, node, get_parent)
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
HUZ_BST_INTERNAL const struct bst_node *__huz_bst_prev(const struct bst_node *restrict subroot_parent, const struct bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   __huz_assert(node && get_parent);

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
      return (__huz_unlikely__(parent == subroot_parent)) ? NULL : parent;
   }
}

/*
 * __huz_bst_preorder_next(subroot_parent, node, get_parent)
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
HUZ_BST_INTERNAL const struct bst_node *__huz_bst_preorder_next(const struct bst_node *restrict subroot_parent, const struct bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   __huz_assert(node && get_parent);

   if (node->left)
      return node->left;

   else if (node->right)
      return node->right;

   else
      while (true)
      {
         const struct bst_node *restrict parent = get_parent(node->__packed_parent);

         if (__huz_unlikely__(parent == subroot_parent))
            return NULL;

         else if (node == parent->left && parent->right)
            return parent->right;

         else
            node = parent;
      }
}

/*
 * __huz_bst_postorder_next(subroot_parent, node, get_parent)
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
HUZ_BST_INTERNAL const struct bst_node *__huz_bst_postorder_next(const struct bst_node *restrict subroot_parent, const struct bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   __huz_assert(node && get_parent);
   const struct bst_node *restrict parent = get_parent(node->__packed_parent);

   if (__huz_unlikely__(parent == subroot_parent))
      return NULL;

   else if (node == parent->left && parent->right)
      return __huz_bst_postorder_first(parent->right);

   else
      return parent;
}



#endif /* HUZ_BST_H */
