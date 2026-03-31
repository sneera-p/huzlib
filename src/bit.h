#ifndef _SNEERA_BIT_H
#define _SNEERA_BIT_H

#include <assert.h>
#include <stdint.h>
#include "types.h"

#define _BIT_TYPES(x, ...)    \
   x(uint8_t, __VA_ARGS__)    \
   x(uint16_t, __VA_ARGS__)   \
   x(uint32_t, __VA_ARGS__)   \
   x(uint64_t, __VA_ARGS__)
   /* add more types here if required */

#define _BIT_C23_PROTOS(type, x)       \
   x(type, trailing_zeros, type w)     \
   x(type, leading_zeros, type w)      \
   x(type, count_ones, type w)         \
   x(type, first_trailing_one, type w) \
   x(type, first_leading_one, type w)  \
   x(type, width, type w)              \
   x(type, ceil, type w)               \
   x(type, floor, type w)

#define _BIT_C2Y_PROTOS(type, x)             \
   x(type, rotate_left, type w, uint8_t r)   \
   x(type, rotate_right, type w, uint8_t r)

#define _BIT_LIB_PROTOS(type, x)                            \
   x(type, rotate_left_part, type w, uint8_t r, uint8_t m)  \
   x(type, rotate_right_part, type w, uint8_t r, uint8_t m)


#define _ASSERT_UNSIGNED(type, ...)          _Static_assert((type)(-1) > 0, #type " is not unsigned");
#define _DECLARE_PROTOTYPE(type, name, ...)  extern type bit_##name##_##type(__VA_ARGS__);
#define _GENERIC_ENTRY(type, func, ...)      type: func##_##type,
#define _GENERIC_ENTRY_FALLBACK              default: bit_type_not_supported


_BIT_TYPES(_ASSERT_UNSIGNED)

#if (__STDC_VERSION__ >= 202311L)
   #include <stdbit.h>

   #define bit_trailing_zeros(w)       stdc_trailing_zeros(w)
   #define bit_leading_zeros(w)        stdc_leading_zeros(w)
   #define bit_count_ones(w)           stdc_count_ones(w)
   #define bit_first_trailing_one(w)   stdc_first_trailing_one(w)
   #define bit_first_leading_one(w)    stdc_first_leading_one(w)

   #define bit_trailing_ones(w)        stdc_trailing_ones(w)
   #define bit_leading_ones(w)         stdc_leading_ones(w)
   #define bit_count_zeros(w)          stdc_count_zeros(w)
   #define bit_first_trailing_zero(w)  stdc_first_trailing_zero(w)
   #define bit_first_leading_zero(w)   stdc_first_leading_zero(w)

   #define bit_width(w)                stdc_bit_width(w)
   #define bit_ceil(w)                 stdc_bit_ceil(w)
   #define bit_floor(w)                stdc_bit_floor(w)

#else
   _BIT_TYPES(_BIT_C23_PROTOS, _DECLARE_PROTOTYPE)

   #define bit_trailing_zeros(w)       _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_trailing_zeros) _GENERIC_ENTRY_FALLBACK)(w)
   #define bit_leading_zeros(w)        _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_leading_zeros) _GENERIC_ENTRY_FALLBACK)(w)
   #define bit_count_ones(w)           _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_count_ones) _GENERIC_ENTRY_FALLBACK)(w)
   #define bit_first_trailing_one(w)   _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_first_trailing_one) _GENERIC_ENTRY_FALLBACK)(w)
   #define bit_first_leading_one(w)    _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_first_leading_one) _GENERIC_ENTRY_FALLBACK)(w)

   #define bit_trailing_ones(w)        bit_trailing_zeros((typeof(w))~(w))
   #define bit_leading_ones(w)         bit_leading_zeros((typeof(w))~(w))
   #define bit_count_zeros(w)          bit_count_ones((typeof(w))~(w))
   #define bit_first_trailing_zero(w)  bit_first_trailing_one((typeof(w))~(w))
   #define bit_first_leading_zero(w)   bit_first_leading_one((typeof(w))~(w))

   #define bit_width(w)                _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_width) _GENERIC_ENTRY_FALLBACK)(w)
   #define bit_ceil(w)                 _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_ceil)  _GENERIC_ENTRY_FALLBACK)(w)
   #define bit_floor(w)                _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_floor) _GENERIC_ENTRY_FALLBACK)(w)

#endif /* C23 PROTOS */


#if (__STDC_VERSION__ > 202311L) /* C2Y */
   #include <stdbit.h>

   #define bit_rotate_left(w, r)       stdc_rotate_left(w, r)
   #define bit_rotate_right(w, r)      stdc_rotate_right(w, r)

