#ifndef HUZLIB_BIT_H
#define HUZLIB_BIT_H

#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include "types.h"
#include "xmacro.h"

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
   x(type, rotate_right_part, type w, uint8_t r, uint8_t m) \


#define _UWIDTH(expr)                        (sizeof(expr) * CHAR_BIT)
#define _UMAX(type)                          ((type)(-1))

#define _ASSERT_UNSIGNED(type, ...)          _Static_assert((type)(-1) > 0, #type " is not unsigned");
#define _ASSERT_SIZEOF(x, expect)            _Static_assert(sizeof(x) == (expect), #x " is not expected size")
#define _DECLARE_PROTOTYPE(type, name, ...)  extern type bit_##name##_##type(__VA_ARGS__);


_BIT_TYPES(_ASSERT_UNSIGNED, _)

#if (__STDC_VERSION__ >= 202311L)
   #include <stdbit.h>

   #define bit_trailing_zeros(w)             stdc_trailing_zeros(w)
   #define bit_leading_zeros(w)              stdc_leading_zeros(w)
   #define bit_count_ones(w)                 stdc_count_ones(w)
   #define bit_first_trailing_one(w)         stdc_first_trailing_one(w)
   #define bit_first_leading_one(w)          stdc_first_leading_one(w)

   #define bit_trailing_ones(w)              stdc_trailing_ones(w)
   #define bit_leading_ones(w)               stdc_leading_ones(w)
   #define bit_count_zeros(w)                stdc_count_zeros(w)
   #define bit_first_trailing_zero(w)        stdc_first_trailing_zero(w)
   #define bit_first_leading_zero(w)         stdc_first_leading_zero(w)

   #define bit_width(w)                      stdc_bit_width(w)
   #define bit_ceil(w)                       stdc_bit_ceil(w)
   #define bit_floor(w)                      stdc_bit_floor(w)

#else
   _BIT_TYPES(_BIT_C23_PROTOS, _DECLARE_PROTOTYPE)

   #define bit_trailing_zeros(w)             _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_trailing_zeros_) XMACRO_GENERIC_DEFAULT(0))(w)
   #define bit_leading_zeros(w)              _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_leading_zeros_) XMACRO_GENERIC_DEFAULT(0))(w)
   #define bit_count_ones(w)                 _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_count_ones_) XMACRO_GENERIC_DEFAULT(0))(w)
   #define bit_first_trailing_one(w)         _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_first_trailing_one_) XMACRO_GENERIC_DEFAULT(0))(w)
   #define bit_first_leading_one(w)          _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_first_leading_one_) XMACRO_GENERIC_DEFAULT(0))(w)

   #define bit_trailing_ones(w)              bit_trailing_zeros((typeof(w))~(w))
   #define bit_leading_ones(w)               bit_leading_zeros((typeof(w))~(w))
   #define bit_count_zeros(w)                bit_count_ones((typeof(w))~(w))
   #define bit_first_trailing_zero(w)        bit_first_trailing_one((typeof(w))~(w))
   #define bit_first_leading_zero(w)         bit_first_leading_one((typeof(w))~(w))

   #define bit_width(w)                      _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_width_) XMACRO_GENERIC_DEFAULT(0))(w)
   #define bit_ceil(w)                       _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_ceil_)  XMACRO_GENERIC_DEFAULT(0))(w)
   #define bit_floor(w)                      _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_floor_) XMACRO_GENERIC_DEFAULT(0))(w)

#endif /* C23 PROTOS */


#if (__STDC_VERSION__ > 202311L) /* C2Y */
   #include <stdbit.h>

   #define bit_rotate_left(w, rot)           stdc_rotate_left(w, rot)
   #define bit_rotate_right(w, rot)          stdc_rotate_right(w, rot)

#elif (defined(__GNUC__) && (__GNUC__ >= 15)) || ((defined(__clang__) || defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__)) && (__clang_major__ >= 23))

   #define bit_rotate_left(w, rot)           __builtin_stdc_rotate_left(w, rot)
   #define bit_rotate_right(w, rot)          __builtin_stdc_rotate_right(w, rot)

#else
   _BIT_TYPES(_BIT_C2Y_PROTOS, _DECLARE_PROTOTYPE)

   #define bit_rotate_left(w, rot)           _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_rotate_left_) XMACRO_GENERIC_DEFAULT(0))(w, rot)
   #define bit_rotate_right(w, rot)          _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_rotate_right_) XMACRO_GENERIC_DEFAULT(0))(w, rot)

#endif /* C2Y PROTOS */


_BIT_TYPES(_BIT_LIB_PROTOS, _DECLARE_PROTOTYPE)

#define bit_rotate_left_part(w, rot, cnt)    _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_rotate_left_part_) XMACRO_GENERIC_DEFAULT(0))(w, rot, cnt)
#define bit_rotate_right_part(w, rot, cnt)   _Generic((w), _BIT_TYPES(XMACRO_GENERIC_ENTRY, bit_rotate_right_part_) XMACRO_GENERIC_DEFAULT(0))(w, rot, cnt)




#define BIT_SWAP(cnt, off, a, b) do {                                         \
   /* assume no sideeffects and cnt + off <= bits(a) */                       \
   typecheck(typeof(a), b);                                                   \
   typeof(b) _UNIQUE(__tmp) = (a);                                            \
   typeof(a) _UNIQUE(__mask) = ((((typeof(a))1) << (cnt)) - 1) << (off);      \
   (a) = ((a) & ~_UNIQUE(__mask)) | ((b) & _UNIQUE(__mask));                  \
   (b) = ((b) & ~_UNIQUE(__mask)) | (_UNIQUE(__tmp) & _UNIQUE(__mask));       \
} while (0)

#define BIT_SWAP_LS(cnt, a, b) BIT_SWAP(cnt, 0, a, b)
#define BIT_SWAP_MS(cnt, a, b) BIT_SWAP(cnt, _UWIDTH(a) - (cnt), a, b)

#define bit_swap(cnt, off, a, b) do {                                         \
   typeof(cnt) *_UNIQUE(__cnt) = &(cnt);                                      \
   typeof(off) *_UNIQUE(__off) = &(off);                                      \
   typeof(a) *_UNIQUE(__a) = &(a);                                            \
   typeof(b) *_UNIQUE(__b) = &(b);                                            \
   assert(*_UNIQUE(__cnt) + *_UNIQUE(__off) <= _UWIDTH(*_UNIQUE(__a)));       \
   BIT_SWAP(*_UNIQUE(__cnt), *_UNIQUE(__off), *_UNIQUE(__a), *_UNIQUE(__b));  \
} while(0)

#define bit_swap_ls(cnt, a, b) do {                                           \
   typeof(cnt) *_UNIQUE(__cnt) = &(cnt);                                      \
   typeof(a) *_UNIQUE(__a) = &(a);                                            \
   typeof(b) *_UNIQUE(__b) = &(b);                                            \
   assert(*_UNIQUE(__cnt) <= _UWIDTH(*_UNIQUE(__a)));                         \
   BIT_SWAP_LS(*_UNIQUE(__cnt), *_UNIQUE(__a), *_UNIQUE(__b));                \
} while(0)

#define bit_swap_ms(cnt, a, b) do {                                           \
   typeof(cnt) *_UNIQUE(__cnt) = &(cnt);                                      \
   typeof(a) *_UNIQUE(__a) = &(a);                                            \
   typeof(b) *_UNIQUE(__b) = &(b);                                            \
   assert(*_UNIQUE(__cnt) <= _UWIDTH(*_UNIQUE(__a)));                         \
   BIT_SWAP_MS(*_UNIQUE(__cnt), *_UNIQUE(__a), *_UNIQUE(__b));                \
} while(0)






