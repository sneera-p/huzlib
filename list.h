#ifndef HUZLIB_LIST_H
#define HUZLIB_LIST_H


#ifdef HUZLIB_STATIC_STACK_IMPL
   #define HUZLIB_IMPL
#endif

#ifdef HUZLIB_STATIC_STACK_SHARED
   #define HUZLIB_SHARED
#endif


#ifndef HUZLIB_LIST_INCLUDES
#define HUZLIB_LIST_INCLUDES

#include "utils/types.h"
#include "utils/hints.h"
#include "utils/assert.h"
#include "utils/api.h"

#endif /* HUZLIB_LIST_INCLUDES */



#include <stddef.h>
#include <stdbool.h>


struct huz_list_node
{
   struct huz_list_node *next, *prev;
};

#define HUZ_LIST_HEAD_INIT(name) {  \
   .next = &(name),                 \
   .prev = &(name),                 \
}


#ifdef NDEBUG
   #define HUZLIB_LIST_API __huzlib_inline__ __huzlib_pure__
#else
   #define HUZLIB_LIST_API __huzlib_inline__
#endif


#ifndef NDEBUG
extern size_t list_len(const struct list_node *head);                                   // WARN: O(n) complexity, do not use in production
extern void list_dump(const struct list_node *head, void (*dump)(struct list_node *));  // WARN: O(n) complexity, do not use in production
extern bool list_contains(const struct list_node *head, const struct list_node *entry); // WARN: O(n) complexity, do not use in production
#endif /* NDEBUG */


/* --- query operations --- */
extern HUZLIB_LIST_API bool list_is_empty(const struct list_node *head);
extern HUZLIB_LIST_API bool list_is_singular(const struct list_node *head);
extern HUZLIB_LIST_API bool list_is_first(const struct list_node *head, const struct list_node *entry);
extern HUZLIB_LIST_API bool list_is_last(const struct list_node *head, const struct list_node *entry);


/* --- mutate operations --- */
extern HUZLIB_LIST_API void list_init(struct list_node *node);
extern HUZLIB_LIST_API void list_add_after(struct list_node *node, struct list_node *new);
extern HUZLIB_LIST_API void list_add_before(struct list_node *node, struct list_node *new);
extern HUZLIB_LIST_API void list_del(struct list_node *entry);
extern HUZLIB_LIST_API void list_del_init(struct list_node *entry);
extern HUZLIB_LIST_API void list_replace(struct list_node *entry, struct list_node *new);
extern HUZLIB_LIST_API void list_replace_init(struct list_node *entry, struct list_node *new);


/* --- inplace rearrange operations --- */
extern HUZLIB_LIST_API void list_swap(struct list_node *a, struct list_node *b);
extern HUZLIB_LIST_API void list_mov_after(struct list_node *node, struct list_node *dest);
extern HUZLIB_LIST_API void list_mov_before(struct list_node *node, struct list_node *dest);
extern HUZLIB_LIST_API void list_rotate_after(struct list_node *head);
extern HUZLIB_LIST_API void list_rotate_before(struct list_node *head);
extern HUZLIB_LIST_API void list_reverse(struct list_node *head);
extern HUZLIB_LIST_API void list_sort(struct list_node *head, int (*cmp)(struct list_node *, struct list_node *));


/* --- chain operations --- */
extern HUZLIB_LIST_API void list_splice_after(struct list_node *node, struct list_node *src);
extern HUZLIB_LIST_API void list_splice_after_init(struct list_node *node, struct list_node *src);
extern HUZLIB_LIST_API void list_splice_before(struct list_node *node, struct list_node *src);
extern HUZLIB_LIST_API void list_splice_before_init(struct list_node *node, struct list_node *src);
extern HUZLIB_LIST_API void list_cut_after(struct list_node *node, struct list_node *entry, struct list_node *dest);
extern HUZLIB_LIST_API void list_cut_before(struct list_node *node, struct list_node *entry, struct list_node *dest);


/* --- list traversal --- */
#define list_foreach(pos, head) for (                 \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->next;                              \
   (pos) != (head);                                   \
   (pos) = (pos)->next                                \
)

#define list_foreach_rev(pos, head) for (             \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->prev;                              \
   (pos) != (head);                                   \
   (pos) = (pos)->prev                                \
)