#else
   _BIT_TYPES(_BIT_C2Y_PROTOS, _DECLARE_PROTOTYPE)

   #define bit_rotate_left(w, r)       _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_rotate_left) _GENERIC_ENTRY_FALLBACK)(w)
   #define bit_rotate_right(w, r)      _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_rotate_left) _GENERIC_ENTRY_FALLBACK)(w)

#endif /* C2Y PROTOS */


_BIT_TYPES(_BIT_LIB_PROTOS, _DECLARE_PROTOTYPE)

#define bit_rotate_left_part(w, r, m)  _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_rotate_left_part) _GENERIC_ENTRY_FALLBACK)(w)
#define bit_rotate_right_part(w, r, m) _Generic((w), _BIT_TYPES(_GENERIC_ENTRY, bit_rotate_left_part) _GENERIC_ENTRY_FALLBACK)(w)



#ifdef _SNEERA_BIT_IMPL

#include <limits.h>

#define _UWIDTH(expr) (sizeof(expr) * CHAR_BIT)
#define _UMAX(type)   ((type)(-1))


#if (__STDC_VERSION__ < 202311L)

#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER) || defined(__TINYC__) || defined(__xlC__) || defined(__ibmxl__)
   /* ------------------------- */
   /* ------ GCC backend ------ */
   /* ------------------------- */

   #define _GENERATE_TRAILING_ZEROS(type, ...)              \
   inline type bit_trailing_zeros_##type(type w)            \
   {                                                        \
      if (w == 0)                                           \
         return _UWIDTH(type);                              \
                                                            \
      if (sizeof(type) <= sizeof(unsigned int))             \
         return __builtin_ctz((unsigned int)w);             \
                                                            \
      else if (sizeof(type) <= sizeof(unsigned long))       \
         return __builtin_ctzl((unsigned long)w);           \
                                                            \
      else                                                  \
         return __builtin_ctzll((unsigned long long)w);     \
   }

   #define _GENERATE_LEADING_ZEROS(type, ...)               \
   inline type bit_leading_zeros_##type(type w)             \
   {                                                        \
      if (w == 0)                                           \
         return _UWIDTH(type);                              \
                                                            \
      unsigned int clz;                                     \
      unsigned int up_bits;                                 \
      if (sizeof(type) <= sizeof(unsigned int)) {           \
         up_bits = sizeof(unsigned int) * CHAR_BIT;         \
         clz = __builtin_clz((unsigned int)w);              \
      }                                                     \
      else if (sizeof(type) <= sizeof(unsigned long)) {     \
         up_bits = sizeof(unsigned long) * CHAR_BIT;        \
         clz = __builtin_clzl((unsigned long)w);            \
      }                                                     \
      else {                                                \
         up_bits = sizeof(unsigned long long) * CHAR_BIT;   \
         clz = __builtin_clzll((unsigned long long)w);      \
      }                                                     \
      return (type)(clz - (up_bits - _UWIDTH(type)));       \
   }

   #define _GENERATE_COUNT_ONES(type, ...)                  \
   inline type bit_count_ones_##type(type w)                \
   {                                                        \
      if (sizeof(type) <= sizeof(unsigned int))             \
         return __builtin_popcount(w);                      \
                                                            \
      else if (sizeof(type) <= sizeof(unsigned long))       \
         return __builtin_popcountl(w);                     \
                                                            \
      else                                                  \
         return __builtin_popcountll(w);                    \
   }


#elif defined(_M_ARM) || defined(_M_ARM64)
   /* -------------------------- */
   /* ---- MVSC ARM backend ---- */
   /* -------------------------- */
   #include <intrin.h>

   #define _GENERATE_TRAILING_ZEROS(type, ...)              \
   inline type bit_trailing_zeros_##type(type w)            \
   {                                                        \
      if (w == 0)                                           \
         return _UWIDTH(type);                              \
                                                            \
      if (sizeof(type) <= sizeof(unsigned int))             \
         return _CountTrailingZeros((unsigned int)w);       \
                                                            \
      else                                                  \
         return _CountTrailingZeros64((unsigned __int64)w); \
   }

   #define _GENERATE_LEADING_ZEROS(type, ...)               \
   inline type bit_leading_zeros_##type(type w)             \
   {                                                        \
      if (w == 0)                                           \
         return _UWIDTH(type);                              \
                                                            \
      if (sizeof(type) <= sizeof(unsigned int))             \
         return _CountLeadingZeros((unsigned int)w);        \
                                                            \
      else                                                  \
         return _CountLeadingZeros64((unsigned __int64)w);  \
   }

   #define _GENERATE_COUNT_ONES(type, ...)                  \
   inline type bit_count_ones_##type(type w)                \
   {                                                        \
      if (sizeof(type) <= sizeof(unsigned int))             \
         return (type)__popcnt((unsigned int)w);            \
                                                            \
      else                                                  \
         return (type)__popcnt64((unsigned __int64)w);      \
   }