#ifdef HUZLIB_BIT_IMPL


#if (__STDC_VERSION__ <= 201710L)


#define _CTZ_128(ret, w) do {                                                    \
   uint64_t _UNIQUE(_low) = (uint64_t)(w);                                       \
   if (_UNIQUE(_low) != 0)                                                       \
      *(ret) = bit_trailing_zeros(_UNIQUE(_low));                                \
   else                                                                          \
      *(ret) = 64 + bit_trailing_zeros((uint64_t)((w) >> 64));                   \
} while (0)

#define _CTZ_256(ret, w) do {                                                    \
   _ASSERT_SIZEOF(w, 8);                                                         \
   _ASSERT_SIZEOF(*(ret), 32);                                                   \
   for (int _UNIQUE(_i) = 0; _UNIQUE(_i) < 4; _UNIQUE(_i)++) {                   \
      uint64_t _UNIQUE(_part) = (uint64_t)((w) >> (_UNIQUE(_i) * 64));           \
      if (_UNIQUE(_part) != 0) {                                                 \
         *(ret) = (_UNIQUE(_i) * 64) + bit_trailing_zeros(_UNIQUE(_part));       \
         break;                                                                  \
      }                                                                          \
   }                                                                             \
} while(0)

#define _CLZ_128(ret, w) do {                                                    \
   uint64_t _UNIQUE(_high) = (uint64_t)((w) >> 64);                              \
   if (_UNIQUE(_high) != 0)                                                      \
      *(ret) = bit_leading_zeros(_UNIQUE(_high));                                \
   else                                                                          \
      *(ret) = 64 + bit_leading_zeros((uint64_t)w);                              \
} while(0)

#define _CLZ_256(ret, w) do {                                                    \
   for (int _UNIQUE(_i) = 3; _UNIQUE(_i) >= 0; _UNIQUE(_i)--) {                  \
      uint64_t _UNIQUE(_part) = (uint64_t)((w) >> (_UNIQUE(_i) * 64));           \
      if (_UNIQUE(_part) != 0) {                                                 \
         *(ret) = ((3 - _UNIQUE(_i)) * 64) + bit_leading_zeros(_UNIQUE(_part));  \
         break;                                                                  \
      }                                                                          \
   }                                                                             \
} while(0)

#define _POPC_128(ret, w) do {                                                   \
   *(ret) = bit_count_ones((uint64_t)(w))                                        \
      + bit_count_ones((uint64_t)((w) >> 64));                                   \
} while(0)

#define _POPC_256(ret, w) do {                                                   \
   *(ret) = 0;                                                                   \
   for (int _UNIQUE(_i) = 3; _UNIQUE(_i) >= 0; _UNIQUE(_i)--)                    \
      *(ret) += bit_count_ones((uint64_t)((w) >> (_UNIQUE(_i) * 64)));           \
}


/*
 * default software emulation
 *
 * NOTE: 
 * this is defined here because some compilers contain
 * partial intrinstics. therefore the default might be
 * applied in more than the default branch itself
 */
#if defined(BIT_DEFAULT_FALLBACK_HIGH_MEMORY)

   /*
    * use De Bruijn Algorithm
    *    time:  O(1)
    *    space: O(n)
    */

   #define _GENERATE_TRAILING_ZEROS_DEFAULT(type)                             \
   inline type bit_trailing_zeros_##type(type w)                              \
   {                                                                          \
      if (w == 0)                                                             \
         return _UWIDTH(type);                                                \
                                                                              \
      if (sizeof(type) == 1)                                                  \
      {                                                                       \
         static const unsigned char table[8] = {                              \
            0, 1, 2, 6, 3, 4, 5, 7                                            \
         };                                                                   \
         return table[((w & -w) * 0x1D) >> 5];                                \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 2)                                                  \
      {                                                                       \
         static const unsigned char table[16] = {                             \
            0, 1, 2, 13, 3, 7, 14, 11,                                        \
            4, 9, 8, 12, 15, 10, 5, 6                                         \
         };                                                                   \
         return table[((w & -w) * 0x4AF) >> 12];                              \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 4)                                                  \
      {                                                                       \
         static const unsigned char table[32] = {                             \
            0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,         \
            31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9        \
         };                                                                   \
         return table[((w & -w) * 0x077CB531U) >> 27];                        \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 8)                                                  \
      {                                                                       \
         static const unsigned char table[64] = {                             \
            0, 1, 2, 53, 3, 7, 54, 27, 4, 38, 41, 8, 34, 55, 48, 28,          \
            62, 5, 39, 46, 44, 42, 22, 9, 24, 35, 59, 56, 49, 18, 29, 11,     \
            63, 52, 6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,    \
            51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12    \
         };                                                                   \
         return table[((w & -w) * 0x03F79D71B4CB0A89ULL) >> 58];              \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 16)                                                 \
      {                                                                       \
         /* 128-bit - fallback to 64-bit halves */                            \
         type ret;                                                            \
         _CTZ_128(&ret, w);                                                   \
         return ret;                                                          \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 32)                                                 \
      {                                                                       \
         /* 256-bit - fallback to 64-bit quarters */                          \
         type ret;                                                            \
         _CTZ_256(&ret, w);                                                   \
         return ret;                                                          \
      }                                                                       \
                                                                              \
      assert(0);                                                              \
   }

   #define _GENERATE_LEADING_ZEROS_DEFAULT(type)                              \
   inline type bit_leading_zeros_##type(type w)                               \
   {                                                                          \
      if (w == 0)                                                             \
         return _UWIDTH(type);                                                \
                                                                              \
      if (sizeof(type) == 1)                                                  \
      {                                                                       \
         static const unsigned char table[8] = {                              \
            7, 6, 5, 5, 4, 4, 4, 4                                            \
         };                                                                   \
         w |= w >> 1;                                                         \
         w |= w >> 2;                                                         \
         w |= w >> 4;                                                         \
         return table[(w * 0x1D) >> 5];                                       \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 2)                                                  \
      {                                                                       \
         static const unsigned char table[16] = {                             \
            15, 14, 13, 12, 11, 10, 9, 8,                                     \
            7, 6, 5, 5, 4, 4, 4, 4                                            \
         };                                                                   \
         w |= w >> 1;                                                         \
         w |= w >> 2;                                                         \
         w |= w >> 4;                                                         \
         w |= w >> 8;                                                         \
         return table[(w * 0x4AF) >> 12];                                     \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 4)                                                  \
      {                                                                       \
         static const unsigned char table[32] = {                             \
            31, 22, 30, 21, 18, 10, 29, 2, 20, 17, 15, 13, 9, 6, 28, 1,       \
            23, 19, 11, 3, 16, 14, 7, 24, 12, 4, 8, 25, 5, 26, 27, 0          \
         };                                                                   \
         w |= w >> 1;                                                         \
         w |= w >> 2;                                                         \
         w |= w >> 4;                                                         \
         w |= w >> 8;                                                         \
         w |= w >> 16;                                                        \
         return table[(w * 0x07C4ACDDU) >> 27];                               \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 8)                                                  \
      {                                                                       \
         static const unsigned char table[64] = {                             \
            63, 52, 53, 51, 54, 49, 50, 48, 55, 45, 46, 43, 47, 40, 41, 39,   \
            56, 35, 36, 33, 37, 30, 31, 29, 38, 26, 27, 24, 28, 22, 23, 21,   \
            57, 44, 34, 32, 25, 20, 19, 18, 42, 17, 16, 15, 14, 13, 12, 11,   \
            58, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 62, 61, 60, 59              \
         };                                                                   \
         w |= w >> 1;                                                         \
         w |= w >> 2;                                                         \
         w |= w >> 4;                                                         \
         w |= w >> 8;                                                         \
         w |= w >> 16;                                                        \
         w |= w >> 32;                                                        \
         return table[(w * 0x03F566ED27179461ULL) >> 58];                     \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 16)                                                 \
      {                                                                       \
         /* 128-bit - fallback to 64-bit halves */                            \
         type ret;                                                            \
         _CLZ_128(&ret, w);                                                   \
         return ret;                                                          \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 32)                                                 \
      {                                                                       \
         /* 256-bit - fallback to 64-bit quarters */                          \
         type ret;                                                            \
         _CLZ_256(&ret, w);                                                   \
         return ret;                                                          \
      }                                                                       \
                                                                              \
      assert(0);                                                              \
   }

