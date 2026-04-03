#ifndef _HUZLIB_XMACRO
#define _HUZLIB_XMACRO

#define XMACRO_ENUM(elem)                 (elem),
#define XMACRO_ENUM_VALUE(elem, val)      (elem) = (val),
#define XMACRO_DESC(elem)                 #elem,

#define XMACRO_GENERIC_ENTRY(type, elem)  type: elem##type,
#define XMACRO_GENERIC_DEFAULT(elem)      default: elem

#endif /* _HUZLIB_XMACRO */