#define list_foreach_prefetch(pos, head) for (        \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->next;                              \
   (pos) != (head);                                   \
   (pos) = (pos)->next,                               \
   prefetch_read((pos)->next)                         \
)

#define list_foreach_prefetch2(pos, head) for (       \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->next;                              \
   (pos) != (head);                                   \
   (pos) = (pos)->next,                               \
   ((pos)->next != (head)                             \
      ? prefetch_read((pos)->next->next)              \
      : (void)0                                       \
   )                                                  \
)

#define list_foreach_safe(pos, tmp, head) for (       \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(tmp)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->next,                              \
   (tmp) = (pos)->next;                               \
   (pos) != (head);                                   \
   (pos) = (tmp),                                     \
   (tmp) = (tmp)->next                                \
)

#define list_foreach_safe_rev(pos, tmp, head) for (   \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(tmp)),               \
   typecheck(struct list_node, *(head)),              \
   (pos) = (head)->prev,                              \
   (tmp) = (pos)->prev;                               \
   (pos) != (head);                                   \
   (pos) = (tmp),                                     \
   (tmp) = (tmp)->prev                                \
)

#define list_foreach_from(pos, head) for (            \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head));              \
   (pos) != (head);                                   \
   (pos) = (pos)->next                                \
)

#define list_foreach_rev_from(pos, head) for (        \
   typecheck(struct list_node, *(pos)),               \
   typecheck(struct list_node, *(head));              \
   (pos) != (head);                                   \
   (pos) = (pos)->prev                                \
)


/* --- container macros --- */
#define list_entry(ptr, type, member)           container_of(ptr, type, member)
#define list_first_entry(head, type, member)    container_of((head)->next, type, member)
#define list_last_entry(head, type, member)     container_of((head)->prev, type, member)
#define list_next_entry(entr, type, member)     container_of((entr)->member.next, type, member)
#define list_prev_entry(entr, type, member)     container_of((entr)->member.prev, type, member)

#define list_foreach_entry(entr, head, type, member) for (                 \
   typecheck(type, *(entr)),                                               \
   typecheck(struct list_node, *(head)),                                   \
   typecheck_member(struct list_node, type, member),                       \
   (entr) = list_first_entry(head, type, member);                          \
   &(entr)->member != (head);                                              \
   (entr) = list_next_entry(entr, type, member)                            \
)

#define list_foreach_entry_rev(entr, head, type, member) for (             \
   typecheck(type, *(entr)),                                               \
   typecheck(struct list_node, *(head)),                                   \
   typecheck_member(struct list_node, type, member),                       \
   (entr) = list_last_entry(head, type, member);                           \
   &(entr)->member != (head);                                              \
   (entr) = list_prev_entry(entr, type, member)                            \
)

#define list_foreach_entry_safe(entr, temp, head, type, member) for (      \
   typecheck(type, *(entr)),                                               \
   typecheck(type, *(temp)),                                               \
   typecheck(struct list_node, *(head)),                                   \
   typecheck_member(struct list_node, type, member),                       \
   (entr) = list_first_entry(head, type, member),                          \
   (temp) = list_next_entry(entr, type, member);                           \
   &(entr)->member != (head);                                              \
   (entr) = (temp),                                                        \
   (temp) = list_next_entry(temp, type, member)                            \
)

#define list_foreach_entry_safe_rev(entr, temp, head, type, member) for (  \
   typecheck(type, *(entr)),                                               \
   typecheck(type, *(temp)),                                               \
   typecheck(struct list_node, *(head)),                                   \
   typecheck_member(struct list_node, type, member),                       \
   (entr) = list_last_entry(head, type, member),                           \
   (temp) = list_prev_entry(entr, type, member);                           \
   &(entr)->member != (head);                                              \
   (entr) = (temp),                                                        \
   (temp) = list_prev_entry(temp, type, member)                            \
)

#define list_foreach_entry_from(entr, head, type, member) for (            \
   typecheck(type, *(entr)),                                               \
   typecheck(struct list_node, *(head));                                   \
   typecheck_member(struct list_node, type, member),                       \
   &(entr)->member != (head);                                              \
   (entr) = list_next_entry(entr, type, member)                            \
)

