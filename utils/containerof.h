#ifndef _SNEERA_CONTAINEROF_H
#define _SNEERA_CONTAINEROF_H

#include <stddef.h>
#include "type.h"

/*
 * __container_of - cast a member of a structure out to the containing structure
 *
 * @ptr:	the pointer to the member
 * @type:	the type of the container struct this is embedded in
 * @member:	the name of the member within the struct
 *
 * WARNING:
 * This macro is the internal implementation and should not be used directly.
 * Use containerof() instead which provides type checking when available.
 */
#define __container_of(ptr, type, member) \
   ((type *)((char*)(ptr) - offsetof(type, member)))


/**
 * containerof - cast a member of a structure out to the containing structure
 *
 * @ptr:	the pointer to the member
 * @type:	the type of the container struct this is embedded in
 * @member:	the name of the member within the struct
 *
 * Return: pointer to the containing structure
 *
 * WARNING:
 * No validation is performed that the pointer is actually within a
 * valid structure. Incorrect usage results in undefined behavior.
 */
#if defined(__GNUC__) || defined(__clang__) || defined(__POCC__) || defined(__TINYC__) || defined(__ICC) || defined(__INTEL_COMPILER)
   #define containerof(ptr, type, member) __extension__ ({  \
      __typeof__(((type *)0)->member) *__m = (ptr);         \
      __container_of(__m, type, member);                    \
   })
#else
   #define containerof(ptr, type, member) typecheck_ptr (   \
      typeof(((type *)0)->member), ptr,                     \
      __container_of(ptr, type, member)                     \
   )
#endif


#undef __container_of

#endif /* _SNEERA_CONTAINEROF_H */
