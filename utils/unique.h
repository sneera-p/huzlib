#ifndef _SNEERA_UNIQUE_H
#define _SNEERA_UNIQUE_H

/*
 * __unique__ - Unique variable names using
 * __LINE__ as part of the variable name
 */

#define _CONCAT_HIDDEN(a, b) a##b
#define _CONCAT(a, b) _CONCAT_HIDDEN(a, b)

#define __unique__(name) _CONCAT(name, __LINE__)

#undef _CONCAT
#undef _CONCAT_HIDDEN

#endif /* _SNEERA_UNIQUE_H */