#elif defined(BIT_DEFAULT_FALLBACK_LOW_MEMORY)

   /*
    * use Binary Search
    *    time:  O(logn)
    *    space: O(1)
    */

   #define _GENERATE_TRAILING_ZEROS_DEFAULT(type)                             \
   inline type bit_trailing_zeros_##type(type w)                              \
   {                                                                          \
      if (w == 0)                                                             \
         return _UWIDTH(type);                                                \
                                                                              \
      if (sizeof(type) == 1)                                                  \
      {                                                                       \
         type n = 0;                                                          \
         if ((w & 0x0F) == 0) { n += 4; w >>= 4; }                            \
         if ((w & 0x03) == 0) { n += 2; w >>= 2; }                            \
         if ((w & 0x01) == 0) { n += 1; }                                     \
         return n;                                                            \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 2)                                                  \
      {                                                                       \
         type n = 0;                                                          \
         if ((w & 0x00FF) == 0) { n += 8; w >>= 8; }                          \
         if ((w & 0x000F) == 0) { n += 4; w >>= 4; }                          \
         if ((w & 0x0003) == 0) { n += 2; w >>= 2; }                          \
         if ((w & 0x0001) == 0) { n += 1; }                                   \
         return n;                                                            \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 4)                                                  \
      {                                                                       \
         type n = 0;                                                          \
         if ((w & 0x0000FFFF) == 0) { n += 16; w >>= 16; }                    \
         if ((w & 0x000000FF) == 0) { n += 8;  w >>= 8;  }                    \
         if ((w & 0x0000000F) == 0) { n += 4;  w >>= 4;  }                    \
         if ((w & 0x00000003) == 0) { n += 2;  w >>= 2;  }                    \
         if ((w & 0x00000001) == 0) { n += 1;  }                              \
         return n;                                                            \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 8)                                                  \
      {                                                                       \
         type n = 0;                                                          \
         if ((w & 0x00000000FFFFFFFFULL) == 0) { n += 32; w >>= 32; }         \
         if ((w & 0x000000000000FFFFULL) == 0) { n += 16; w >>= 16; }         \
         if ((w & 0x00000000000000FFULL) == 0) { n += 8;  w >>= 8;  }         \
         if ((w & 0x000000000000000FULL) == 0) { n += 4;  w >>= 4;  }         \
         if ((w & 0x0000000000000003ULL) == 0) { n += 2;  w >>= 2;  }         \
         if ((w & 0x0000000000000001ULL) == 0) { n += 1;  }                   \
         return n;                                                            \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 16)                                                 \
      {                                                                       \
         /* 128-bit - fallback to 64-bit halves */                            \
         type ret;                                                            \
         _CTZ_128(&ret, w);                                                   \
         return ret;                                                          \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 32)                                                 \
      {                                                                       \
         /* 256-bit - fallback to 64-bit quarters */                          \
         type ret;                                                            \
         _CTZ_256(&ret, w);                                                   \
         return ret;                                                          \
      }                                                                       \
                                                                              \
      assert(0);                                                              \
   }

   #define _GENERATE_LEADING_ZEROS_DEFAULT(type)                              \
   inline type bit_leading_zeros_##type(type w)                               \
   {                                                                          \
      if (w == 0)                                                             \
         return _UWIDTH(type);                                                \
                                                                              \
      if (sizeof(type) == 1)                                                  \
      {                                                                       \
         type n = 0;                                                          \
         if (w <= 0x0F) { n += 4; w <<= 4; }                                  \
         if (w <= 0x3F) { n += 2; w <<= 2; }                                  \
         if (w <= 0x7F) { n += 1; }                                           \
         return n;                                                            \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 2)                                                  \
      {                                                                       \
         type n = 0;                                                          \
         if (w <= 0x00FF) { n += 8; w <<= 8; }                                \
         if (w <= 0x0FFF) { n += 4; w <<= 4; }                                \
         if (w <= 0x3FFF) { n += 2; w <<= 2; }                                \
         if (w <= 0x7FFF) { n += 1; }                                         \
         return n;                                                            \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 4)                                                  \
      {                                                                       \
         type n = 0;                                                          \
         if (w <= 0x0000FFFF) { n += 16; w <<= 16; }                          \
         if (w <= 0x00FFFFFF) { n += 8;  w <<= 8;  }                          \
         if (w <= 0x0FFFFFFF) { n += 4;  w <<= 4;  }                          \
         if (w <= 0x3FFFFFFF) { n += 2;  w <<= 2;  }                          \
         if (w <= 0x7FFFFFFF) { n += 1;  }                                    \
         return n;                                                            \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 8)                                                  \
      {                                                                       \
         type n = 0;                                                          \
         if (w <= 0x00000000FFFFFFFFULL) { n += 32; w <<= 32; }               \
         if (w <= 0x0000FFFFFFFFFFFFULL) { n += 16; w <<= 16; }               \
         if (w <= 0x00FFFFFFFFFFFFFFULL) { n += 8;  w <<= 8;  }               \
         if (w <= 0x0FFFFFFFFFFFFFFFULL) { n += 4;  w <<= 4;  }               \
         if (w <= 0x3FFFFFFFFFFFFFFFULL) { n += 2;  w <<= 2;  }               \
         if (w <= 0x7FFFFFFFFFFFFFFFULL) { n += 1;  }                         \
         return n;                                                            \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 16)                                                 \
      {                                                                       \
         /* 128-bit - fallback to 64-bit halves */                            \
         type ret;                                                            \
         _CLZ_128(&ret, w);                                                   \
         return ret;                                                          \
      }                                                                       \
                                                                              \
      if (sizeof(type) == 32)                                                 \
      {                                                                       \
         /* 256-bit - fallback to 64-bit quarters */                          \
         type ret;                                                            \
         _CLZ_256(&ret, w);                                                   \
         return ret;                                                          \
      }                                                                       \
                                                                              \
      assert(0);                                                              \
   }

#endif