#define list_foreach_entry_rev_from(entr, head, type, member) for (        \
   typecheck(type, *(entr)),                                               \
   typecheck(struct list_node, *(head));                                   \
   typecheck_member(struct list_node, type, member),                       \
   &(entr)->member != (head);                                              \
   (entr) = list_prev_entry(entr, type, member)                            \
)



#ifdef HUZLIB_LIST_IMPL

#ifndef NDEBUG

size_t list_len(const struct list_node *head)
{
   __huzlib_assert(head);
   struct list_node *restrict cur;
   size_t len = 0;
   list_foreach(cur, head)
      len++;
   return len;
}

void list_dump(const struct list_node *head, void (*dump)(struct list_node *))
{
   __huzlib_assert(head && dump);
   struct list_node *restrict cur;
   list_foreach(cur, head)
      dump(cur);
}

bool list_contains(const struct list_node *head, const struct list_node *entry)
{
   __huzlib_assert(head && entry);
   struct list_node *restrict cur;
   list_foreach(cur, head)
      if (cur == entry)
          return true;
   return false;
}

#endif /* NDEBUG */


/* ------------------------------------------------ */
/* --------------- query operations --------------- */
/* ------------------------------------------------ */

HUZLIB_LIST_API bool list_is_empty(const struct list_node *head)
{
   __huzlib_assert(head);
   return head->next == head;
}

HUZLIB_LIST_API bool list_is_singular(const struct list_node *head)
{
   __huzlib_assert(head);
   return !list_is_empty(head) && head->next->next == head;
}

HUZLIB_LIST_API bool list_is_first(const struct list_node *head, const struct list_node *entry)
{
   __huzlib_assert(head && entry);
   return head->next == entry;
}

HUZLIB_LIST_API bool list_is_last(const struct list_node *head, const struct list_node *entry)
{
   __huzlib_assert(head && entry);
   return head->prev == entry;
}


/* ------------------------------------------------- */
/* --------------- helper functions  --------------- */
/* ------------------------------------------------- */

/*
 * before:  prev <-> next, prev <-> old <-> next
 * after:   prev <-> new <-> next
 */ 
static HUZLIB_LIST_API void __list_add(struct list_node *restrict new, struct list_node *prev, struct list_node *next)
{
   __huzlib_assert(new && prev && next);
   __huzlib_assert((prev->next == next && next->prev == prev) || (prev->next->next == next && next->prev->prev == prev));

   prev->next = new;
   next->prev = new;
   new->prev = prev;
   new->next = next;
}

/*
 * before:  prev <-> next, prev <-> old(...) <-> next
 * after:   prev <-> new_head <-> ... <-> new_tail <-> next
 */ 
static HUZLIB_LIST_API void __list_add_batch(struct list_node *new_head, struct list_node *new_tail, struct list_node *prev, struct list_node *next)
{
   __huzlib_assert(new_head && new_tail && prev && next);

   prev->next = new_head;
   next->prev = new_tail;
   new_head->prev = prev;
   new_tail->next = next;
}

/*
 * before:  prev <-> del <-> next
 * after:   prev <-> next
 */ 
static HUZLIB_LIST_API void __list_rm(struct list_node *prev, struct list_node *next)
{
   __huzlib_assert(prev && next);
   prev->next = next;
   next->prev = prev;
}

/*
 * before:  aprev <-> a <-> anext,  bprev <-> b <-> bnext
 * after:   aprev <-> b <-> anext,  bprev <-> a <-> bnext
 */ 
static HUZLIB_LIST_API void __list_swap(struct list_node *aprev, struct list_node *restrict a, struct list_node *anext, struct list_node *bprev, struct list_node *restrict b, struct list_node *bnext)
{
   __huzlib_assert(aprev && a && anext && bprev && b && bnext);
   __huzlib_assert(aprev == a->prev && aprev->next == a);
   __huzlib_assert(anext == a->next && anext->prev == a);
   __huzlib_assert(bprev == b->prev && bprev->next == b);
   __huzlib_assert(bnext == b->next && bnext->prev == b);

   aprev->next = b;
   anext->prev = b;

   bprev->next = a;
   bnext->prev = a;

   a->prev = bprev;
   a->next = bnext;

   b->prev = aprev;
   b->next = anext;
}

/*
 * before:  pre <-> prev <-> next <-> suc
 * after:   pre <-> next <-> prev <-> suc
 */ 
