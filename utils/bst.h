#ifndef HUZ_BST_H
#define HUZ_BST_H


#if !(                              \
   defined(huz_typeof) &&           \
   defined(huz_typeof_member) &&    \
   defined(huz_typecheck) &&        \
   defined(huz_types_equal) &&      \
   defined(huz_container_of) &&     \
   defined(huz_assert) &&           \
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
#define __huz_bst_foreach(node, start, traverse, ...)                \
   for (                                                             \
      __VA_ARGS__, /* caller injected typechecks */                  \
      huz_types_equal(huz_typeof(*(start)), huz_typeof(*(node))),    \
      huz_types_equal(huz_typeof(*(traverse)), huz_typeof(*(node))), \
      (node) = (start);                                              \
      (node) != NULL;                                                \
      (node) = (traverse)                                            \
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
#define __huz_bst_foreach_safe(node, tmp, start, traverse, ...)      \
   for (                                                             \
      __VA_ARGS__, /* caller injected typechecks */                  \
      huz_types_equal(huz_typeof(*(tmp)), huz_typeof(*(node))),      \
      huz_types_equal(huz_typeof(*(start)), huz_typeof(*(node))),    \
      huz_types_equal(huz_typeof(*(traverse)), huz_typeof(*(node))), \
      (node) = (start);                                              \
      ((node) != NULL)                                               \
         && (((tmp) = (traverse)), 1);                               \
      (node) = (tmp)                                                 \
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
 * Conversion between the concrete node types and huz_bst_node is done
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
 * The huz_bst_node remains the first member, preserving compatibility
 * with the shared BST traversal infrastructure.
 */
struct huz_bst_node_linked
{
   struct huz_bst_node node;
   struct huz_bst_node_linked *prev, *next;
};

#define huz_bst_node_linked_cast(_node, parent_field)  \
   huz_container_of(huz_bst_node_cast(&(_node)->node, parent_field), struct huz_bst_node_linked, node)
#define huz_bst_node_linked_recast(type, _node, parent_field)  \
   huz_container_of(huz_bst_node_recast(huz_typeof_member(type, node), &(_node)->node, parent_field), type, node)



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
HUZ_INTERNAL_API
struct huz_bst_node **__huz_bst_parent_ptr(struct huz_bst_node **restrict root_link, struct huz_bst_node *restrict node, struct huz_bst_node *restrict parent, bool *restrict isleft)
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
      huz_bst_node_cast(node, parent_field),                               \
      huz_bst_node_cast(parent, parent_field),                             \
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
      set_parent                                                           \
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

#define huz_bst_rm_linked(prev, next, parent_field)   \
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

#define huz_bst_rotate_right(node, child, link, set_parent, parent_field)  \
   __huz_bst_rotate_right(                                                 \
      huz_bst_node_cast(node, parent_field),                               \
      huz_bst_node_cast(child, parent_field),                              \
      (struct huz_bst_node **)(link),                                      \
      set_parent                                                           \
   )


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

#define huz_bst_rotate_left_left(parent, node, child, link, set_parent, parent_field)   \
   __huz_bst_rotate_left_left(                                                          \
      huz_bst_node_cast(parent, parent_field),                                          \
      huz_bst_node_cast(node, parent_field),                                            \
      huz_bst_node_cast(child, parent_field),                                           \
      (struct huz_bst_node **)(link),                                                   \
      set_parent                                                                        \
   )


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

#define huz_bst_rotate_right_right(parent, node, child, link, set_parent, parent_field) \
   __huz_bst_rotate_right_right(                                                        \
      huz_bst_node_cast(parent, parent_field),                                          \
      huz_bst_node_cast(node, parent_field),                                            \
      huz_bst_node_cast(child, parent_field),                                           \
      (struct huz_bst_node **)(link),                                                   \
      set_parent                                                                        \
   )


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

#define huz_bst_rotate_left_right(parent, node, child, link, set_parent, parent_field)  \
   __huz_bst_rotate_left_right(                                                         \
      huz_bst_node_cast(parent, parent_field),                                          \
      huz_bst_node_cast(node, parent_field),                                            \
      huz_bst_node_cast(child, parent_field),                                           \
      (struct huz_bst_node **)(link),                                                   \
      set_parent                                                                        \
   )


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

#define huz_bst_rotate_right_left(parent, node, child, link, set_parent, parent_field)  \
   __huz_bst_rotate_right_left(                                                         \
      huz_bst_node_cast(parent, parent_field),                                          \
      huz_bst_node_cast(node, parent_field),                                            \
      huz_bst_node_cast(child, parent_field),                                           \
      (struct huz_bst_node **)(link),                                                   \
      set_parent                                                                        \
   )



/* ------------------------------------------------------------- */
/* --------------- full-tree / sub-tree queries  --------------- */
/* ------------------------------------------------------------- */

/*
 * __huz_bst_first(node)
 * ---------------------
 * first node in in-order traversal with subtree boundary.
 *
 * @node: sub-tree root node (root->node for full-tree)
 */
HUZ_INTERNAL_API
const struct huz_bst_node *__huz_bst_first(const struct huz_bst_node *restrict node)
{
   huz_assert(node);
   while (node->left)
      node = node->left;
   return node;
}

#define huz_bst_first(node, parent_field)                      \
   huz_bst_node_recast(                                        \
      huz_typeof(*(node)),                                     \
      __huz_bst_first(huz_bst_node_cast(node, parent_field)),  \
      parent_field                                             \
   )


/*
 * __huz_bst_last(node)
 * --------------------
 * last node in in-order traversal with subtree boundary.
 *
 * @node: sub-tree root node (root->node for full-tree)
 */
HUZ_INTERNAL_API
const struct huz_bst_node *__huz_bst_last(const struct huz_bst_node *restrict node)
{
   huz_assert(node);
   while (node->right)
      node = node->right;
   return node;
}

#define huz_bst_last(node, parent_field)                       \
   huz_bst_node_recast(                                        \
      huz_typeof(*(node)),                                     \
      __huz_bst_last(huz_bst_node_cast(node, parent_field)),   \
      parent_field                                             \
   )


/*
 * __huz_bst_postorder_first(node)
 * -------------------------------
 * first node in post-order traversal with subtree boundary.
 *
 * @node: sub-tree root node (root->node for full-tree)
 */
HUZ_INTERNAL_API
const struct huz_bst_node *__huz_bst_postorder_first(const struct huz_bst_node *restrict node)
{
   huz_assert(node);

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

#define huz_bst_postorder_first(node, parent_field)                     \
   huz_bst_node_recast(                                                 \
      huz_typeof(*(node)),                                              \
      __huz_bst_postorder_first(huz_bst_node_cast(node, parent_field)), \
      parent_field                                                      \
   )



/* -------------------------------------------------------------- */
/* -------------- full-tree / sub-tree traversals  -------------- */
/* -------------------------------------------------------------- */

/*
 * __huz_bst_next(subroot_parent, node, get_parent)
 * ------------------------------------------------
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
HUZ_INTERNAL_API
const struct huz_bst_node *__huz_bst_next(const struct huz_bst_node *restrict subroot_parent, const struct huz_bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   huz_assert(node && get_parent);

   if (node->right)
   {
      node = node->right;
      while (node->left)
         node = node->left;
      return node;
   }
   else
   {
      const struct huz_bst_node *restrict parent = get_parent(node->__packed_parent);
      while (parent != subroot_parent && node == parent->right)
      {
         node = parent;
         parent = get_parent(parent->__packed_parent);
      }
      return (__huz_unlikely__(parent == subroot_parent)) ? NULL : parent;
   }
}

#define huz_bst_next(subroot_parent, node, get_parent, parent_field)  \
   huz_bst_node_recast(                                               \
      huz_typeof(*(node)),                                            \
      __huz_bst_next(                                                 \
         huz_bst_node_cast(subroot_parent, parent_field),             \
         huz_bst_node_cast(node, parent_field),                       \
         get_parent                                                   \
      ),                                                              \
      parent_field                                                    \
   )


/*
 * __huz_bst_prev(subroot_parent, node, get_parent)
 * ------------------------------------------------
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
HUZ_INTERNAL_API
const struct huz_bst_node *__huz_bst_prev(const struct huz_bst_node *restrict subroot_parent, const struct huz_bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   huz_assert(node && get_parent);

   if (node->left)
   {
      node = node->left;
      while (node->right)
         node = node->right;
      return node;
   }
   else
   {
      const struct huz_bst_node *restrict parent = get_parent(node->__packed_parent);
      while (parent != subroot_parent && node == parent->left)
      {
         node = parent;
         parent = get_parent(parent->__packed_parent);
      }
      return (__huz_unlikely__(parent == subroot_parent)) ? NULL : parent;
   }
}

#define huz_bst_prev(subroot_parent, node, get_parent, parent_field)  \
   huz_bst_node_recast(                                               \
      huz_typeof(*(node)),                                            \
      __huz_bst_prev(                                                 \
         huz_bst_node_cast(subroot_parent, parent_field),             \
         huz_bst_node_cast(node, parent_field),                       \
         get_parent                                                   \
      ),                                                              \
      parent_field                                                    \
   )


/*
 * __huz_bst_preorder_next(subroot_parent, node, get_parent)
 * ---------------------------------------------------------
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
HUZ_INTERNAL_API
const struct huz_bst_node *__huz_bst_preorder_next(const struct huz_bst_node *restrict subroot_parent, const struct huz_bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   huz_assert(node && get_parent);

   if (node->left)
      return node->left;

   else if (node->right)
      return node->right;

   else
      while (true)
      {
         const struct huz_bst_node *restrict parent = get_parent(node->__packed_parent);

         if (__huz_unlikely__(parent == subroot_parent))
            return NULL;

         else if (node == parent->left && parent->right)
            return parent->right;

         else
            node = parent;
      }
}

#define huz_bst_preorder_next(subroot_parent, node, get_parent, parent_field) \
   huz_bst_node_recast(                                                       \
      huz_typeof(*(node)),                                                    \
      __huz_bst_preorder_next(                                                \
         huz_bst_node_cast(subroot_parent, parent_field),                     \
         huz_bst_node_cast(node, parent_field),                               \
         get_parent                                                           \
      ),                                                                      \
      parent_field                                                            \
   )


/*
 * __huz_bst_postorder_next(subroot_parent, node, get_parent)
 * ----------------------------------------------------------
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
HUZ_INTERNAL_API
const struct huz_bst_node *__huz_bst_postorder_next(const struct huz_bst_node *restrict subroot_parent, const struct huz_bst_node *restrict node, void *(*get_parent)(const uintptr_t))
{
   huz_assert(node && get_parent);
   const struct huz_bst_node *restrict parent = get_parent(node->__packed_parent);

   if (__huz_unlikely__(parent == subroot_parent))
      return NULL;

   else if (node == parent->left && parent->right)
      return __huz_bst_postorder_first(parent->right);

   else
      return parent;
}

#define huz_bst_postorder_next(subroot_parent, node, get_parent, parent_field)   \
   huz_bst_node_recast(                                                          \
      huz_typeof(*(node)),                                                       \
      __huz_bst_postorder_next(                                                  \
         huz_bst_node_cast(subroot_parent, parent_field),                        \
         huz_bst_node_cast(node, parent_field),                                  \
         get_parent                                                              \
      ),                                                                         \
      parent_field                                                               \
   )


#endif /* HUZ_BST_H */




#ifdef HUZ_BST_TEST

/*
 * sample test command
 * ------------
 * gcc -std=c11 -DHUZ_BST_TEST \
 *     -iquote utils \
 *     -Ilib \
 *     -include utils/types.h \
 *     -include utils/hints.h \
 *     -include utils/assert.h \
 *     -include utils/api.h \
 *     -x c utils/bst.h \
 *     -x none lib/*.c \
 *     -o test
 */

#include "unity.h"
#include "pcg_basic.h"

/*
 * struct mock_node
 * ----------------
 * Stand-in for a concrete tree node (avl_node, rb_node, ...).
 * '__parent' mirrors huz_bst_node's packed parent as the first member.
 */
struct mock_node
{
   uintptr_t __parent;
   struct mock_node *left, *right;
};

/*
 * struct mock_node_linked
 * -----------------------
 * Stand-in for a concrete linked tree node.
 */
struct mock_node_linked
{
   struct mock_node node;
   struct mock_node_linked *prev, *next;
};


/*
 * MOCK_PARENT_SALT
 * ----------------
 * Low bits forced on in every packed parent, mimicking the AVL balance /
 * RB colour bits. Any code path that reads '__packed_parent' raw instead of
 * going through get_parent() will dereference a misaligned pointer and fail.
 */
#define MOCK_PARENT_SALT ((uintptr_t)0x3)

static void *mock_get_parent(const uintptr_t __parent)
{
   return (void *)(__parent & ~MOCK_PARENT_SALT);
}

static void mock_set_parent(struct mock_node *restrict node, const struct mock_node *restrict parent)
{
   huz_assert(node);
   node->__parent = (uintptr_t)parent | MOCK_PARENT_SALT;
}

static void mock_set_parent_bst(struct huz_bst_node *restrict node, const struct huz_bst_node *restrict parent)
{
   mock_set_parent(
      huz_bst_node_recast(struct mock_node, node, __parent),
      huz_bst_node_recast(struct mock_node, parent, __parent)
   );
}


/*
 * mock_setup(node, left, right, parent)
 * -------------------------------------
 * sets up 'node' with children 'left' & 'right' and parent 'parent'
 *
 * @node:    mock node
 * @left:    left child to be set
 * @right:   right child to be set
 * @parent:  parent to be set
 */
static void mock_setup(struct mock_node *node, struct mock_node *left, struct mock_node *right, struct mock_node *parent)
{
   huz_assert(node);
   node->left = left;
   node->right = right;
   mock_set_parent(node, parent);
}

/*
 * mock_setup_linked(node, left, right, parent, prev, next)
 * --------------------------------------------------------
 * sets up 'node' with children 'left' & 'right', parent 'parent',
 * and in-order 'prev' and 'next'
 *
 * @node:    mock node
 * @left:    left child to be set
 * @right:   right child to be set
 * @parent:  parent to be set
 * @prev:    in-order predecessor
 * @next:    in-order successor
 */
static void mock_setup_linked(struct mock_node_linked *node, struct mock_node_linked *left, struct mock_node_linked *right, struct mock_node_linked *parent, struct mock_node_linked *prev, struct mock_node_linked *next)
{
   huz_assert(node);
   mock_setup(
      &node->node,
      left   ? &left->node   : NULL,
      right  ? &right->node  : NULL,
      parent ? &parent->node : NULL
   );
   node->next = next;
   node->prev = prev;
}

/*
 * mock_setup_tree17(n)
 * --------------------
 * Builds the shared 17-node tree, in-order key == array index:
 *
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
#define MOCK_TREE17_SIZE 17

static void mock_setup_tree17(struct mock_node n[MOCK_TREE17_SIZE])
{
   mock_setup(n + 10, n + 8,  n + 13, NULL);
   mock_setup(n + 8,  n + 4,  n + 9,  n + 10);
   mock_setup(n + 13, n + 11, n + 14, n + 10);
   mock_setup(n + 4,  n + 0,  n + 5,  n + 8);
   mock_setup(n + 9,  NULL,   NULL,   n + 8);
   mock_setup(n + 11, NULL,   n + 12, n + 13);
   mock_setup(n + 14, NULL,   n + 16, n + 13);
   mock_setup(n + 0,  NULL,   n + 2,  n + 4);
   mock_setup(n + 5,  NULL,   n + 7,  n + 4);
   mock_setup(n + 12, NULL,   NULL,   n + 11);
   mock_setup(n + 16, n + 15, NULL,   n + 14);
   mock_setup(n + 2,  n + 1,  n + 3,  n + 0);
   mock_setup(n + 7,  n + 6,  NULL,   n + 5);
   mock_setup(n + 15, NULL,   NULL,   n + 16);
   mock_setup(n + 1,  NULL,   NULL,   n + 2);
   mock_setup(n + 3,  NULL,   NULL,   n + 2);
   mock_setup(n + 6,  NULL,   NULL,   n + 7);
}

#define MOCK_NULL ((struct mock_node *)NULL)


void setUp(void) {}
void tearDown(void) {}



/* ------------------------------------------------------------- */
/* ----------------------- helper tests  ----------------------- */
/* ------------------------------------------------------------- */

static void test_bst_parent_ptr(void)
{
   struct mock_node node, parent, *root, **res;
   bool isleft;


   // CASE 1: node is root
   mock_setup(&node, NULL, NULL, NULL);

   res = huz_bst_parent_ptr(&root, &node, MOCK_NULL, &isleft, __parent);
   TEST_ASSERT_EQUAL_PTR(&root, res);


   // CASE 2: node is left child
   mock_setup(&node, NULL, NULL, &parent);
   mock_setup(&parent, &node, NULL, NULL);

   res = huz_bst_parent_ptr(&root, &node, &parent, &isleft, __parent);
   TEST_ASSERT_EQUAL_PTR(&parent.left, res);
   TEST_ASSERT_TRUE(isleft);


   // CASE 3: node is right child
   mock_setup(&node, NULL, NULL, &parent);
   mock_setup(&parent, NULL, &node, NULL);

   res = huz_bst_parent_ptr(&root, &node, &parent, &isleft, __parent);
   TEST_ASSERT_EQUAL_PTR(&parent.right, res);
   TEST_ASSERT_FALSE(isleft);
}

static void test_bst_delink_node(void)
{
   struct mock_node node, parent, child, t, u, g;

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
   mock_setup(&g, &parent, NULL, NULL);
   mock_setup(&parent, NULL, &node, &g);
   mock_setup(&node, &child, NULL, &parent);
   mock_setup(&child, &t, &u, &node);
   mock_setup(&t, NULL, NULL, &child);
   mock_setup(&u, NULL, NULL, &child);

   huz_bst_delink_node(&child, &parent, &parent.right, mock_set_parent_bst, __parent);

   TEST_ASSERT_EQUAL_PTR(&child, parent.right);
   TEST_ASSERT_EQUAL_PTR(&parent, mock_get_parent(child.__parent));
}

static void test_bst_replace_node(void)
{
   struct mock_node node, succ, parent, tmp, w;


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
   mock_setup(&parent, NULL, &node, NULL);
   mock_setup(&node, &w, &tmp, &parent);
   mock_setup(&w, NULL, NULL, &node);
   mock_setup(&tmp, &succ, NULL, &node);
   mock_setup(&succ, NULL, NULL, &tmp);

   huz_bst_replace_node(&node, &succ, &parent.right, __parent);

   TEST_ASSERT_EQUAL_PTR(&parent, mock_get_parent(succ.__parent));
   TEST_ASSERT_EQUAL_PTR(&succ, parent.right);


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
   mock_setup(&parent, NULL, &node, NULL);
   mock_setup(&node, &w, &succ, &parent);
   mock_setup(&w, NULL, NULL, &node);
   mock_setup(&succ, NULL, &tmp, &node);
   mock_setup(&tmp, NULL, NULL, &succ);

   huz_bst_replace_node(&node, &succ, &parent.right, __parent);

   TEST_ASSERT_EQUAL_PTR(&parent, mock_get_parent(succ.__parent));
   TEST_ASSERT_EQUAL_PTR(&succ, parent.right);
   TEST_ASSERT_EQUAL_PTR(&tmp, succ.right);
}



/* ------------------------------------------------------------- */
/* ----------------------- linked tests  ----------------------- */
/* ------------------------------------------------------------- */

static void test_bst_add_rm_linked(void)
{
   struct mock_node_linked a, b, n;

   /*
    * TEST ADD:
    *
    *   a -- b
    */
   mock_setup_linked(&a, NULL, NULL, NULL, NULL, &b);
   mock_setup_linked(&b, NULL, NULL, NULL, &a, NULL);
   mock_setup_linked(&n, NULL, NULL, NULL, NULL, NULL);

   huz_bst_add_linked(&n, &a, &b, __parent);

   TEST_ASSERT_EQUAL_PTR(&a, n.prev);
   TEST_ASSERT_EQUAL_PTR(&b, n.next);

   TEST_ASSERT_EQUAL_PTR(&n, a.next);
   TEST_ASSERT_NULL(a.prev);

   TEST_ASSERT_NULL(b.next);
   TEST_ASSERT_EQUAL_PTR(&n, b.prev);

   /*
    * TEST RM:
    *
    *  a -- n -- b
    */
   huz_bst_rm_linked(&a, &b, __parent);

   TEST_ASSERT_EQUAL_PTR(&b, a.next);
   TEST_ASSERT_NULL(a.prev);

   TEST_ASSERT_NULL(b.next);
   TEST_ASSERT_EQUAL_PTR(&a, b.prev);
}



/* ------------------------------------------------------------- */
/* ----------------------- rotate tests  ----------------------- */
/* ------------------------------------------------------------- */

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
   struct mock_node p, n, c, x, y, z;
   mock_setup(&p, NULL, &n,   NULL);
   mock_setup(&n, &x,   &c,   &p);
   mock_setup(&c, &y,   &z,   &n);
   mock_setup(&x, NULL, NULL, &n);
   mock_setup(&y, NULL, NULL, &c);
   mock_setup(&z, NULL, NULL, &c);

   huz_bst_rotate_left(&n, &c, &p.right, mock_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL_PTR(&c, p.right);

   /* c takes n's place */
   TEST_ASSERT_EQUAL_PTR(&n, c.left);
   TEST_ASSERT_EQUAL_PTR(&z, c.right);

   /* n gets c's old left child */
   TEST_ASSERT_EQUAL_PTR(&x, n.left);
   TEST_ASSERT_EQUAL_PTR(&y, n.right);

   /* parent pointer of adopted child updated */
   TEST_ASSERT_EQUAL_PTR(&n, mock_get_parent(y.__parent));
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
   struct mock_node p, n, c, x, y, z;
   mock_setup(&p, &n,   NULL, NULL);
   mock_setup(&n, &c,   &x,   &p);
   mock_setup(&c, &z,   &y,   &n);
   mock_setup(&x, NULL, NULL, &n);
   mock_setup(&y, NULL, NULL, &c);
   mock_setup(&z, NULL, NULL, &c);

   huz_bst_rotate_right(&n, &c, &p.left, mock_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL_PTR(&c, p.left);

   /* c takes n's place */
   TEST_ASSERT_EQUAL_PTR(&z, c.left);
   TEST_ASSERT_EQUAL_PTR(&n, c.right);

   /* n gets c's old right child */
   TEST_ASSERT_EQUAL_PTR(&y, n.left);
   TEST_ASSERT_EQUAL_PTR(&x, n.right);

   /* parent pointer of adopted child updated */
   TEST_ASSERT_EQUAL_PTR(&n, mock_get_parent(y.__parent));
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
   struct mock_node g, p, n, c, w, x, y, z;
   mock_setup(&g, &p,   NULL, NULL);
   mock_setup(&p, &w,   &n,   &g);
   mock_setup(&n, &x,   &c,   &p);
   mock_setup(&c, &y,   &z,   &n);
   mock_setup(&w, NULL, NULL, &p);
   mock_setup(&x, NULL, NULL, &n);
   mock_setup(&y, NULL, NULL, &c);
   mock_setup(&z, NULL, NULL, &c);

   huz_bst_rotate_left_left(&p, &n, &c, &g.left, mock_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL_PTR(&c, g.left);

   /* c is new root */
   TEST_ASSERT_EQUAL_PTR(&n, c.left);
   TEST_ASSERT_EQUAL_PTR(&z, c.right);

   /* n gets p and y */
   TEST_ASSERT_EQUAL_PTR(&p, n.left);
   TEST_ASSERT_EQUAL_PTR(&y, n.right);

   /* p keeps w, gets x */
   TEST_ASSERT_EQUAL_PTR(&w, p.left);
   TEST_ASSERT_EQUAL_PTR(&x, p.right);

   /* parent pointers of adopted children updated */
   TEST_ASSERT_EQUAL_PTR(&p, mock_get_parent(x.__parent));
   TEST_ASSERT_EQUAL_PTR(&n, mock_get_parent(y.__parent));
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
   struct mock_node g, p, n, c, w, x, y, z;
   mock_setup(&g, NULL, &p,   NULL);
   mock_setup(&p, &n,   &w,   &g);
   mock_setup(&n, &c,   &x,   &p);
   mock_setup(&c, &z,   &y,   &n);
   mock_setup(&w, NULL, NULL, &p);
   mock_setup(&x, NULL, NULL, &n);
   mock_setup(&y, NULL, NULL, &c);
   mock_setup(&z, NULL, NULL, &c);

   huz_bst_rotate_right_right(&p, &n, &c, &g.right, mock_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL_PTR(&c, g.right);

   /* c is new root */
   TEST_ASSERT_EQUAL_PTR(&z, c.left);
   TEST_ASSERT_EQUAL_PTR(&n, c.right);

   /* n gets y and p */
   TEST_ASSERT_EQUAL_PTR(&y, n.left);
   TEST_ASSERT_EQUAL_PTR(&p, n.right);

   /* p gets x, keeps w */
   TEST_ASSERT_EQUAL_PTR(&x, p.left);
   TEST_ASSERT_EQUAL_PTR(&w, p.right);

   /* parent pointers of adopted children updated */
   TEST_ASSERT_EQUAL_PTR(&p, mock_get_parent(x.__parent));
   TEST_ASSERT_EQUAL_PTR(&n, mock_get_parent(y.__parent));
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
   struct mock_node g, p, n, c, w, x, y, z;
   mock_setup(&g, &p,   NULL, NULL);
   mock_setup(&p, &n,   &w,   &g);
   mock_setup(&n, &x,   &c,   &p);
   mock_setup(&c, &y,   &z,   &n);
   mock_setup(&w, NULL, NULL, &p);
   mock_setup(&x, NULL, NULL, &n);
   mock_setup(&y, NULL, NULL, &c);
   mock_setup(&z, NULL, NULL, &c);

   huz_bst_rotate_left_right(&p, &n, &c, &g.left, mock_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL_PTR(&c, g.left);

   /* c is new root */
   TEST_ASSERT_EQUAL_PTR(&n, c.left);
   TEST_ASSERT_EQUAL_PTR(&p, c.right);

   /* n gets x and y */
   TEST_ASSERT_EQUAL_PTR(&x, n.left);
   TEST_ASSERT_EQUAL_PTR(&y, n.right);

   /* p gets z and w */
   TEST_ASSERT_EQUAL_PTR(&z, p.left);
   TEST_ASSERT_EQUAL_PTR(&w, p.right);

   /* parent pointers of adopted children updated */
   TEST_ASSERT_EQUAL_PTR(&n, mock_get_parent(y.__parent));
   TEST_ASSERT_EQUAL_PTR(&p, mock_get_parent(z.__parent));
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
   struct mock_node g, p, n, c, w, x, y, z;
   mock_setup(&g, NULL, &p,   NULL);
   mock_setup(&p, &w,   &n,   &g);
   mock_setup(&n, &c,   &x,   &p);
   mock_setup(&c, &y,   &z,   &n);
   mock_setup(&w, NULL, NULL, &p);
   mock_setup(&x, NULL, NULL, &n);
   mock_setup(&y, NULL, NULL, &c);
   mock_setup(&z, NULL, NULL, &c);

   huz_bst_rotate_right_left(&p, &n, &c, &g.right, mock_set_parent_bst, __parent);

   /* link updated */
   TEST_ASSERT_EQUAL_PTR(&c, g.right);

   /* c is new root */
   TEST_ASSERT_EQUAL_PTR(&p, c.left);
   TEST_ASSERT_EQUAL_PTR(&n, c.right);

   /* p gets w and y */
   TEST_ASSERT_EQUAL_PTR(&w, p.left);
   TEST_ASSERT_EQUAL_PTR(&y, p.right);

   /* n gets z and x */
   TEST_ASSERT_EQUAL_PTR(&z, n.left);
   TEST_ASSERT_EQUAL_PTR(&x, n.right);

   /* parent pointers of adopted children updated */
   TEST_ASSERT_EQUAL_PTR(&p, mock_get_parent(y.__parent));
   TEST_ASSERT_EQUAL_PTR(&n, mock_get_parent(z.__parent));
}



/* ------------------------------------------------------------- */
/* ----------------------- query tests  ------------------------ */
/* ------------------------------------------------------------- */

static void test_bst_first_last_postorder_first(void)
{
   /*
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
   struct mock_node n1, n2, n3, n4, n5, n6, n7, n8, n9;

   mock_setup(&n7, &n5,  &n8,  NULL);
   mock_setup(&n5, &n1,  &n6,  &n7);
   mock_setup(&n8, NULL, &n9,  &n7);
   mock_setup(&n1, NULL, &n3,  &n5);
   mock_setup(&n6, NULL, NULL, &n5);
   mock_setup(&n9, NULL, NULL, &n8);
   mock_setup(&n3, &n2,  &n4,  &n1);
   mock_setup(&n2, NULL, NULL, &n3);
   mock_setup(&n4, NULL, NULL, &n3);

   TEST_ASSERT_EQUAL_PTR(&n1, huz_bst_first(&n7, __parent));
   TEST_ASSERT_EQUAL_PTR(&n9, huz_bst_last(&n7, __parent));
   TEST_ASSERT_EQUAL_PTR(&n2, huz_bst_postorder_first(&n7, __parent));

   /* subtree bounded */
   TEST_ASSERT_EQUAL_PTR(&n2, huz_bst_first(&n3, __parent));
   TEST_ASSERT_EQUAL_PTR(&n6, huz_bst_last(&n5, __parent));
}



/* ------------------------------------------------------------- */
/* --------------------- traversal tests  ---------------------- */
/* ------------------------------------------------------------- */

static void test_bst_next_prev(void)
{
   struct mock_node n[MOCK_TREE17_SIZE];
   mock_setup_tree17(n);

   // next
   for (size_t i = 0; i < MOCK_TREE17_SIZE - 1; i++)
      TEST_ASSERT_EQUAL_PTR(n + i + 1, huz_bst_next(MOCK_NULL, n + i, mock_get_parent, __parent));

   TEST_ASSERT_NULL(huz_bst_next(MOCK_NULL, n + MOCK_TREE17_SIZE - 1, mock_get_parent, __parent));

   // prev
   for (size_t i = MOCK_TREE17_SIZE - 1; i > 0; i--)
      TEST_ASSERT_EQUAL_PTR(n + i - 1, huz_bst_prev(MOCK_NULL, n + i, mock_get_parent, __parent));

   TEST_ASSERT_NULL(huz_bst_prev(MOCK_NULL, n + 0, mock_get_parent, __parent));

   // subtree bounded: subtree rooted at 4 spans [0, 7]
   struct mock_node *sub_parent = mock_get_parent(n[4].__parent);
   TEST_ASSERT_NULL(huz_bst_next(sub_parent, n + 7, mock_get_parent, __parent));
   TEST_ASSERT_NULL(huz_bst_prev(sub_parent, n + 0, mock_get_parent, __parent));
}

static void test_bst_preorder_next(void)
{
   struct mock_node n[MOCK_TREE17_SIZE];
   mock_setup_tree17(n);

   /* 10,8,4,0,2,1,3,5,7,6,9,13,11,12,14,16,15 */
   const struct mock_node *preorder[MOCK_TREE17_SIZE] = {
      n + 10, n + 8,  n + 4, n + 0, n + 2,  n + 1,  n + 3,
      n + 5,  n + 7,  n + 6, n + 9, n + 13, n + 11, n + 12,
      n + 14, n + 16, n + 15
   };

   for (size_t i = 0; i < MOCK_TREE17_SIZE - 1; i++)
      TEST_ASSERT_EQUAL_PTR(preorder[i + 1], huz_bst_preorder_next(MOCK_NULL, preorder[i], mock_get_parent, __parent));

   TEST_ASSERT_NULL(huz_bst_preorder_next(MOCK_NULL, preorder[MOCK_TREE17_SIZE - 1], mock_get_parent, __parent));
}

static void test_bst_postorder_next(void)
{
   struct mock_node n[MOCK_TREE17_SIZE];
   mock_setup_tree17(n);

   /* 1,3,2,0,6,7,5,4,9,8,12,11,15,16,14,13,10 */
   const struct mock_node *postorder[MOCK_TREE17_SIZE] = {
      n + 1, n + 3, n + 2,  n + 0,  n + 6,  n + 7,  n + 5,  n + 4,
      n + 9, n + 8, n + 12, n + 11, n + 15, n + 16, n + 14, n + 13, n + 10
   };

   TEST_ASSERT_EQUAL_PTR(postorder[0], huz_bst_postorder_first(n + 10, __parent));

   for (size_t i = 0; i < MOCK_TREE17_SIZE - 1; i++)
      TEST_ASSERT_EQUAL_PTR(postorder[i + 1], huz_bst_postorder_next(MOCK_NULL, postorder[i], mock_get_parent, __parent));

   TEST_ASSERT_NULL(huz_bst_postorder_next(MOCK_NULL, postorder[MOCK_TREE17_SIZE - 1], mock_get_parent, __parent));
}



/* ------------------------------------------------------------- */
/* ---------------------- foreach tests  ----------------------- */
/* ------------------------------------------------------------- */

struct mock_entry
{
   int padding[2];        /* forces offsetof(bst) != 0 */
   struct mock_node bst;
   int key;
};

static struct mock_node *mock_first(const struct mock_node *subroot)
{
   return (struct mock_node *)huz_bst_first(subroot, __parent);
}

static struct mock_node *mock_next(const struct mock_node *subroot, const struct mock_node *node)
{
   const struct mock_node *subroot_parent = mock_get_parent(subroot->__parent);
   return (struct mock_node *)huz_bst_next(subroot_parent, node, mock_get_parent, __parent);
}

static void test_bst_foreach(void)
{
   struct mock_node n[MOCK_TREE17_SIZE];
   mock_setup_tree17(n);

   struct mock_node *node, *tmp, *subroot = n + 10;

   #define mock_foreach(node, subroot)                \
      __huz_bst_foreach(node,                         \
         mock_first(subroot),                         \
         mock_next(subroot, node),                    \
         huz_typecheck(struct mock_node, *(node))     \
      )

   size_t i = 0;
   mock_foreach(node, subroot)
   {
      TEST_ASSERT_EQUAL_PTR(n + i, node);
      i++;
   }
   TEST_ASSERT_EQUAL_size_t(MOCK_TREE17_SIZE, i);

   #define mock_foreach_safe(node, tmp, subroot)      \
      __huz_bst_foreach_safe(node, tmp,               \
         mock_first(subroot),                         \
         mock_next(subroot, node),                    \
         huz_typecheck(struct mock_node, *(node)),    \
         huz_typecheck(struct mock_node, *(tmp))      \
      )

   i = 0;
   mock_foreach_safe(node, tmp, subroot)
   {
      if (i < MOCK_TREE17_SIZE - 1)
         TEST_ASSERT_EQUAL_PTR(n + i + 1, tmp);
      else
         TEST_ASSERT_NULL(tmp);

      TEST_ASSERT_EQUAL_PTR(n + i, node);
      node = NULL; // simulate free()
      i++;
   }
   TEST_ASSERT_EQUAL_size_t(MOCK_TREE17_SIZE, i);

   #undef mock_foreach_safe
   #undef mock_foreach
}

static void test_bst_foreach_entry(void)
{
   struct mock_entry entries[MOCK_TREE17_SIZE];
   struct mock_entry *e = entries;

   mock_setup(&e[10].bst, &e[8].bst,  &e[13].bst, NULL);
   mock_setup(&e[8].bst,  &e[4].bst,  &e[9].bst,  &e[10].bst);
   mock_setup(&e[13].bst, &e[11].bst, &e[14].bst, &e[10].bst);
   mock_setup(&e[4].bst,  &e[0].bst,  &e[5].bst,  &e[8].bst);
   mock_setup(&e[9].bst,  NULL,       NULL,       &e[8].bst);
   mock_setup(&e[11].bst, NULL,       &e[12].bst, &e[13].bst);
   mock_setup(&e[14].bst, NULL,       &e[16].bst, &e[13].bst);
   mock_setup(&e[0].bst,  NULL,       &e[2].bst,  &e[4].bst);
   mock_setup(&e[5].bst,  NULL,       &e[7].bst,  &e[4].bst);
   mock_setup(&e[12].bst, NULL,       NULL,       &e[11].bst);
   mock_setup(&e[16].bst, &e[15].bst, NULL,       &e[14].bst);
   mock_setup(&e[2].bst,  &e[1].bst,  &e[3].bst,  &e[0].bst);
   mock_setup(&e[7].bst,  &e[6].bst,  NULL,       &e[5].bst);
   mock_setup(&e[15].bst, NULL,       NULL,       &e[16].bst);
   mock_setup(&e[1].bst,  NULL,       NULL,       &e[2].bst);
   mock_setup(&e[3].bst,  NULL,       NULL,       &e[2].bst);
   mock_setup(&e[6].bst,  NULL,       NULL,       &e[7].bst);

   struct mock_entry *cur, *tmp;
   struct mock_node *subroot = &e[10].bst;

   #define mock_foreach_entry(cur, subroot, type, member)            \
      __huz_bst_foreach_entry(cur, type, member,                     \
         mock_first(subroot),                                        \
         mock_next(subroot, &(cur)->member),                         \
         huz_typecheck(huz_typeof_member(type, member), *(subroot))  \
      )

   size_t i = 0;
   mock_foreach_entry(cur, subroot, struct mock_entry, bst)
   {
      TEST_ASSERT_EQUAL_PTR(e + i, cur);
      i++;
   }
   TEST_ASSERT_EQUAL_size_t(MOCK_TREE17_SIZE, i);

   #define mock_foreach_entry_safe(cur, tmp, subroot, type, member)  \
      __huz_bst_foreach_entry_safe(cur, tmp, type, member,           \
         mock_first(subroot),                                        \
         mock_next(subroot, &(cur)->member),                         \
         huz_typecheck(huz_typeof_member(type, member), *(subroot))  \
      )

   i = 0;
   mock_foreach_entry_safe(cur, tmp, subroot, struct mock_entry, bst)
   {
      /* tmp holds the next *node* address, converted on the next iteration */
      if (i < MOCK_TREE17_SIZE - 1)
         TEST_ASSERT_EQUAL_PTR(&e[i + 1].bst, tmp);
      else
         TEST_ASSERT_NULL(tmp);

      TEST_ASSERT_EQUAL_PTR(e + i, cur);
      cur = NULL; // simulate free()
      i++;
   }
   TEST_ASSERT_EQUAL_size_t(MOCK_TREE17_SIZE, i);

   #undef mock_foreach_entry_safe
   #undef mock_foreach_entry
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

   RUN_TEST(test_bst_foreach);
   RUN_TEST(test_bst_foreach_entry);

   return UnityEnd();
}

#endif /* HUZ_BST_TEST */