#define _GENERATE_COUNT_ONES_DEFAULT(type)                                    \
inline type bit_count_ones_##type(type w)                                     \
{                                                                             \
   if (w == 0)                                                                \
      return 0;                                                               \
                                                                              \
   if (sizeof(type) == 1)                                                     \
   {                                                                          \
      static const unsigned char table[16] = {                                \
         0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4                       \
      };                                                                      \
      return table[w & 0x0F] + table[(w >> 4) & 0x0F];                        \
   }                                                                          \
                                                                              \
   if (sizeof(type) == 2)                                                     \
   {                                                                          \
      w = (w & 0x5555) + ((w >> 1) & 0x5555);                                 \
      w = (w & 0x3333) + ((w >> 2) & 0x3333);                                 \
      w = (w & 0x0F0F) + ((w >> 4) & 0x0F0F);                                 \
      w = (w & 0x00FF) + ((w >> 8) & 0x00FF);                                 \
      return w;                                                               \
   }                                                                          \
                                                                              \
   if (sizeof(type) == 4)                                                     \
   {                                                                          \
      w = w - ((w >> 1) & 0x55555555);                                        \
      w = (w & 0x33333333) + ((w >> 2) & 0x33333333);                         \
      w = (w + (w >> 4)) & 0x0F0F0F0F;                                        \
      w = w + (w >> 8);                                                       \
      w = w + (w >> 16);                                                      \
      return w & 0x3F;                                                        \
   }                                                                          \
                                                                              \
   if (sizeof(type) == 8)                                                     \
   {                                                                          \
      w = w - ((w >> 1) & 0x5555555555555555ULL);                             \
      w = (w & 0x3333333333333333ULL) + ((w >> 2) & 0x3333333333333333ULL);   \
      w = (w + (w >> 4)) & 0x0F0F0F0F0F0F0F0FULL;                             \
      w = w + (w >> 8);                                                       \
      w = w + (w >> 16);                                                      \
      w = w + (w >> 32);                                                      \
      return (type)(w & 0x7F);                                                \
   }                                                                          \
                                                                              \
   if (sizeof(type) == 16)                                                    \
   {                                                                          \
      /* 128-bit - fallback to 64-bit halves */                               \
      type ret;                                                               \
      _POPC_128(&ret, w);                                                     \
      return ret;                                                             \
   }                                                                          \
                                                                              \
   if (sizeof(type) == 32)                                                    \
   {                                                                          \
      /* 256-bit - fallback to 64-bit quarters */                             \
      type ret;                                                               \
      _POPC_256(&ret, w);                                                     \
      return ret;                                                             \
   }                                                                          \
                                                                              \
   assert(0);                                                                 \
}

#define _GENERATE_FIRST_TRAILING_ONE_CTZ_FALLBACK(type)                       \
inline type bit_first_trailing_one_##type(type w)                             \
{                                                                             \
   if (w == 0)                                                                \
      return 0;                                                               \
   else                                                                       \
      return 1 + bit_trailing_zeros_##type(w);                                \
}