static HUZLIB_LIST_API void __list_swap_adj(struct list_node *pre, struct list_node *restrict prev, struct list_node *restrict next, struct list_node *suc)
{
   __huzlib_assert(prev && next);
   __huzlib_assert(pre == prev->prev && pre->next == prev);
   __huzlib_assert(suc == next->next && suc->prev == next);

   pre->next = next;
   suc->prev = prev;

   prev->next = suc;
   next->prev = pre;

   prev->prev = next;
   next->next = prev;
}


/* ------------------------------------------------- */
/* --------------- mutate operations --------------- */
/* ------------------------------------------------- */

HUZLIB_LIST_API void list_add_after(struct list_node *restrict node, struct list_node *restrict new)
{
   __huzlib_assert(node && new);
   __list_add(new, node, node->next);
}

HUZLIB_LIST_API void list_add_before(struct list_node *restrict node, struct list_node *restrict new)
{
   __huzlib_assert(node && new);
   __list_add(new, node->prev, node);
}

HUZLIB_LIST_API void list_del(struct list_node *restrict entry)
{
   __huzlib_assert(entry);
   __list_rm(entry->prev, entry->next);
}

HUZLIB_LIST_API void list_del_init(struct list_node *restrict entry)
{
   list_del(entry);
   list_init(entry);
}

HUZLIB_LIST_API void list_replace(struct list_node *restrict entry, struct list_node *restrict new)
{
   __huzlib_assert(entry && new);
   __list_add(new, entry->prev, entry->next);
}

HUZLIB_LIST_API void list_replace_init(struct list_node *restrict entry, struct list_node *restrict new)
{
   list_replace(entry, new);
   list_init(entry);
}


/* ------------------------------------------------ */
/* --------- inplace rearrange operations --------- */
/* ------------------------------------------------ */

HUZLIB_LIST_API void list_swap(struct list_node *restrict a, struct list_node *restrict b)
{
   __huzlib_assert(a && b);
   if (a->next == b)
      __list_swap_adj(a->prev, a, b, b->next);
   else if (a->prev == b)
      __list_swap_adj(b->prev, b, a, a->next);
   else
      __list_swap(a->prev, a, a->next, b->prev, b, b->next);
}

HUZLIB_LIST_API void list_mov_after(struct list_node *restrict node, struct list_node *restrict dest)
{
   __huzlib_assert(node && dest);
   list_del(node);
   list_add_after(dest, node);
}

HUZLIB_LIST_API void list_mov_before(struct list_node *restrict node, struct list_node *restrict dest)
{
   __huzlib_assert(node && dest);
   list_del(node);
   list_add_before(dest, node);
}

HUZLIB_LIST_API void list_rotate_after(struct list_node *restrict head)
{
   __huzlib_assert(head);
   if (!(list_is_empty(head) || list_is_singular(head)))
      __list_swap_adj(head->prev, head, head->next, head->next->next);
}

HUZLIB_LIST_API void list_rotate_before(struct list_node *restrict head)
{
   __huzlib_assert(head);
   if (!(list_is_empty(head) || list_is_singular(head)))
      __list_swap_adj(head->prev->prev, head->prev, head, head->next);
}

HUZLIB_LIST_API void list_reverse(struct list_node *restrict head)
{
   __huzlib_assert(head);
   struct list_node *cur, *tmp;
   list_foreach_safe(cur, tmp, head)
      SWAP(cur->prev, cur->next);
   SWAP(head->prev, head->next);
}

/*
 * list_sort - mergesort implementation
 *
 * Strategy:
 *    Bottom-up iterative merge
 *    merge sub-lists of size 1, then 2, then 4, etc.
 *
 * --- this function was generated by Gemini 3 Fast ---
 */