#elif defined(_MSC_VER) || defined(__POCC__)
   /* -------------------------- */
   /* ---- MVSC x86 backend ---- */
   /* -------------------------- */
   #include <intrin.h>

   #define _GENERATE_TRAILING_ZEROS(type, ...)              \
   inline type bit_trailing_zeros_##type(type w)            \
   {                                                        \
      if (w == 0)                                           \
         return _UWIDTH(type);                              \
                                                            \
      unsigned long index;                                  \
                                                            \
      if (sizeof(type) <= sizeof(unsigned long))            \
         _BitScanForward(&index, (unsigned long)w);         \
                                                            \
      else                                                  \
         _BitScanForward64(&index, (unsigned __int64)w);    \
                                                            \
      return (type)index;                                   \
   }

   #define _GENERATE_LEADING_ZEROS(type, ...)               \
   inline type bit_leading_zeros_##type(type w)             \
   {                                                        \
      if (w == 0)                                           \
         return _UWIDTH(type);                              \
                                                            \
      unsigned long index;                                  \
                                                            \
      if (sizeof(type) <= sizeof(unsigned long))            \
         _BitScanReverse(&index, (unsigned long)w);         \
                                                            \
      else                                                  \
         _BitScanReverse64(&index, (unsigned __int64)w);    \
                                                            \
      return (type)(_UWIDTH(type) - 1 - index);             \
   }

   #define _GENERATE_COUNT_ONES(type, ...)                  \
   inline type bit_count_ones_##type(type w)                \
   {                                                        \
      if (sizeof(type) <= sizeof(unsigned int))             \
         return (type)__popcnt((unsigned int)w);            \
                                                            \
      else                                                  \
         return (type)__popcnt64((unsigned __int64)w);      \
   }


#else
   /* ------------------------- */
   /* ---- default backend ---- */
   /* ------------------------- */

   #define _GENERATE_TRAILING_ZEROS(type, ...)              \
   inline type bit_trailing_zeros_##type(type w)            \
   {                                                        \
      if (w == 0)                                           \
         return _UWIDTH(type);                              \
                                                            \
      type count = 0;                                       \
      while ((w & 1) == 0) {                                \
         w >>= 1;                                           \
         count++;                                           \
      }                                                     \
      return count;                                         \
   }

   #define _GENERATE_LEADING_ZEROS(type, ...)               \
   inline type bit_leading_zeros_##type(type w)             \
   {                                                        \
      if (w == 0)                                           \
         return _UWIDTH(type);                              \
                                                            \
      type count = 0;                                       \
      type mask = (type)(1) << (_UWIDTH(type) - 1);         \
      while ((w & mask) == 0) {                             \
         mask >>= 1;                                        \
         count++;                                           \
      }                                                     \
      return count;                                         \
   }

   #define _GENERATE_COUNT_ONES(type, ...)                  \
   inline type bit_count_ones_##type(type w)                \
   {                                                        \
      type count = 0;                                       \
      while (w) {                                           \
         w &= w - 1; /* clear lowest set bit */             \
         count++;                                           \
      }                                                     \
      return count;                                         \
   }

#endif


#define _GENERATE_FIRST_TRAILING_ONE(type, ...)
inline type bit_first_trailing_one_##type(type w)           \
{                                                           \
   return (w == 0)                                          \
      ? 0                                                   \
      : bit_trailing_zeros_##type(w) + 1;                   \
}