#define _GENERATE_FIRST_LEADING_ONE_CLZ_FALLBACK(type)                        \
inline type bit_first_leading_one_##type(type w)                              \
{                                                                             \
   if (w == 0)                                                                \
      return 0;                                                               \
   else                                                                       \
      return _UWIDTH(w) - (1 + bit_leading_zeros_##type(w));                  \
}


/*
 * CUDA:
 *    - __clz[,ll]
 *    - __popc[,ll]
 *    - __ffs[,ll]
 */
#if defined(__NVCC__)
   #include <cuda_runtime.h>

   #define _GENERATE_TRAILING_ZEROS(type, ...)        \
   inline type bit_trailing_zeros(type w)             \
   {                                                  \
      type ret;                                       \
      if (w == 0)                                     \
         return _UWIDTH(type);                        \
                                                      \
      switch (sizeof(type))                           \
      {                                               \
         case 1:                                      \
         case 2:                                      \
         case 4:                                      \
            ret = __ffs((uint32_t)w) - 1;             \
            break;                                    \
                                                      \
         case 8:                                      \
            ret = __ffsll(w) - 1;                     \
            break;                                    \
                                                      \
         case 16:                                     \
            _CTZ_128(&ret, w);                        \
            break;                                    \
                                                      \
         case 32:                                     \
            _CTZ_256(&ret, w);                        \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

#define _GENERATE_LEADING_ZEROS(type, ...)            \
   inline type bit_leading_zeros_##type(type w)       \
   {                                                  \
      type ret;                                       \
      if (w == 0)                                     \
         return _UWIDTH(type);                        \
                                                      \
      switch (sizeof(type))                           \
      {                                               \
         case 1:                                      \
            ret = __clz((uint32_t)w) - 24;            \
            break;                                    \
                                                      \
         case 2:                                      \
            ret = __clz((uint32_t)w) - 16;            \
            break;                                    \
                                                      \
         case 4:                                      \
            ret = __clz((uint32_t)w);                 \
            break;                                    \
                                                      \
         case 8:                                      \
            ret = __clzll(w);                         \
            break;                                    \
                                                      \
         case 16:                                     \
            _CLZ_128(&ret, w);                        \
            break;                                    \
                                                      \
         case 32:                                     \
            _CLZ_256(&ret, w);                        \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_COUNT_ONES(type, ...)            \
   inline type bit_count_ones_##type(type w)          \
   {                                                  \
      type ret;                                       \
      switch (sizeof(type))                           \
      {                                               \
         case 1:                                      \
         case 2:                                      \
         case 4:                                      \
            ret = __popc((uint32_t)w);                \
            break;                                    \
                                                      \
         case 8:                                      \
            ret = __popcll(w);                        \
            break;                                    \
                                                      \
         case 16:                                     \
            _POPC_128(&ret, w);                       \
            break;                                    \
                                                      \
         case 32:                                     \
            _POPC_256(&ret, w);                       \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_FIRST_TRAILING_ONE(type, ...)    \
   inline type bit_first_trailing_one(type w)         \
   {                                                  \
      if (w == 0)                                     \
         return 0;                                    \
                                                      \
      type ret;                                       \
      switch (sizeof(type))                           \
      {                                               \
         case 1:                                      \
         case 2:                                      \
         case 4:                                      \
            ret = __ffs((uint32_t)w);                 \
            break;                                    \
                                                      \
         case 8:                                      \
            ret = __ffsll(w);                         \
            break;                                    \
                                                      \
         case 16:                                     \
            _CTZ_128(&ret, w);                        \
            ret++;                                    \
            break;                                    \
                                                      \
         case 32:                                     \
            _CTZ_256(&ret, w);                        \
            ret++;                                    \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_FIRST_LEADING_ONE(type, ...)  _GENERATE_FIRST_LEADING_ONE_CLZ_FALLBACK(type)


/*
 * GCC/Clang/Intel/ARMCLANG/OpenXL:
 *    - __builtin_ctz[,l,ll]
 *    - __builtin_clz[,l,ll]
 *    - __builtin_popcount[,l,ll]
 *    - __builtin_ffs[,l,ll]
 */
#elif defined(__INTEL_LLVM_COMPILER) || (defined(__INTEL_COMPILER) && defined(__GNUC__)) || (defined(__ARMCOMPILER_VERSION) && __ARMCOMPILER_VERSION >= 600000) || (defined(__ibmxl__) && __ibmxl__ >= 0x10010000) || (defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))) || defined(__clang__)

   #define _GENERATE_TRAILING_ZEROS(type, ...)        \
   inline type bit_trailing_zeros(type w)             \
   {                                                  \
      type ret;                                       \
      switch (sizeof(type))                           \
      {                                               \
         case sizeof(unsigned char):                  \
         case sizeof(unsigned short):                 \
         case sizeof(unsigned int):                   \
            ret = __builtin_ctz((unsigned int)w);     \
            break;                                    \
                                                      \
         case sizeof(unsigned long):                  \
            ret = __builtin_ctzl(w);                  \
            break;                                    \
                                                      \
         case sizeof(unsigned long long):             \
            ret = __builtin_ctzll(w);                 \
            break;                                    \
                                                      \
         case 16:                                     \
            _CTZ_128(&ret, w);                        \
            break;                                    \
                                                      \
         case 32:                                     \
            _CTZ_256(&ret, w);                        \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_LEADING_ZEROS(type, ...)         \
   inline type bit_leading_zeros_##type(type w)       \
   {                                                  \
      type ret;                                       \
      switch (sizeof(type))                           \
      {                                               \
         case sizeof(unsigned char):                  \
         case sizeof(unsigned short):                 \
         case sizeof(unsigned int):                   \
            ret = __builtin_clz((unsigned int)w);     \
            break;                                    \
                                                      \
         case sizeof(unsigned long):                  \
            ret = __builtin_clzl(w);                  \
            break;                                    \
                                                      \
         case sizeof(unsigned long long):             \
            ret = __builtin_clzll(w);                 \
            break;                                    \
                                                      \
         case 16:                                     \
            _CLZ_128(&ret, w);                        \
            break;                                    \
                                                      \
         case 32:                                     \
            _CLZ_256(&ret, w);                        \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_COUNT_ONES(type, ...)            \
   inline type bit_count_ones_##type(type w)          \
   {                                                  \
      type ret;                                       \
      switch (sizeof(type))                           \
      {                                               \
         case sizeof(unsigned char):                  \
         case sizeof(unsigned short):                 \
         case sizeof(unsigned int):                   \
            ret = __builtin_popcount((unsigned int)w);\
            break;                                    \
                                                      \
         case sizeof(unsigned long):                  \
            ret = __builtin_popcountl(w);             \
            break;                                    \
                                                      \
         case sizeof(unsigned long long):             \
            ret = __builtin_popcountll(w);            \
            break;                                    \
                                                      \
         case 16:                                     \
            _POPC_128(&ret, w);                       \
            break;                                    \
                                                      \
         case 32:                                     \
            _POPC_256(&ret, w);                       \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_FIRST_TRAILING_ONE(type, ...)    \
   inline type bit_first_trailing_one(type w)         \
   {                                                  \
      type ret;                                       \
      switch (sizeof(type))                           \
      {                                               \
         case sizeof(unsigned char):                  \
         case sizeof(unsigned short):                 \
         case sizeof(unsigned int):                   \
            ret = __builtin_ffs((unsigned int)w);     \
            break;                                    \
                                                      \
         case sizeof(unsigned long):                  \
            ret = __builtin_ffsl(w);                  \
            break;                                    \
                                                      \
         case sizeof(unsigned long long):             \
            ret = __builtin_ffsll(w);                 \
            break;                                    \
                                                      \
         case 16:                                     \
            _CTZ_128(&ret, w);                        \
            ret++;                                    \
            break;                                    \
                                                      \
         case 32:                                     \
            _CTZ_256(&ret, w);                        \
            ret++;                                    \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_FIRST_LEADING_ONE(type, ...)  _GENERATE_FIRST_LEADING_ONE_CLZ_FALLBACK(type)


/*
 * IBM Classic XL:
 *    - __cnttz[4,8]
 *    - __cntlz[4,8]
 *    - __popcnt[4,8]
 */
#elif defined(__xlC__) || defined(__ibmxl__)
   #include <builtins.h>

   #define _GENERATE_TRAILING_ZEROS(type, ...)        \
   inline type bit_trailing_zeros(type w)             \
   {                                                  \
      type ret;                                       \
      if (w == 0)                                     \
         return _UWIDTH(type);                        \
                                                      \
      switch (sizeof(type))                           \
      {                                               \
         case 1:                                      \
         case 2:                                      \
         case 4:                                      \
            ret = __cnttz4((uint32_t)w);              \
            break;                                    \
                                                      \
         case 8:                                      \
            ret = __cnttz8(w);                        \
            break;                                    \
                                                      \
         case 16:                                     \
            _CTZ_128(&ret, w);                        \
            break;                                    \
                                                      \
         case 32:                                     \
            _CTZ_256(&ret, w);                        \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_LEADING_ZEROS(type, ...)         \
   inline type bit_leading_zeros_##type(type w)       \
   {                                                  \
      type ret;                                       \
      if (w == 0)                                     \
         return _UWIDTH(type);                        \
                                                      \
      switch (sizeof(type))                           \
      {                                               \
         case 1:                                      \
            ret = __cntlz4((uint32_t)w) - 24;         \
            break;                                    \
                                                      \
         case 2:                                      \
            ret = __cntlz4((uint32_t)w) - 16;         \
            break;                                    \
                                                      \
         case 4:                                      \
            ret = __cntlz4((uint32_t)w);              \
            break;                                    \
                                                      \
         case 8:                                      \
            ret = __cntlz8(w);                        \
            break;                                    \
                                                      \
         case 16:                                     \
            _CLZ_128(&ret, w);                        \
            break;                                    \
                                                      \
         case 32:                                     \
            _CLZ_256(&ret, w);                        \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_COUNT_ONES(type, ...)            \
   inline type bit_count_ones_##type(type w)          \
   {                                                  \
      type ret;                                       \
      switch (sizeof(type))                           \
      {                                               \
         case 1:                                      \
         case 2:                                      \
         case 4:                                      \
            ret = __popcnt4((uint32_t)w);             \
            break;                                    \
                                                      \
         case 8:                                      \
            ret = __popcnt8(w);                       \
            break;                                    \
                                                      \
         case 16:                                     \
            _POPC_128(&ret, w);                       \
            break;                                    \
                                                      \
         case 32:                                     \
            _POPC_256(&ret, w);                       \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_FIRST_TRAILING_ONE(type, ...) _GENERATE_FIRST_TRAILING_ONE_CTZ_FALLBACK(type)
   #define _GENERATE_FIRST_LEADING_ONE(type, ...)  _GENERATE_FIRST_LEADING_ONE_CLZ_FALLBACK(type)


/*
 * MSVC / Pelles C:
 *    x86:
 *       - _BitScanForward[,64]
 *       - _BitScanReverse[,64]
 *       - __tzcnt[16, ,64]
 *       - __lzcnt[16, ,64]
 *       - __popcnt[16, ,64]
 *    ARM:
 *       - _BitScanForward[,64] || _CountTrailingZeros[,64]
 *       - _BitScanReverse[,64] || _CountLeadingZeros[,64]
 *       - _CountOneBits[,64]
 */
#elif defined(_MSC_VER) || defined(__POCC__)
   #include <intrin.h>

   #if defined(__AVX2__) || defined(__BMI__) || defined(__LZCNT__) || defined(__POPCNT__)

      #define _CTZ_16(ret, w)       do { *(ret) = __tzcnt16(w); } while(0)
      #define _CTZ_32(ret, w)       do { *(ret) = __tzcnt(w); } while(0)
      #define _CLZ_16(ret, w)       do { *(ret) = __lzcnt16(w); } while(0)
      #define _CLZ_32(ret, w)       do { *(ret) = __lzcnt(w); } while(0)
      #define _POPC_16(ret, w)      do { *(ret) = __popcnt16(w); } while(0)
      #define _POPC_32(ret, w)      do { *(ret) = __popcnt(w); } while(0)
      #define _MVSC_HAS_POPC 1

      #if defined(_M_X64)
         #define _CTZ_64(ret, w)    do { *(ret) = __tzcnt64(w); } while(0)
         #define _CLZ_64(ret, w)    do { *(ret) = __lzcnt64(w); } while(0)
         #define _POPC_64(ret, w)   do { *(ret) = __popcnt64(w); } while(0)
      #else
         #define _CTZ_64(ret, w)    do {                       \
            uint32_t _UNIQUE(_low) = (uint32_t)(w);            \
            if (_UNIQUE(_low) != 0)                            \
               *(ret) = __tzcnt(_UNIQUE(_low));                \
            else                                               \
               *(ret) = 32 + __tzcnt((uint32_t)((w) >> 32));   \
         } while(0)

         #define _CLZ_64(ret, w)    do {                       \
            uint32_t _UNIQUE(_high) = (uint32_t)((w) >> 32);   \
            if (_UNIQUE(_high) != 0)                           \
               *(ret) = __lzcnt(_UNIQUE(_high));               \
            else                                               \
               *(ret) = 32 + __lzcnt((uint32_t)(w));           \
         } while(0);

         #define _POPC_64(ret, w)   do {                       \
            *(ret) = __popcnt((uint32_t)(w))                   \
               + __popcnt((uint32_t)((w) >> 32));              \
         } while(0)
      #endif
 
   #else

      #define _CTZ_16(ret, w)       do { _BitScanForward(ret, w); } while(0)
      #define _CTZ_32(ret, w)       do { _BitScanForward(ret, w); } while(0)
      #define _CLZ_16(ret, w)       do { _BitScanReverse(ret, w); *(ret) -= 16; } while(0)
      #define _CLZ_32(ret, w)       do { _BitScanReverse(ret, w); } while(0)

      #if defined(_M_ARM) || defined(_M_ARM64)
         #define _POPC_16(ret, w)   do { *(ret) = _CountOneBits(w); } while(0)
         #define _POPC_32(ret, w)   do { *(ret) = _CountOneBits(w); } while(0)
         #define _MVSC_HAS_POPC 1
      #endif

      #if defined(_M_X64) || defined(_M_ARM64)
         #define _CTZ_64(ret, w)    do { _BitScanForward64(ret, w); } while(0)
         #define _CLZ_64(ret, w)    do { _BitScanReverse64(ret, w); } while(0)
         #ifdef _M_ARM64
         #define _POPC_64(ret, w)   do { *(ret) = _CountOneBits64(w); } while(0)
         #endif
      #else
         #define _CTZ_64(ret, w)    do {                       \
            uint32_t _UNIQUE(_low) = (uint32_t)(w);            \
            if (_UNIQUE(_low) != 0)                            \
               _BitScanForward(ret, _UNIQUE(_low));            \
            else {                                             \
               _BitScanForward(ret, (uint32_t)((w) >> 32));    \
               *(ret) += 32;                                   \
            }                                                  \
         } while(0)

         #define _CLZ_64(ret, w)    do {                       \
            uint32_t _UNIQUE(_high) = (uint32_t)((w) >> 32);   \
            if (_UNIQUE(_high) != 0)                           \
               _BitScanReverse(ret, _UNIQUE(_high));           \
            else {                                             \
               _BitScanReverse(ret, (uint32_t)(w));            \
               *(ret) += 32;                                   \
            }                                                  \
         } while(0)

         #ifdef _M_ARM
            #define _POPC_64(ret, w) do {                      \
               *(ret) = _CountOneBits((uint32_t)(w))           \
                  + _CountOneBits((uint32_t)((w) >> 32));      \
            } while(0)
         #endif
      #endif

   #endif /* fast intrinsics (only modern x86) */

   #define _GENERATE_TRAILING_ZEROS(type, ...)        \
   inline type bit_trailing_zeros(type w)             \
   {                                                  \
      type ret;                                       \
      if (w == 0)                                     \
         return _UWIDTH(type);                        \
                                                      \
      switch (sizeof(type))                           \
      {                                               \
         case 1:                                      \
         case 2:     _CTZ_16(&ret, w);    break;      \
         case 4:     _CTZ_32(&ret, w);    break;      \
         case 8:     _CTZ_64(&ret, w);    break;      \
         case 16:    _CTZ_128(&ret, w);   break;      \
         case 32:    _CTZ_256(&ret, w);   break;      \
         default:    assert(0);           break;      \
      }                                               \
      return ret;                                     \
   }

   #define _GENERATE_LEADING_ZEROS(type, ...)         \
   inline type bit_leading_zeros_##type(type w)       \
   {                                                  \
      type ret;                                       \
      if (w == 0)                                     \
         return _UWIDTH(type);                        \
                                                      \
      switch (sizeof(type))                           \
      {                                               \
         case 1:     _CLZ_16(&ret, w);                \
                     ret -= 8;            break;      \
         case 2:     _CLZ_16(&ret, w);    break;      \
         case 4:     _CLZ_32(&ret, w);    break;      \
         case 8:     _CLZ_64(&ret, w);    break;      \
         case 16:    _CLZ_128(&ret, w);   break;      \
         case 32:    _CLZ_256(&ret, w);   break;      \
         default:    assert(0);           break;      \
      }                                               \
      return ret;                                     \
   }

   #ifdef _MVSC_HAS_POPC
      #define _GENERATE_COUNT_ONES(type, ...)         \
      inline type bit_count_ones_##type(type w)       \
      {                                               \
         type ret;                                    \
         switch (sizeof(type))                        \
         {                                            \
            case 1:                                   \
            case 2:  _POPC_16(&ret, w);   break;      \
            case 4:  _POPC_32(&ret, w);   break;      \
            case 8:  _POPC_64(&ret, w);   break;      \
            case 16: _POPC_128(&ret, w);  break;      \
            case 32: _POPC_256(&ret, w);  break;      \
            default: assert(0);           break;      \
         }                                            \
         return ret;                                  \
      }
   #else
      #define _GENERATE_COUNT_ONES(type, ...)      _GENERATE_COUNT_ONES_DEFAULT(type)
   #endif
   #undef _MVSC_HAS_POPC

   #define _GENERATE_FIRST_TRAILING_ONE(type, ...) _GENERATE_FIRST_TRAILING_ONE_CTZ_FALLBACK(type)
   #define _GENERATE_FIRST_LEADING_ONE(type, ...)  _GENERATE_FIRST_LEADING_ONE_CLZ_FALLBACK(type)

   #undef _POPC_16
   #undef _POPC_32
   #undef _POPC_64
   #undef _CLZ_16
   #undef _CLZ_32
   #undef _CLZ_64
   #undef _CTZ_16
   #undef _CTZ_32
   #undef _CTZ_64


/*
 * SunCC:
 *    - __builtin_popcount[,l,ll]
 */
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)

   #define _GENERATE_TRAILING_ZEROS(type, ...)     _GENERATE_TRAILING_ZEROS_DEFAULT(type)
   #define _GENERATE_LEADING_ZEROS(type, ...)      _GENERATE_LEADING_ZEROS_DEFAULT(type)
   #define _GENERATE_FIRST_TRAILING_ONE(type, ...) _GENERATE_FIRST_TRAILING_ONE_CTZ_FALLBACK(type)
   #define _GENERATE_FIRST_LEADING_ONE(type, ...)  _GENERATE_FIRST_LEADING_ONE_CLZ_FALLBACK(type)

   #define _GENERATE_COUNT_ONES(type, ...)            \
   inline type bit_count_ones_##type(type w)          \
   {                                                  \
      type ret;                                       \
      switch (sizeof(type))                           \
      {                                               \
         case sizeof(unsigned char):                  \
         case sizeof(unsigned short):                 \
         case sizeof(unsigned int):                   \
            ret = __builtin_popcount((unsigned int)w);\
            break;                                    \
                                                      \
         case sizeof(unsigned long):                  \
            ret = __builtin_popcountl(w);             \
            break;                                    \
                                                      \
         case sizeof(unsigned long long):             \
            ret = __builtin_popcountll(w);            \
            break;                                    \
                                                      \
         case 16:                                     \
            _POPC_128(&ret, w);                       \
            break;                                    \
                                                      \
         case 32:                                     \
            _POPC_256(&ret, w);                       \
            break;                                    \
                                                      \
         default:                                     \
            assert(0);                                \
            break;                                    \
      }                                               \
      return ret;                                     \
   }


/*
 * default:
 */
#else

   #define _GENERATE_TRAILING_ZEROS(type, ...)     _GENERATE_TRAILING_ZEROS_DEFAULT(type)
   #define _GENERATE_LEADING_ZEROS(type, ...)      _GENERATE_LEADING_ZEROS_DEFAULT(type)
   #define _GENERATE_COUNT_ONES(type, ...)         _GENERATE_COUNT_ONES_DEFAULT(type)
   #define _GENERATE_FIRST_TRAILING_ONE(type, ...) _GENERATE_FIRST_TRAILING_ONE_CTZ_FALLBACK(type)
   #define _GENERATE_FIRST_LEADING_ONE(type, ...)  _GENERATE_FIRST_LEADING_ONE_CLZ_FALLBACK(type)

#endif


#define _GENERATE_WIDTH(type, ...)                                \
inline type bit_width_##type(type w)                              \
{                                                                 \
   return (w == 0)                                                \
      ? 0                                                         \
      : _UWIDTH(w) - bit_leading_zeros_##type(w);                 \
}

#define _GENERATE_CEIL(type, ...)                                 \
inline type bit_ceil_##type(type w)                               \
{                                                                 \
   return (w == 0)                                                \
      ? 1                                                         \
      :(1ull << (_UWIDTH(w) - bit_leading_zeros_##type(w - 1)));  \
}

#define _GENERATE_FLOOR(type, ...)                                \
inline type bit_floor_##type(type w)                              \
{                                                                 \
   return (w == 0)                                                \
      ? 0                                                         \
      : (1ull << (_UWIDTH(w) - 1 - bit_leading_zeros_##type(w))); \
}

_BIT_TYPES(_GENERATE_TRAILING_ZEROS, _)
_BIT_TYPES(_GENERATE_LEADING_ZEROS, _)
_BIT_TYPES(_GENERATE_COUNT_ONES, _)
_BIT_TYPES(_GENERATE_FIRST_TRAILING_ONE, _)
_BIT_TYPES(_GENERATE_FIRST_LEADING_ONE, _)
_BIT_TYPES(_GENERATE_WIDTH, _)
_BIT_TYPES(_GENERATE_CEIL, _)
_BIT_TYPES(_GENERATE_FLOOR, _)

#undef _GENERATE_BIT_FLOOR
#undef _GENERATE_BIT_CEIL
#undef _GENERATE_BIT_WIDTH
#undef _GENERATE_FIRST_LEADING_ONE
#undef _GENERATE_FIRST_TRAILING_ONE
#undef _GENERATE_COUNT_ONES
#undef _GENERATE_LEADING_ZEROS
#undef _GENERATE_TRAILING_ZEROS

#undef _POPC_128
#undef _POPC_256
#undef _CLZ_128
#undef _CLZ_256
#undef _CTZ_128
#undef _CTZ_256

#endif /* C23 PROTOS */


#if !((__STDC_VERSION__ > 202311L) || (defined(__GNUC__) && (__GNUC__ >= 15)) || ((defined(__clang__) || defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__)) && (__clang_major__ >= 23)))

/* ------- NEW ------------- */

#define _ROTL_GENERIC(w, rot) (((w) << (rot)) | ((w) >> (_UWIDTH(w) - (rot))))
#define _ROTR_GENERIC(w, rot) (((w) >> (rot)) | ((w) << (_UWIDTH(w) - (rot))))

#define _ROTL_64_SPLIT(ret, w, rot) do {                                         \
   uint32_t _UNIQUE(__lo), _UNIQUE(__hi);                                        \
   if ((rot) < _UWIDTH(w) / 2) {                                                 \
      _UNIQUE(__lo) = bit_rotate_left((uint32_t)(w), rot);                       \
      _UNIQUE(__hi) = bit_rotate_left((uint32_t)((w) >> 32), rot);               \
      BIT_SWAP_LS(rot, _UNIQUE(__lo), _UNIQUE(__hi));                            \
   } else {                                                                      \
      uint32_t _UNIQUE(__rot) = _UWIDTH(w) - (rot);                              \
      _UNIQUE(__lo) = bit_rotate_right((uint32_t)(w), _UNIQUE(__rot));           \
      _UNIQUE(__hi) = bit_rotate_right((uint32_t)((w) >> 32), _UNIQUE(__rot));   \
      BIT_SWAP_MS(_UNIQUE(__rot), _UNIQUE(__lo), _UNIQUE(__hi));                 \
   }                                                                             \
   *(ret) = (typeof(w))_UNIQUE(__lo) | ((typeof(w))_UNIQUE(__hi) << 32);         \
} while(0)

#define _ROTR_64_SPLIT(ret, w, rot) do {                                         \
   uint32_t _UNIQUE(__lo), _UNIQUE(__hi);                                        \
   if ((rot) < _UWIDTH(w) / 2) {                                                 \
      _UNIQUE(__lo) = bit_rotate_right((uint32_t)(w), rot);                      \
      _UNIQUE(__hi) = bit_rotate_right((uint32_t)((w) >> 32), rot);              \
      BIT_SWAP_LS(rot, _UNIQUE(__lo), _UNIQUE(__hi));                            \
   } else {                                                                      \
      uint32_t _UNIQUE(__rot) = _UWIDTH(w) - (rot);                              \
      _UNIQUE(__lo) = bit_rotate_left((uint32_t)(w), _UNIQUE(__rot));            \
      _UNIQUE(__hi) = bit_rotate_left((uint32_t)((w) >> 32), _UNIQUE(__rot));    \
      BIT_SWAP_MS(_UNIQUE(__rot), _UNIQUE(__lo), _UNIQUE(__hi));                 \
   }                                                                             \
   *(ret) = (typeof(w))_UNIQUE(__lo) | ((typeof(w))_UNIQUE(__hi) << 32);         \
} while(0)

#define _ROTL_128_SPLIT(ret, w, rot) do {                                        \
   uint64_t _UNIQUE(__lo), _UNIQUE(__hi);                                        \
   if ((rot) < _UWIDTH(w) / 2) {                                                 \
      _UNIQUE(__lo) = bit_rotate_left((uint64_t)(w), rot);                       \
      _UNIQUE(__hi) = bit_rotate_left((uint64_t)((w) >> 64), rot);               \
      BIT_SWAP_LS(rot, _UNIQUE(__lo), _UNIQUE(__hi));                            \
   } else {                                                                      \
      uint64_t _UNIQUE(__rot) = _UWIDTH(w) - (rot);                              \
      _UNIQUE(__lo) = bit_rotate_right((uint64_t)(w), _UNIQUE(__rot));           \
      _UNIQUE(__hi) = bit_rotate_right((uint64_t)((w) >> 64), _UNIQUE(__rot));   \
      BIT_SWAP_MS(_UNIQUE(__rot), _UNIQUE(__lo), _UNIQUE(__hi));                 \
   }                                                                             \
   *(ret) = (typeof(w))_UNIQUE(__lo) | ((typeof(w))_UNIQUE(__hi) << 64);         \
} while(0)

#define _ROTR_128_SPLIT(ret, w, rot) do {                                        \
   uint64_t _UNIQUE(__lo), _UNIQUE(__hi);                                        \
   if ((rot) < _UWIDTH(w) / 2) {                                                 \
      _UNIQUE(__lo) = bit_rotate_right((uint64_t)(w), rot);                      \
      _UNIQUE(__hi) = bit_rotate_right((uint64_t)((w) >> 64), rot);              \
      BIT_SWAP_LS(rot, _UNIQUE(__lo), _UNIQUE(__hi));                            \
   } else {                                                                      \
      uint64_t _UNIQUE(__rot) = _UWIDTH(w) - (rot);                              \
      _UNIQUE(__lo) = bit_rotate_left((uint64_t)(w), _UNIQUE(__rot));            \
      _UNIQUE(__hi) = bit_rotate_left((uint64_t)((w) >> 64), _UNIQUE(__rot));    \
      BIT_SWAP_MS(_UNIQUE(__rot), _UNIQUE(__lo), _UNIQUE(__hi));                 \
   }                                                                             \
   *(ret) = (typeof(w))_UNIQUE(__lo) | ((typeof(w))_UNIQUE(__hi) << 64);         \
} while(0)


#if defined(_MSC_VER) || defined(__POCC__) || defined(__INTEL_COMPILER)
   #include <intrin.h>

   #if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__amd64__)

      #define _GENERATE_ROTATE_LEFT(type, ...)                 \
      inline type bit_rotate_left_##type(type w, uint8_t rot)  \
      {                                                        \
         assert(rot <= _UWIDTH(type));                         \
         type ret;                                             \
         switch(sizeof(type))                                  \
         {                                                     \
            case 1:  ret = _rotl8(w, rot);           break;    \
            case 2:  ret = _rotl16(w, rot);          break;    \
            case 4:  ret = _rotl(w, rot);            break;    \
            case 8:  ret = _rotl64(w, rot);          break;    \
            case 16: _ROTL_128_SPLIT(&ret, w, rot);  break;    \
            default: ret = _ROTL_GENERIC(w, rot);    break;    \
         }                                                     \
         return ret;                                           \
      }

      #define _GENERATE_ROTATE_RIGHT(type, ...)                \
      inline type bit_rotate_right_##type(type w, uint8_t rot) \
      {                                                        \
         assert(rot <= _UWIDTH(type));                         \
         type ret;                                             \
         switch(sizeof(type))                                  \
         {                                                     \
            case 1:  ret = _rotr8(w, rot);           break;    \
            case 2:  ret = _rotr16(w, rot);          break;    \
            case 4:  ret = _rotr(w, rot);            break;    \
            case 8:  ret = _rotr64(w, rot);          break;    \
            case 16: _ROTR_128_SPLIT(&ret, w, rot);  break;    \
            default: ret = _ROTR_GENERIC(w, rot);    break;    \
         }                                                     \
         return ret;                                           \
      }

   #else

      #define _GENERATE_ROTATE_LEFT(type, ...)                 \
      inline type bit_rotate_left_##type(type w, uint8_t rot)  \
      {                                                        \
         assert(rot <= _UWIDTH(type));                         \
         type ret;                                             \
         switch(sizeof(type))                                  \
         {                                                     \
            case 1:  ret = _rotl8(w, rot);           break;    \
            case 2:  ret = _rotl16(w, rot);          break;    \
            case 4:  ret = _rotl(w, rot);            break;    \
            case 8:  _ROTL_64_SPLIT(&ret, w, rot);   break;    \
            default: ret = _ROTL_GENERIC(w, rot);    break;    \
         }                                                     \
         return ret;                                           \
      }

      #define _GENERATE_ROTATE_RIGHT(type, ...)                \
      inline type bit_rotate_right_##type(type w, uint8_t rot) \
      {                                                        \
         assert(rot <= _UWIDTH(type));                         \
         type ret;                                             \
         switch(sizeof(type))                                  \
         {                                                     \
            case 1:  ret = _rotr8(w, rot);           break;    \
            case 2:  ret = _rotr16(w, rot);          break;    \
            case 4:  ret = _rotr(w, rot);            break;    \
            case 8:  _ROTR_64_SPLIT(&ret, w, rot);   break;    \
            default: ret = _ROTR_GENERIC(w, rot);    break;    \
         }                                                     \
         return ret;                                           \
      }

   #endif


#elif defined(__xlC__) || defined(__ibmxl__)
   #include <builtins.h>

   static inline unsigned int __rotater4(unsigned int w, unsigned int rot) {
      return __rotl(w, (32 - (rot & 31)) & 31);
   }

   static inline unsigned long __rotater8(unsigned long w, unsigned long rot) {
      return __rotl64(w, (64 - (rot & 63)) & 63);
   }

   #define _GENERATE_ROTATE_LEFT(type, ...)                    \
   inline type bit_rotate_left_##type(type w, uint8_t rot)     \
   {                                                           \
      assert(rot <= _UWIDTH(type));                            \
      type ret;                                                \
      switch(sizeof(type))                                     \
      {                                                        \
         case 4:  ret = __rotatel4(w, rot);        break;      \
         case 8:  ret = __rotatel8(w, rot);        break;      \
         case 16: _ROTL_128_SPLIT(&ret, w, rot);   break;      \
         default: ret = _ROTL_GENERIC(w, rot);     break;      \
      }                                                        \
      return ret;                                              \
   }

   #define _GENERATE_ROTATE_RIGHT(type, ...)                   \
   inline type bit_rotate_right_##type(type w, uint8_t rot)    \
   {                                                           \
      assert(rot <= _UWIDTH(type));                            \
      type ret;                                                \
      switch(sizeof(type))                                     \
      {                                                        \
         case 4:  ret = __rotater4(w, rot);        break;      \
         case 8:  ret = __rotater8(w, rot);        break;      \
         case 16: _ROTR_128_SPLIT(&ret, w, rot);   break;      \
         default: ret = _ROTR_GENERIC(w, rot);     break;      \
      }                                                        \
      return ret;                                              \
   }

#else

   #define _GENERATE_ROTATE_LEFT(type, ...)                    \
   inline type bit_rotate_left_##type(type w, uint8_t rot)     \
   {                                                           \
      assert(rot <= _UWIDTH(type));                            \
      return _ROTL_GENERIC(w, rot);                            \
   }

   #define _GENERATE_ROTATE_RIGHT(type, ...)                   \
   inline type bit_rotate_right_##type(type w, uint8_t rot)    \
   {                                                           \
      assert(rot <= _UWIDTH(type));                            \
      return _ROTR_GENERIC(w, rot);                            \
   }

#endif

_BIT_TYPES(_GENERATE_ROTATE_LEFT, _)
_BIT_TYPES(_GENERATE_ROTATE_RIGHT, _)

#undef _GENERATE_ROTATE_RIGHT
#undef _GENERATE_ROTATE_LEFT

#endif /* C2Y PROTOS */



#define _GENERATE_ROTATE_LEFT_PART(type, ...)                           \
inline type bit_rotate_left_part_##type(type w, uint8_t r, uint8_t m)   \
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

_BIT_TYPES(_GENERATE_ROTATE_LEFT_PART, _)
_BIT_TYPES(_GENERATE_ROTATE_RIGHT_PART, _)

#undef _GENERATE_ROTATE_RIGHT_PART
#undef _GENERATE_ROTATE_LEFT_PART



#endif /* HUZLIB_BIT_IMPL */






#undef _DECLARE_PROTOTYPE
#undef _ASSERT_SIZEOF
#undef _ASSERT_UNSIGNED

#undef _BIT_LIB_PROTOS
#undef _BIT_C23_PROTOS
#undef _BIT_C2Y_PROTOS
#undef _BIT_TYPES


#endif /* HUZLIB_BIT_H */