HUZLIB_LIST_API void list_sort(struct list_node *head, int (*cmp)(struct list_node *, struct list_node *))
{
   __huzlib_assert(head && cmp);

   if (list_is_empty(head) || list_is_singular(head))
      return;

   struct list_node *list = head->next;
   head->prev->next = NULL; // Temporarily turn circular list into null-terminated

   struct list_node *p, *q, *e, *tail;
   size_t insize = 1, nmerges, psize, qsize, i;

   while (1)
   {
      p = list;
      list = NULL;
      tail = NULL;
      nmerges = 0;

      while (p)
      {
         nmerges++;
         q = p;
         psize = 0;
         for (i = 0; i < insize; i++)
         {
            psize++;
            q = q->next;
            if (!q) break;
         }

         qsize = insize;
         while (psize > 0 || (qsize > 0 && q))
         {
            if (psize == 0)
            {
               e = q;
               q = q->next;
               qsize--;
            }
            else if (qsize == 0 || !q)
            {
               e = p;
               p = p->next;
               psize--;
            }
            else if (cmp(p, q) <= 0)
            {
               e = p;
               p = p->next;
               psize--;
            }
            else
            {
               e = q;
               q = q->next;
               qsize--;
            }

            if (tail)
               tail->next = e;
            else
               list = e;
 
            e->prev = tail; // Maintain the prev pointer
            tail = e;
         }
         p = q;
      }

      tail->next = NULL;
      if (nmerges <= 1)
      {
         // Sorting complete, close the circle
         head->next = list;
         list->prev = head;
         tail->next = head;
         head->prev = tail;
         return;
      }
      insize *= 2;
   }
}


/* ------------------------------------------------ */
/* --------------- chain operations --------------- */
/* ------------------------------------------------ */

/*
 * target:  node <-> next_node
 * source:  [src] <-> first <-> ... <-> last <-> [src]
 *
 * after:   node <-> first <-> ... <-> last <-> next_node
 */
HUZLIB_LIST_API void list_splice_after(struct list_node *restrict node, struct list_node *restrict src)
{
   __huzlib_assert(node && src);
   __list_add_batch(src->next, src->prev, node, node->next);
}

HUZLIB_LIST_API void list_splice_after_init(struct list_node *restrict node, struct list_node *restrict src)
{
   __huzlib_assert(node && src);
   list_splice_after(node, src);
   list_init(src);
}

/*
 * target:  prev_node <-> node
 * source:  [src] <-> first <-> ... <-> last <-> [src]
 *
 * after:   prev_node <-> first <-> ... <-> last <-> node
 */
HUZLIB_LIST_API void list_splice_before(struct list_node *restrict node, struct list_node *restrict src)
{
   __huzlib_assert(node && src);
   __list_add_batch(src->next, src->prev, node->prev, node);
}

HUZLIB_LIST_API void list_splice_before_init(struct list_node *restrict node, struct list_node *restrict src)
{
   __huzlib_assert(node && src);
   list_splice_before(node, src);
   list_init(src);
}

/*
 * node:  [node] <-> 1 <-> ... <-> [entry] <-> [split_node] <-> ... <-> [node]
 * dest:  [dest] <-> [dest] (must be empty)
 *
 * step 1: [node] <--------------------------> [split_node] (bypass segment)
 * step 2: [dest] <-> 1 <-> ... <-> [entry] <-> [dest]      (re-anchor segment)
 *
 * after:  node contains [node] <-> [split_node] ...
 * dest contains [dest] <-> 1 ... <-> [entry]
 */
HUZLIB_LIST_API void list_cut_after(struct list_node *restrict node, struct list_node *restrict entry, struct list_node *restrict dest)
{
   __huzlib_assert(node && entry && list_is_empty(dest));
   struct list_node *restrict _tmp = node->next;
   __list_rm(node, entry->next);
   __list_add_batch(_tmp, entry, dest, dest);
}

/*
 * node:  [node] <-> ... <-> [prev_node] <-> [entry] <-> ... <-> [last] <-> [node]
 * dest:  [dest] <-> [dest] (must be empty)
 *
 * step 1: [node] <-> ... <-> [prev_node] <---------------------------> [node]
 * step 2: [dest] <---------> [entry] <-> ... <-> [last] <------------> [dest]
 *
 * after:  node contains [node] <-> ... <-> [prev_node]
 * dest contains [dest] <-> [entry] ... <-> [last]
 */
HUZLIB_LIST_API void list_cut_before(struct list_node *restrict node, struct list_node *restrict entry, struct list_node *restrict dest)
{
   __huzlib_assert(node && entry && list_is_empty(dest));
   struct list_node *restrict _tmp = node->prev;
   __list_rm(entry->prev, node);
   __list_add_batch(entry, _tmp, dest, dest);
}

#endif /* HUZLIB_LIST_IMPL */




#ifdef HUZLIB_LIST_TEST
#endif /* HUZLIB_LIST_TEST */




#endif /* HUZLIB_LIST_H */