#define _GENERATE_FIRST_LEADING_ONE(type, ...)
inline type bit_first_leading_one_##type(type w)            \
{                                                           \
   return (w == 0)                                          \
      ? 0                                                   \
      : _UWIDTH(w) - (bit_leading_zeros_##type(w) + 1);     \
}

#define _GENERATE_WIDTH(type, ...)
inline type bit_width_##type(type w)                        \
{                                                           \
   return (w == 0)                                          \
      ? 0                                                   \
      : _UWIDTH(w) - bit_leading_zeros_##type(w);           \
}

#define _GENERATE_CEIL(type, ...)
inline type bit_ceil_##type(type w)                               \
{                                                                 \
   return (w == 0)                                                \
      ? 1                                                         \
      :(1ull << (_UWIDTH(w) - bit_leading_zeros_##type(w - 1)));  \
}

#define _GENERATE_FLOOR(type, ...)
inline type bit_ceil_##type(type w)                               \
{                                                                 \
   return (w == 0)                                                \
      ? 0                                                         \
      : (1ull << (_UWIDTH(w) - 1 - bit_leading_zeros_##type(w))); \
}

_BIT_TYPES(_GENERATE_TRAILING_ZEROS)
_BIT_TYPES(_GENERATE_LEADING_ZEROS)
_BIT_TYPES(_GENERATE_COUNT_ONES)
_BIT_TYPES(_GENERATE_FIRST_TRAILING_ONE)
_BIT_TYPES(_GENERATE_FIRST_LEADING_ONE)
_BIT_TYPES(_GENERATE_WIDTH)
_BIT_TYPES(_GENERATE_CEIL)
_BIT_TYPES(_GENERATE_FLOOR)

#undef _GENERATE_BIT_FLOOR
#undef _GENERATE_BIT_CEIL
#undef _GENERATE_BIT_WIDTH
#undef _GENERATE_FIRST_LEADING_ONE
#undef _GENERATE_FIRST_TRAILING_ONE
#undef _GENERATE_COUNT_ONES
#undef _GENERATE_LEADING_ZEROS
#undef _GENERATE_TRAILING_ZEROS

#endif /* C17 PROTOS */


#if (__STDC_VERSION__ <= 202311L)

#if defined(_M_ARM) || defined(_M_ARM64)
   /* -------------------------- */
   /* ---- MVSC ARM backend ---- */
   /* -------------------------- */
   #include <intrin.h>

   #define _GENERATE_ROTATE_LEFT(type, ...)                       \
   inline type bit_rotate_left_##type(type w, uint8_t r)          \
   {                                                              \
      assert(r < _UWIDTH(type));                                  \
                                                                  \
      if (sizeof(type) <= sizeof(uint16_t))                       \
         return (uint16_t)((w << r) | (w >> (_UWIDTH(w) - r)));   \
                                                                  \
      else if (sizeof(type) <= sizeof(uint32_t))                  \
         return _rotl(w, r);                                      \
                                                                  \
      else                                                        \
         return _rotl64(w, r);                                    \
   }

   #define _GENERATE_ROTATE_RIGHT(type, ...)                      \
   inline type bit_rotate_right_##type(type w, uint8_t r)         \
   {                                                              \
      assert(r < _UWIDTH(type));                                  \
                                                                  \
      if (sizeof(type) <= sizeof(uint16_t))                       \
         return (uint16_t)((w >> r) | (w << (_UWIDTH(w) - r)));   \
                                                                  \
      else if (sizeof(type) <= sizeof(uint32_t))                  \
         return _rotr(w, r);                                      \
                                                                  \
      else                                                        \
         return _rotr64(w, r);                                    \
   }

#elif defined(__INTEL_COMPILER) || defined(_MSC_VER) || defined(__POCC__)
   /* -------------------------- */
   /* ---- MVSC x86 backend ---- */
   /* -------------------------- */
   #include <intrin.h>

   #define _GENERATE_ROTATE_LEFT(type, ...)                       \
   inline type bit_rotate_left_##type(type w, uint8_t r)          \
   {                                                              \
      assert(r < _UWIDTH(type));                                  \
                                                                  \
      if (sizeof(type) <= sizeof(uint8_t))                        \
         return _rotl8(w, r);                                     \
                                                                  \
      else if (sizeof(type) <= sizeof(uint16_t))                  \
         return _rotl16(w, r);                                    \
                                                                  \
      else if (sizeof(type) <= sizeof(uint32_t))                  \
         return _rotl(w, r);                                      \
                                                                  \
      else                                                        \
         return _rotl64(w, r);                                    \
   }

   #define _GENERATE_ROTATE_RIGHT(type, ...)                      \
   inline type bit_rotate_right_##type(type w, uint8_t r)         \
   {                                                              \
      assert(r < _UWIDTH(type));                                  \
                                                                  \
      if (sizeof(type) <= sizeof(uint8_t))                        \
         return _rotr8(w, r);                                     \
                                                                  \
      else if (sizeof(type) <= sizeof(uint16_t))                  \
         return _rotr16(w, r);                                    \
                                                                  \
      else if (sizeof(type) <= sizeof(uint32_t))                  \
         return _rotr(w, r);                                      \
                                                                  \
      else                                                        \
         return _rotr64(w, r);                                    \
   }

#elif defined(__xlC__) || defined(__ibmxl__)
   /* -------------------------- */
   /* ----- IBM PC backend ----- */
   /* -------------------------- */
   #include <builtins.h>

   #define _GENERATE_ROTATE_LEFT(type, ...)                       \
   inline type bit_rotate_left_##type(type w, uint8_t r)          \
   {                                                              \
      assert(r < _UWIDTH(type));                                  \
                                                                  \
      if (sizeof(type) <= sizeof(uint16_t))                       \
         return (uint16_t)((w << r) | (w >> (_UWIDTH(w) - r)));   \
                                                                  \
      else if (sizeof(type) <= sizeof(uint32_t))                  \
         return __rlwinm(w, r, 0, 31);                            \
                                                                  \
      else                                                        \
         return __rlwnm(w, r, 0, 63);                             \
}

   #define _GENERATE_ROTATE_RIGHT(type, ...)                      \
   inline type bit_rotate_right_##type(type w, uint8_t r)         \
   {                                                              \
      assert(r < _UWIDTH(type));                                  \
                                                                  \
      if (sizeof(type) <= sizeof(uint16_t))                       \
         return (uint16_t)((w >> r) | (w << (_UWIDTH(w) - r)));   \
                                                                  \
      else if (sizeof(type) <= sizeof(uint32_t))                  \
         return __rlwinm(w, 32 - r, 0, 31);                       \
                                                                  \
      else                                                        \
         return __rlwnm(w, 64 - r, 0, 63);                        \
   }

#else
   /* ------------------------- */
   /* ---- default backend ---- */
   /* ------------------------- */

   #define _GENERATE_ROTATE_LEFT(type, ...)                       \
   inline type bit_rotate_left_##type(type w, uint8_t r)          \
   {                                                              \
      assert(r < _UWIDTH(type));                                  \
      return (type)((w << r) | (w >> (_UWIDTH(type) - r)));       \
   }

   #define _GENERATE_ROTATE_RIGHT(type, ...)                      \
   inline type bit_rotate_right_##type(type w, uint8_t r)         \
   {                                                              \
      assert(r < _UWIDTH(type));                                  \
      return (type)((w >> r) | (w << (_UWIDTH(type) - r)));       \
   }

_BIT_TYPES(_GENERATE_ROTATE_LEFT)
_BIT_TYPES(_GENERATE_ROTATE_RIGHT)

#undef _GENERATE_ROTATE_RIGHT
#undef _GENERATE_ROTATE_LEFT

#endif


#endif /* C23 PROTOS */


#define _GENERATE_ROTATE_LEFT_PART(type, ...)                           \
inline type bit_rotate_left_part_##type(type w, uint8_t r,, uint8_t m)  \
{                                                                       \
   assert(r < m && m < _UWIDTH(type));                                  \
   if (r == 0 || m == 0)                                                \
      return w;                                                         \
                                                                        \
   type mask = ((type)1 << m) - 1;                                      \
   type part = w & mask;                                                \
   type high = w & ~mask;                                               \
                                                                        \
   type rotated = (type)((part << r) | (part >> (m - r))) & mask;       \
   return high | rotated;                                               \
}

#define _GENERATE_ROTATE_RIGHT_PART(type, ...)                          \
inline type bit_rotate_right_part_##type(type w, uint8_t r, uint8_t m)  \
{                                                                       \
   assert(r < m && m < _UWIDTH(type));                                  \
   if (r == 0 || m == 0)                                                \
      return w;                                                         \
                                                                        \
   type mask = ((type)1 << m) - 1;                                      \
   type part = w & mask;                                                \
   type high = w & ~mask;                                               \
                                                                        \
   type rotated = (type)((part >> r) | (part << (m - r))) & mask;       \
   return high | rotated;                                               \
}

_BIT_TYPES(_GENERATE_ROTATE_LEFT_PART)
_BIT_TYPES(_GENERATE_ROTATE_RIGHT_PART)

#undef _GENERATE_ROTATE_RIGHT_PART
#undef _GENERATE_ROTATE_LEFT_PART


#undef _UMAX
#undef _UWIDTH

#endif /* _SNEERA_BIT_IMPL */


#undef _GENERIC_ENTRY_FALLBACK
#undef _GENERIC_ENTRY
#undef _DECLARE_PROTOTYPE
#undef _ASSERT_UNSIGNED

#undef _BIT_LIB_PROTOS
#undef _BIT_C23_PROTOS
#undef _BIT_C2Y_PROTOS
#undef _BIT_TYPES


#endif /* _SNEERA_BIT_H */
