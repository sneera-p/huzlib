#ifndef HUZLIB_BIT_H
#define HUZLIB_BIT_H


/* --------------------------------------------------------------------------- */
/* ------------------------------ utils/types.h ------------------------------ */
/* --------------------------------------------------------------------------- */

#include <stddef.h>


/*
 * Use the following compiler detection order in macros to 
 * avoid my ass being riddled with spice trying to find which 
 * one of my braincells forgot to add a compiler and maintain 
 * Human(logn) search time for this particular ass ripping
 *
 * Order:
 *    defined(__NVCC__)
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
   defined(__NVCC__) ||                         \
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
#endif


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
#endif




/* __huzuq(a)
 * -------------------
 * internal cancatation utility used to
 * create unique tmp varaible name
 *
 * WARN:
 * This macro is the internal implementation and should not be used directly.
 */
#define HUZLIB_UNIQUE_CONCAT_INTERNAL(a, b) a##b
#define HUZLIB_UNIQUE_CONCAT(a, b) HUZLIB_UNIQUE_CONCAT_INTERNAL(a, b)
#define __huzuq(name) HUZLIB_UNIQUE_CONCAT(name, __LINE__)




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
 * typecheck(type, expr)
 * ---------------------
 * Validates 'expr' matches 'type' exactly.
 */
#ifndef typecheck
#define typecheck(type, expr) _Generic(   \
   (expr),                                \
   type: 1                                \
)
#endif /* typecheck */



/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */




#include <assert.h>
#include <limits.h>

/* ------- shorthand names ------- */

#define bit_ctz(...)    bit_trailing_zeros(__VA_ARGS__)
#define bit_cto(...)    bit_trailing_ones(__VA_ARGS__)
#define bit_clz(...)    bit_leading_zeros(__VA_ARGS__)
#define bit_clo(...)    bit_leading_ones(__VA_ARGS__)

#define bit_popc(...)   bit_count_ones(__VA_ARGS__)
#define bit_popcz(...)  bit_count_zeros(__VA_ARGS__)

#define bit_ffs(...)    bit_first_trailing_one(__VA_ARGS__)
#define bit_ffsz(...)   bit_first_trailing_zero(__VA_ARGS__)
#define bit_fls(...)    bit_first_leading_one(__VA_ARGS__)
#define bit_flsz(...)   bit_first_leading_zero(__VA_ARGS__)

#define bit_rotl(...)   bit_rotate_left(__VA_ARGS__)
#define bit_rotr(...)   bit_rotate_right(__VA_ARGS__)

#define bit_rotlp(...)  bit_rotate_left_part(__VA_ARGS__)
#define bit_rotrp(...)  bit_rotate_right_part(__VA_ARGS__)




typedef unsigned char      uchar;
typedef unsigned short     ushort;
typedef unsigned int       uint;
typedef unsigned long      ulong;
typedef unsigned long long ullong;

#define HUZLIB_BIT_INTERNAL_TYPES(x, ...) \
   x(uchar, __VA_ARGS__)                  \
   x(ushort, __VA_ARGS__)                 \
   x(uint, __VA_ARGS__)                   \
   x(ulong, __VA_ARGS__)                  \
   x(ullong, __VA_ARGS__)

#define HUZLIB_BIT_INTERNAL_C23_PROTOS(type, x) \
   x(type, trailing_zeros, type w)              \
   x(type, leading_zeros, type w)               \
   x(type, count_ones, type w)                  \
   x(type, first_trailing_one, type w)          \
   x(type, first_leading_one, type w)           \
   x(type, width, type w)                       \
   x(type, ceil, type w)                        \
   x(type, floor, type w)

#define HUZLIB_BIT_INTERNAL_C2Y_PROTOS(type, x) \
   x(type, rotate_left, type w, uint8_t r)      \
   x(type, rotate_right, type w, uint8_t r)

#define HUZLIB_BIT_INTERNAL_LIB_PROTOS(type, x)             \
   x(type, rotate_left_part, type w, uint8_t r, uint8_t m)  \
   x(type, rotate_right_part, type w, uint8_t r, uint8_t m) \


#define HUZLIB_BIT_INTERNAL_TYPE_WIDTH(expr)                (sizeof(expr) * CHAR_BIT)
#define HUZLIB_BIT_INTERNAL_GENERIC_PROTO(type, func)       func##_##type

#define HUZLIB_BIT_INTERNAL_ASSERT_UNSIGNED(type, ...)      _Static_assert((type)(-1) > 0, #type " is not unsigned");
#define HUZLIB_BIT_INTERNAL_DECLARE_PROTO(type, name, ...)  extern type bit_##name##_##type(__VA_ARGS__);




HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_ASSERT_UNSIGNED, _)

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
   HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_C23_PROTOS, HUZLIB_BIT_INTERNAL_DECLARE_PROTO)

   #define bit_trailing_zeros(w)             _Generic((w),                                                                    \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_trailing_zeros),       \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_trailing_zeros),      \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_trailing_zeros),        \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_trailing_zeros),       \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_trailing_zeros)       \
                                             )(w)

   #define bit_leading_zeros(w)              _Generic((w),                                                                    \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_leading_zeros),        \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_leading_zeros),       \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_leading_zeros),         \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_leading_zeros),        \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_leading_zeros)        \
                                             )(w)

   #define bit_count_ones(w)                 _Generic((w),                                                                    \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_count_ones),           \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_count_ones),          \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_count_ones),            \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_count_ones),           \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_count_ones)           \
                                             )(w)

   #define bit_first_trailing_one(w)         _Generic((w),                                                                    \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_first_trailing_one),   \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_first_trailing_one),  \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_first_trailing_one),    \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_first_trailing_one),   \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_first_trailing_one)   \
                                             )(w)

   #define bit_first_leading_one(w)          _Generic((w),                                                                    \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_first_leading_one),    \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_first_leading_one),   \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_first_leading_one),     \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_first_leading_one),    \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_first_leading_one)    \
                                             )(w)


   #define bit_trailing_ones(w)              bit_trailing_zeros((typeof(w))~(w))
   #define bit_leading_ones(w)               bit_leading_zeros((typeof(w))~(w))
   #define bit_count_zeros(w)                bit_count_ones((typeof(w))~(w))
   #define bit_first_trailing_zero(w)        bit_first_trailing_one((typeof(w))~(w))
   #define bit_first_leading_zero(w)         bit_first_leading_one((typeof(w))~(w))

   #define bit_width(w)                      _Generic((w),                                                                    \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_width),                \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_width),               \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_width),                 \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_width),                \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_width)                \
                                             )(w)

   #define bit_ceil(w)                       _Generic((w),                                                                    \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_ceil),                 \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_ceil),                \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_ceil),                  \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_ceil),                 \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_ceil)                 \
                                             )(w)

   #define bit_floor(w)                      _Generic((w),                                                                    \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_floor),                \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_floor),               \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_floor),                 \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_floor),                \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_floor)                \
                                             )(w)

#endif /* C23 PROTOS */


#if (__STDC_VERSION__ > 202311L) /* C2Y */
   #include <stdbit.h>

   #define bit_rotate_left(w, rot)           stdc_rotate_left(w, rot)
   #define bit_rotate_right(w, rot)          stdc_rotate_right(w, rot)

#elif (defined(__GNUC__) && (__GNUC__ >= 15)) || ((defined(__clang__) || defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__)) && (__clang_major__ >= 23))

   #define bit_rotate_left(w, rot)           __builtin_stdc_rotate_left(w, rot)
   #define bit_rotate_right(w, rot)          __builtin_stdc_rotate_right(w, rot)

#else
   HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_C2Y_PROTOS, HUZLIB_BIT_INTERNAL_DECLARE_PROTO)

   #define bit_rotate_left(w, rot)           _Generic((w),                                                                       \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_rotate_left),             \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_rotate_left),            \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_rotate_left),              \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_rotate_left),             \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_rotate_left)             \
                                             )(w, rot)

   #define bit_rotate_right(w, rot)          _Generic((w),                                                                       \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_rotate_right),            \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_rotate_right),           \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_rotate_right),             \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_rotate_right),            \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_rotate_right)            \
                                             )(w, rot)

#endif /* C2Y PROTOS */


HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_LIB_PROTOS, HUZLIB_BIT_INTERNAL_DECLARE_PROTO)

#define bit_rotate_left_part(w, rot, cnt)    _Generic((w),                                                                       \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_rotate_left_part),        \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_rotate_left_part),       \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_rotate_left_part),         \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_rotate_left_part),        \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_rotate_left_part)        \
                                             )(w, rot, cnt)

#define bit_rotate_right_part(w, rot, cnt)   _Generic((w),                                                                       \
                                                uchar:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uchar,  bit_rotate_right_part),       \
                                                ushort:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ushort,  bit_rotate_right_part),      \
                                                uint:    HUZLIB_BIT_INTERNAL_GENERIC_PROTO(uint,  bit_rotate_right_part),        \
                                                ulong:   HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ulong,  bit_rotate_right_part),       \
                                                ullong:  HUZLIB_BIT_INTERNAL_GENERIC_PROTO(ullong,  bit_rotate_right_part)       \
                                             )(w, rot, cnt)



#define BIT_SWAP(cnt, off, a, b) do {                                                           \
   /* assume no sideeffects and cnt + off <= bits(a) */                                         \
   typecheck(typeof(a), b);                                                                     \
   typeof(b) __huzuq(__tmp) = (a);                                                              \
   typeof(a) __huzuq(__mask) = ((((typeof(a))1) << (cnt)) - 1) << (off);                        \
   (a) = ((a) & ~__huzuq(__mask)) | ((b) & __huzuq(__mask));                                    \
   (b) = ((b) & ~__huzuq(__mask)) | (__huzuq(__tmp) & __huzuq(__mask));                         \
} while (0)

#define BIT_SWAP_LS(cnt, a, b) BIT_SWAP(cnt, 0, a, b)
#define BIT_SWAP_MS(cnt, a, b) BIT_SWAP(cnt, HUZLIB_BIT_INTERNAL_TYPE_WIDTH(a) - (cnt), a, b)

#define bit_swap(cnt, off, a, b) do {                                                           \
   typeof(cnt) *__huzuq(__cnt) = &(cnt);                                                        \
   typeof(off) *__huzuq(__off) = &(off);                                                        \
   typeof(a) *__huzuq(__a) = &(a);                                                              \
   typeof(b) *__huzuq(__b) = &(b);                                                              \
   assert(*__huzuq(__cnt) + *__huzuq(__off) <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(*__huzuq(__a)));  \
   BIT_SWAP(*__huzuq(__cnt), *__huzuq(__off), *__huzuq(__a), *__huzuq(__b));                    \
} while(0)

#define bit_swap_ls(cnt, a, b) do {                                                             \
   typeof(cnt) *__huzuq(__cnt) = &(cnt);                                                        \
   typeof(a) *__huzuq(__a) = &(a);                                                              \
   typeof(b) *__huzuq(__b) = &(b);                                                              \
   assert(*__huzuq(__cnt) <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(*__huzuq(__a)));                    \
   BIT_SWAP_LS(*__huzuq(__cnt), *__huzuq(__a), *__huzuq(__b));                                  \
} while(0)

#define bit_swap_ms(cnt, a, b) do {                                                             \
   typeof(cnt) *__huzuq(__cnt) = &(cnt);                                                        \
   typeof(a) *__huzuq(__a) = &(a);                                                              \
   typeof(b) *__huzuq(__b) = &(b);                                                              \
   assert(*__huzuq(__cnt) <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(*__huzuq(__a)));                    \
   BIT_SWAP_MS(*__huzuq(__cnt), *__huzuq(__a), *__huzuq(__b));                                  \
} while(0)




#ifdef HUZLIB_BIT_IMPL


#include <stdlib.h>

static inline unsigned int __bit_operation_not_found(void)
{
   assert(0);
   abort();
   return 0;
}


#if (__STDC_VERSION__ <= 201710L)


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

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS_DEFAULT(type)          \
   inline type bit_trailing_zeros_##type(type w)                                    \
   {                                                                                \
      if (w == 0)                                                                   \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                               \
                                                                                    \
      if (sizeof(type) == 1)                                                        \
      {                                                                             \
         static const unsigned char table[8] = {                                    \
            0, 1, 2, 6, 3, 4, 5, 7                                                  \
         };                                                                         \
         return table[((w & -w) * 0x1D) >> 5];                                      \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 2)                                                        \
      {                                                                             \
         static const unsigned char table[16] = {                                   \
            0, 1, 2, 13, 3, 7, 14, 11,                                              \
            4, 9, 8, 12, 15, 10, 5, 6                                               \
         };                                                                         \
         return table[((w & -w) * 0x4AF) >> 12];                                    \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 4)                                                        \
      {                                                                             \
         static const unsigned char table[32] = {                                   \
            0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,               \
            31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9              \
         };                                                                         \
         return table[((w & -w) * 0x077CB531U) >> 27];                              \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 8)                                                        \
      {                                                                             \
         static const unsigned char table[64] = {                                   \
            0, 1, 2, 53, 3, 7, 54, 27, 4, 38, 41, 8, 34, 55, 48, 28,                \
            62, 5, 39, 46, 44, 42, 22, 9, 24, 35, 59, 56, 49, 18, 29, 11,           \
            63, 52, 6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,          \
            51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12          \
         };                                                                         \
         return table[((w & -w) * 0x03F79D71B4CB0A89ULL) >> 58];                    \
      }                                                                             \
                                                                                    \
      return (type)__bit_operation_not_found();                                     \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS_DEFAULT(type)           \
   inline type bit_leading_zeros_##type(type w)                                     \
   {                                                                                \
      if (w == 0)                                                                   \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                               \
                                                                                    \
      if (sizeof(type) == 1)                                                        \
      {                                                                             \
         static const unsigned char table[8] = {                                    \
            7, 6, 5, 5, 4, 4, 4, 4                                                  \
         };                                                                         \
                                                                                    \
         w |= w >> 1;                                                               \
         w |= w >> 2;                                                               \
         w |= w >> 4;                                                               \
                                                                                    \
         return table[(w * 0x1D) >> 5];                                             \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 2)                                                        \
      {                                                                             \
         static const unsigned char table[16] = {                                   \
            15, 14, 13, 12, 11, 10, 9, 8,                                           \
            7, 6, 5, 5, 4, 4, 4, 4                                                  \
         };                                                                         \
                                                                                    \
         w |= w >> 1;                                                               \
         w |= w >> 2;                                                               \
         w |= w >> 4;                                                               \
         w |= w >> 8;                                                               \
                                                                                    \
         return table[(w * 0x4AF) >> 12];                                           \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 4)                                                        \
      {                                                                             \
         static const unsigned char table[32] = {                                   \
            31, 22, 30, 21, 18, 10, 29, 2, 20, 17, 15, 13, 9, 6, 28, 1,             \
            23, 19, 11, 3, 16, 14, 7, 24, 12, 4, 8, 25, 5, 26, 27, 0                \
         };                                                                         \
                                                                                    \
         w |= w >> 1;                                                               \
         w |= w >> 2;                                                               \
         w |= w >> 4;                                                               \
         w |= w >> 8;                                                               \
         w |= w >> 16;                                                              \
                                                                                    \
         return table[(w * 0x07C4ACDDU) >> 27];                                     \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 8)                                                        \
      {                                                                             \
         static const unsigned char table[64] = {                                   \
            63, 52, 53, 51, 54, 49, 50, 48, 55, 45, 46, 43, 47, 40, 41, 39,         \
            56, 35, 36, 33, 37, 30, 31, 29, 38, 26, 27, 24, 28, 22, 23, 21,         \
            57, 44, 34, 32, 25, 20, 19, 18, 42, 17, 16, 15, 14, 13, 12, 11,         \
            58, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 62, 61, 60, 59                    \
         };                                                                         \
                                                                                    \
         w |= w >> 1;                                                               \
         w |= w >> 2;                                                               \
         w |= w >> 4;                                                               \
         w |= w >> 8;                                                               \
         w |= w >> 16;                                                              \
         w |= w >> 32;                                                              \
                                                                                    \
         return table[(w * 0x03F566ED27179461ULL) >> 58];                           \
      }                                                                             \
                                                                                    \
      return (type)__bit_operation_not_found();                                     \
   }

#elif defined(BIT_DEFAULT_FALLBACK_LOW_MEMORY)

   /*
    * use Binary Search
    *    time:  O(logn)
    *    space: O(1)
    */

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS_DEFAULT(type)          \
   inline type bit_trailing_zeros_##type(type w)                                    \
   {                                                                                \
      if (w == 0)                                                                   \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                               \
                                                                                    \
      if (sizeof(type) == 1)                                                        \
      {                                                                             \
         type n = 0;                                                                \
         if ((w & 0x0F) == 0) { n += 4; w >>= 4; }                                  \
         if ((w & 0x03) == 0) { n += 2; w >>= 2; }                                  \
         if ((w & 0x01) == 0) { n += 1; }                                           \
         return n;                                                                  \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 2)                                                        \
      {                                                                             \
         type n = 0;                                                                \
         if ((w & 0x00FF) == 0) { n += 8; w >>= 8; }                                \
         if ((w & 0x000F) == 0) { n += 4; w >>= 4; }                                \
         if ((w & 0x0003) == 0) { n += 2; w >>= 2; }                                \
         if ((w & 0x0001) == 0) { n += 1; }                                         \
         return n;                                                                  \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 4)                                                        \
      {                                                                             \
         type n = 0;                                                                \
         if ((w & 0x0000FFFF) == 0) { n += 16; w >>= 16; }                          \
         if ((w & 0x000000FF) == 0) { n += 8;  w >>= 8;  }                          \
         if ((w & 0x0000000F) == 0) { n += 4;  w >>= 4;  }                          \
         if ((w & 0x00000003) == 0) { n += 2;  w >>= 2;  }                          \
         if ((w & 0x00000001) == 0) { n += 1;  }                                    \
         return n;                                                                  \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 8)                                                        \
      {                                                                             \
         type n = 0;                                                                \
         if ((w & 0x00000000FFFFFFFFULL) == 0) { n += 32; w >>= 32; }               \
         if ((w & 0x000000000000FFFFULL) == 0) { n += 16; w >>= 16; }               \
         if ((w & 0x00000000000000FFULL) == 0) { n += 8;  w >>= 8;  }               \
         if ((w & 0x000000000000000FULL) == 0) { n += 4;  w >>= 4;  }               \
         if ((w & 0x0000000000000003ULL) == 0) { n += 2;  w >>= 2;  }               \
         if ((w & 0x0000000000000001ULL) == 0) { n += 1;  }                         \
         return n;                                                                  \
      }                                                                             \
                                                                                    \
      return (type)__bit_operation_not_found();                                     \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS_DEFAULT(type)           \
   inline type bit_leading_zeros_##type(type w)                                     \
   {                                                                                \
      if (w == 0)                                                                   \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                               \
                                                                                    \
      if (sizeof(type) == 1)                                                        \
      {                                                                             \
         type n = 0;                                                                \
         if (w <= 0x0F) { n += 4; w <<= 4; }                                        \
         if (w <= 0x3F) { n += 2; w <<= 2; }                                        \
         if (w <= 0x7F) { n += 1; }                                                 \
         return n;                                                                  \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 2)                                                        \
      {                                                                             \
         type n = 0;                                                                \
         if (w <= 0x00FF) { n += 8; w <<= 8; }                                      \
         if (w <= 0x0FFF) { n += 4; w <<= 4; }                                      \
         if (w <= 0x3FFF) { n += 2; w <<= 2; }                                      \
         if (w <= 0x7FFF) { n += 1; }                                               \
         return n;                                                                  \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 4)                                                        \
      {                                                                             \
         type n = 0;                                                                \
         if (w <= 0x0000FFFF) { n += 16; w <<= 16; }                                \
         if (w <= 0x00FFFFFF) { n += 8;  w <<= 8;  }                                \
         if (w <= 0x0FFFFFFF) { n += 4;  w <<= 4;  }                                \
         if (w <= 0x3FFFFFFF) { n += 2;  w <<= 2;  }                                \
         if (w <= 0x7FFFFFFF) { n += 1;  }                                          \
         return n;                                                                  \
      }                                                                             \
                                                                                    \
      if (sizeof(type) == 8)                                                        \
      {                                                                             \
         type n = 0;                                                                \
         if (w <= 0x00000000FFFFFFFFULL) { n += 32; w <<= 32; }                     \
         if (w <= 0x0000FFFFFFFFFFFFULL) { n += 16; w <<= 16; }                     \
         if (w <= 0x00FFFFFFFFFFFFFFULL) { n += 8;  w <<= 8;  }                     \
         if (w <= 0x0FFFFFFFFFFFFFFFULL) { n += 4;  w <<= 4;  }                     \
         if (w <= 0x3FFFFFFFFFFFFFFFULL) { n += 2;  w <<= 2;  }                     \
         if (w <= 0x7FFFFFFFFFFFFFFFULL) { n += 1;  }                               \
         return n;                                                                  \
      }                                                                             \
                                                                                    \
      return (type)__bit_operation_not_found();                                     \
   }

#endif


#define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES_DEFAULT(type)                 \
inline type bit_count_ones_##type(type w)                                           \
{                                                                                   \
   if (w == 0)                                                                      \
      return 0;                                                                     \
                                                                                    \
   if (sizeof(type) == 1)                                                           \
   {                                                                                \
      static const unsigned char table[16] = {                                      \
         0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4                             \
      };                                                                            \
      return table[w & 0x0F] + table[(w >> 4) & 0x0F];                              \
   }                                                                                \
                                                                                    \
   if (sizeof(type) == 2)                                                           \
   {                                                                                \
      w = (w & 0x5555) + ((w >> 1) & 0x5555);                                       \
      w = (w & 0x3333) + ((w >> 2) & 0x3333);                                       \
      w = (w & 0x0F0F) + ((w >> 4) & 0x0F0F);                                       \
      w = (w & 0x00FF) + ((w >> 8) & 0x00FF);                                       \
      return w;                                                                     \
   }                                                                                \
                                                                                    \
   if (sizeof(type) == 4)                                                           \
   {                                                                                \
      w = w - ((w >> 1) & 0x55555555);                                              \
      w = (w & 0x33333333) + ((w >> 2) & 0x33333333);                               \
      w = (w + (w >> 4)) & 0x0F0F0F0F;                                              \
      w = w + (w >> 8);                                                             \
      w = w + (w >> 16);                                                            \
      return w & 0x3F;                                                              \
   }                                                                                \
                                                                                    \
   if (sizeof(type) == 8)                                                           \
   {                                                                                \
      w = w - ((w >> 1) & 0x5555555555555555ULL);                                   \
      w = (w & 0x3333333333333333ULL) + ((w >> 2) & 0x3333333333333333ULL);         \
      w = (w + (w >> 4)) & 0x0F0F0F0F0F0F0F0FULL;                                   \
      w = w + (w >> 8);                                                             \
      w = w + (w >> 16);                                                            \
      w = w + (w >> 32);                                                            \
      return (type)(w & 0x7F);                                                      \
   }                                                                                \
                                                                                    \
   return (type)__bit_operation_not_found();                                        \
}

#define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE_CTZ_FALLBACK(type)    \
inline type bit_first_trailing_one_##type(type w)                                   \
{                                                                                   \
   if (w == 0)                                                                      \
      return 0;                                                                     \
   else                                                                             \
      return 1 + bit_trailing_zeros_##type(w);                                      \
}

#define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE_CLZ_FALLBACK(type)     \
inline type bit_first_leading_one_##type(type w)                                    \
{                                                                                   \
   if (w == 0)                                                                      \
      return 0;                                                                     \
   else                                                                             \
      return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) - (1 + bit_leading_zeros_##type(w)); \
}


/*
 * CUDA:
 *    - __clz[,ll]
 *    - __popc[,ll]
 *    - __ffs[,ll]
 */
#if defined(__NVCC__)
   #include <cuda_runtime.h>

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS(type, ...)       \
   inline type bit_trailing_zeros_##type(type w)                              \
   {                                                                          \
      if (w == 0)                                                             \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                         \
                                                                              \
      switch (sizeof(type))                                                   \
      {                                                                       \
         case 1:                                                              \
         case 2:                                                              \
         case 4:  return __ffs((uint32_t)w) - 1;                              \
         case 8:  return __ffsll((uint64_t)w) - 1;                            \
      }                                                                       \
                                                                              \
      return (type)__bit_operation_not_found();                               \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS(type, ...)        \
   inline type bit_leading_zeros_##type(type w)                               \
   {                                                                          \
      if (w == 0)                                                             \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                         \
                                                                              \
      switch (sizeof(type))                                                   \
      {                                                                       \
         case 1:  return __clz((uint32_t)w) - 24;                             \
         case 2:  return __clz((uint32_t)w) - 16;                             \
         case 4:  return __clz((uint32_t)w);                                  \
         case 8:  return __clzll((uint64_t)w);                                \
      }                                                                       \
                                                                              \
      return (type)__bit_operation_not_found();                               \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES(type, ...)           \
   inline type bit_count_ones_##type(type w)                                  \
   {                                                                          \
      switch (sizeof(type))                                                   \
      {                                                                       \
         case 1:                                                              \
         case 2:                                                              \
         case 4:  return __popc((uint32_t)w);                                 \
         case 8:  return __popcll((uint64_t)w);                               \
      }                                                                       \
      return (type)__bit_operation_not_found();                               \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE(type, ...)   \
   inline type bit_first_trailing_one_##type(type w)                          \
   {                                                                          \
      switch (sizeof(type))                                                   \
      {                                                                       \
         case 1:                                                              \
         case 2:                                                              \
         case 4:  return __ffs((uint32_t)w);                                  \
         case 8:  return __ffsll((uint64_t)w);                                \
      }                                                                       \
      return (type)__bit_operation_not_found();                               \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE(type, ...)  HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE_CLZ_FALLBACK(type)


/*
 * GCC/Clang/Intel/ARMCLANG/OpenXL:
 *    - __builtin_ctz[,l,ll]
 *    - __builtin_clz[,l,ll]
 *    - __builtin_popcount[,l,ll]
 *    - __builtin_ffs[,l,ll]
 */
#elif defined(__INTEL_LLVM_COMPILER) || (defined(__INTEL_COMPILER) && defined(__GNUC__)) || (defined(__ARMCOMPILER_VERSION) && __ARMCOMPILER_VERSION >= 600000) || (defined(__ibmxl__) && __ibmxl__ >= 0x10010000) || (defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))) || defined(__clang__)

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS(type, ...)                                     \
   inline type bit_trailing_zeros_##type(type w)                                                            \
   {                                                                                                        \
      if (w == 0)                                                                                           \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                                                       \
                                                                                                            \
      if (sizeof(type) <= sizeof(unsigned int))       return __builtin_ctz((unsigned int)w);                \
      if (sizeof(type) == sizeof(unsigned long))      return __builtin_ctzl((unsigned long)w);              \
      if (sizeof(type) == sizeof(unsigned long long)) return __builtin_ctzll((unsigned long long)w);        \
      else                                            return (type)__bit_operation_not_found();             \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS(type, ...)                                      \
   inline type bit_leading_zeros_##type(type w)                                                             \
   {                                                                                                        \
      if (w == 0)                                                                                           \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                                                       \
                                                                                                            \
      if (sizeof(type) == sizeof(unsigned char))      return __builtin_clz((unsigned int)w) - 24;           \
      if (sizeof(type) == sizeof(unsigned short))     return __builtin_clz((unsigned int)w) - 16;           \
      if (sizeof(type) == sizeof(unsigned int))       return __builtin_clz((unsigned int)w);                \
      if (sizeof(type) == sizeof(unsigned long))      return __builtin_clzl((unsigned long)w);              \
      if (sizeof(type) == sizeof(unsigned long long)) return __builtin_clzll((unsigned long long)w);        \
      else                                            return (type)__bit_operation_not_found();             \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES(type, ...)                                         \
   inline type bit_count_ones_##type(type w)                                                                \
   {                                                                                                        \
      if (sizeof(type) <= sizeof(unsigned int))       return __builtin_popcount((unsigned int)w);           \
      if (sizeof(type) == sizeof(unsigned long))      return __builtin_popcountl((unsigned long)w);         \
      if (sizeof(type) == sizeof(unsigned long long)) return __builtin_popcountll((unsigned long long)w);   \
      else                                            return (type)__bit_operation_not_found();             \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE(type, ...)                                 \
   inline type bit_first_trailing_one_##type(type w)                                                        \
   {                                                                                                        \
      if (sizeof(type) <= sizeof(unsigned int))       return __builtin_ffs((unsigned int)w);                \
      if (sizeof(type) == sizeof(unsigned long))      return __builtin_ffsl((unsigned long)w);              \
      if (sizeof(type) == sizeof(unsigned long long)) return __builtin_ffsll((unsigned long long)w);        \
      else                                            return (type)__bit_operation_not_found();             \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE(type, ...)    HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE_CLZ_FALLBACK(type)


/*
 * IBM Classic XL:
 *    - __cnttz[4,8]
 *    - __cntlz[4,8]
 *    - __popcnt[4,8]
 */
#elif defined(__xlC__) || defined(__ibmxl__)
   #include <builtins.h>

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS(type, ...) \
   inline type bit_trailing_zeros_##type(type w)                        \
   {                                                                    \
      if (w == 0)                                                       \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                   \
                                                                        \
      switch (sizeof(type))                                             \
      {                                                                 \
         case 1:                                                        \
         case 2:                                                        \
         case 4:  return __cnttz4((uint32_t)w);                         \
         case 8:  return __cnttz8((uint64_t)w);                         \
      }                                                                 \
                                                                        \
      return (type)__bit_operation_not_found();                         \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS(type, ...)  \
   inline type bit_leading_zeros_##type(type w)                         \
   {                                                                    \
      if (w == 0)                                                       \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                   \
                                                                        \
      switch (sizeof(type))                                             \
      {                                                                 \
         case 1:  return __cntlz4((uint32_t)w) - 24;                    \
         case 2:  return __cntlz4((uint32_t)w) - 16;                    \
         case 4:  return __cntlz4((uint32_t)w);                         \
         case 8:  return __cntlz8((uint64_t)w);                         \
      }                                                                 \
                                                                        \
      return (type)__bit_operation_not_found();                         \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES(type, ...)     \
   inline type bit_count_ones_##type(type w)                            \
   {                                                                    \
      switch (sizeof(type))                                             \
      {                                                                 \
         case 1:                                                        \
         case 2:                                                        \
         case 4:  return __popcnt4((uint32_t)w);                        \
         case 8:  return __popcnt8((uint64_t)w);                        \
      }                                                                 \
      return (type)__bit_operation_not_found();                         \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE(type, ...)   HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE_CTZ_FALLBACK(type)
   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE(type, ...)    HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE_CLZ_FALLBACK(type)


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

      #define HUZLIB_BIT_IMPL_CTZ_16(ret, w)       do { *(ret) = __tzcnt16((uint16_t)w);  } while(0)
      #define HUZLIB_BIT_IMPL_CTZ_32(ret, w)       do { *(ret) = __tzcnt((uint32_t)w);    } while(0)
      #define HUZLIB_BIT_IMPL_CLZ_16(ret, w)       do { *(ret) = __lzcnt16((uint16_t)w);  } while(0)
      #define HUZLIB_BIT_IMPL_CLZ_32(ret, w)       do { *(ret) = __lzcnt((uint32_t)w);    } while(0)
      #define HUZLIB_BIT_IMPL_POPC_16(ret, w)      do { *(ret) = __popcnt16((uint16_t)w); } while(0)
      #define HUZLIB_BIT_IMPL_POPC_32(ret, w)      do { *(ret) = __popcnt((uint32_t)w);   } while(0)

      #if defined(_M_X64)
         #define HUZLIB_BIT_IMPL_CTZ_64(ret, w)    do { *(ret) = __tzcnt64((uint64_t)w);  } while(0)
         #define HUZLIB_BIT_IMPL_CLZ_64(ret, w)    do { *(ret) = __lzcnt64((uint64_t)w);  } while(0)
         #define HUZLIB_BIT_IMPL_POPC_64(ret, w)   do { *(ret) = __popcnt64((uint64_t)w); } while(0)

      #else
         #define HUZLIB_BIT_IMPL_CTZ_64(ret, w)    do {        \
            uint32_t __huzuq(_low) = (uint32_t)(w);            \
            if (__huzuq(_low) != 0)                            \
               *(ret) = __tzcnt(__huzuq(_low));                \
            else                                               \
               *(ret) = 32 + __tzcnt((uint32_t)((w) >> 32));   \
         } while(0)

         #define HUZLIB_BIT_IMPL_CLZ_64(ret, w)    do {        \
            uint32_t __huzuq(_high) = (uint32_t)((w) >> 32);   \
            if (__huzuq(_high) != 0)                           \
               *(ret) = __lzcnt(__huzuq(_high));               \
            else                                               \
               *(ret) = 32 + __lzcnt((uint32_t)(w));           \
         } while(0)

         #define HUZLIB_BIT_IMPL_POPC_64(ret, w)   do {        \
            *(ret) = __popcnt((uint32_t)(w))                   \
               + __popcnt((uint32_t)((w) >> 32));              \
         } while(0)

      #endif
      #define HUZLIB_BIT_INTERNAL_MVSC_HAS_POPC 1
 
   #else

      #define HUZLIB_BIT_IMPL_CTZ_16(ret, w)       do { _BitScanForward(ret, (uint16_t)w);               } while(0)
      #define HUZLIB_BIT_IMPL_CTZ_32(ret, w)       do { _BitScanForward(ret, (uint32_t)w);               } while(0)
      #define HUZLIB_BIT_IMPL_CLZ_16(ret, w)       do { _BitScanReverse(ret, (uint16_t)w); *(ret) -= 16; } while(0)
      #define HUZLIB_BIT_IMPL_CLZ_32(ret, w)       do { _BitScanReverse(ret, (uint32_t)w);               } while(0)

      #if defined(_M_ARM) || defined(_M_ARM64)
         #define HUZLIB_BIT_IMPL_POPC_16(ret, w)   do { *(ret) = _CountOneBits((uint16_t)w);       } while(0)
         #define HUZLIB_BIT_IMPL_POPC_32(ret, w)   do { *(ret) = _CountOneBits((uint32_t)w);       } while(0)
         #define HUZLIB_BIT_INTERNAL_MVSC_HAS_POPC 1
      #endif

      #if defined(_M_X64) || defined(_M_ARM64)
         #define HUZLIB_BIT_IMPL_CTZ_64(ret, w)    do { _BitScanForward64(ret, (uint64_t)w);       } while(0)
         #define HUZLIB_BIT_IMPL_CLZ_64(ret, w)    do { _BitScanReverse64(ret, (uint64_t)w);       } while(0)

         #ifdef _M_ARM64
            #define HUZLIB_BIT_IMPL_POPC_64(ret, w) do { *(ret) = _CountOneBits64((uint64_t)w);    } while(0)
         #endif

      #else
         #define HUZLIB_BIT_IMPL_CTZ_64(ret, w)    do {        \
            uint32_t __huzuq(_low) = (uint32_t)(w);            \
            if (__huzuq(_low) != 0)                            \
               _BitScanForward(ret, __huzuq(_low));            \
            else {                                             \
               _BitScanForward(ret, (uint32_t)((w) >> 32));    \
               *(ret) += 32;                                   \
            }                                                  \
         } while(0)

         #define HUZLIB_BIT_IMPL_CLZ_64(ret, w)    do {        \
            uint32_t __huzuq(_high) = (uint32_t)((w) >> 32);   \
            if (__huzuq(_high) != 0)                           \
               _BitScanReverse(ret, __huzuq(_high));           \
            else {                                             \
               _BitScanReverse(ret, (uint32_t)(w));            \
               *(ret) += 32;                                   \
            }                                                  \
         } while(0)

         #ifdef _M_ARM
            #define HUZLIB_BIT_IMPL_POPC_64(ret, w) do {       \
               *(ret) = _CountOneBits((uint32_t)(w))           \
                  + _CountOneBits((uint32_t)((w) >> 32));      \
            } while(0)
         #endif

      #endif

   #endif /* fast intrinsics (only modern x86) */

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS(type, ...) \
   inline type bit_trailing_zeros_##type(type w)                        \
   {                                                                    \
      type ret;                                                         \
      if (w == 0)                                                       \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                   \
                                                                        \
      switch (sizeof(type))                                             \
      {                                                                 \
         case 1:                                                        \
         case 2:     HUZLIB_BIT_IMPL_CTZ_16(&ret, w);    break;         \
         case 4:     HUZLIB_BIT_IMPL_CTZ_32(&ret, w);    break;         \
         case 8:     HUZLIB_BIT_IMPL_CTZ_64(&ret, w);    break;         \
         default:    assert(0);                          break;         \
      }                                                                 \
      return ret;                                                       \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS(type, ...)        \
   inline type bit_leading_zeros_##type(type w)                               \
   {                                                                          \
      type ret;                                                               \
      if (w == 0)                                                             \
         return HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);                         \
                                                                              \
      switch (sizeof(type))                                                   \
      {                                                                       \
         case 1:     HUZLIB_BIT_IMPL_CLZ_16(&ret, w);    ret -= 8;   break;   \
         case 2:     HUZLIB_BIT_IMPL_CLZ_16(&ret, w);                break;   \
         case 4:     HUZLIB_BIT_IMPL_CLZ_32(&ret, w);                break;   \
         case 8:     HUZLIB_BIT_IMPL_CLZ_64(&ret, w);                break;   \
         default:    assert(0);                                      break;   \
      }                                                                       \
      return ret;                                                             \
   }

   #ifdef HUZLIB_BIT_INTERNAL_MVSC_HAS_POPC
      #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES(type, ...)  \
      inline type bit_count_ones_##type(type w)                         \
      {                                                                 \
         type ret;                                                      \
         switch (sizeof(type))                                          \
         {                                                              \
            case 1:                                                     \
            case 2:  HUZLIB_BIT_IMPL_POPC_16(&ret, w);   break;         \
            case 4:  HUZLIB_BIT_IMPL_POPC_32(&ret, w);   break;         \
            case 8:  HUZLIB_BIT_IMPL_POPC_64(&ret, w);   break;         \
            default: assert(0);                          break;         \
         }                                                              \
         return ret;                                                    \
      }
   #else
      #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES(type, ...)        HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES_DEFAULT(type)
   #endif
   #undef HUZLIB_BIT_INTERNAL_MVSC_HAS_POPC

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE(type, ...)   HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE_CTZ_FALLBACK(type)
   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE(type, ...)    HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE_CLZ_FALLBACK(type)

   #undef HUZLIB_BIT_IMPL_POPC_16
   #undef HUZLIB_BIT_IMPL_POPC_32
   #undef HUZLIB_BIT_IMPL_POPC_64
   #undef HUZLIB_BIT_IMPL_CLZ_16
   #undef HUZLIB_BIT_IMPL_CLZ_32
   #undef HUZLIB_BIT_IMPL_CLZ_64
   #undef HUZLIB_BIT_IMPL_CTZ_16
   #undef HUZLIB_BIT_IMPL_CTZ_32
   #undef HUZLIB_BIT_IMPL_CTZ_64


/*
 * SunCC:
 *    - __builtin_popcount[,l,ll]
 */
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS(type, ...)       HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS_DEFAULT(type)
   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS(type, ...)        HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS_DEFAULT(type)
   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE(type, ...)   HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE_CTZ_FALLBACK(type)
   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE(type, ...)    HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE_CLZ_FALLBACK(type)

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES(type, ...)                                         \
   inline type bit_count_ones_##type(type w)                                                                \
   {                                                                                                        \
      if (sizeof(type) <= sizeof(unsigned int))       return __builtin_popcount((unsigned int)w);           \
      if (sizeof(type) == sizeof(unsigned long))      return __builtin_popcountl((unsigned long)w);         \
      if (sizeof(type) == sizeof(unsigned long long)) return __builtin_popcountll((unsigned long long)w);   \
      else                                            return (type)__bit_operation_not_found();             \
   }


/*
 * default:
 */
#else

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS(type, ...)       HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS_DEFAULT(type)
   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS(type, ...)        HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS_DEFAULT(type)
   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES(type, ...)           HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES_DEFAULT(type)
   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE(type, ...)   HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE_CTZ_FALLBACK(type)
   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE(type, ...)    HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE_CLZ_FALLBACK(type)

#endif


#define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_WIDTH(type, ...)                               \
inline type bit_width_##type(type w)                                                      \
{                                                                                         \
   return (w == 0)                                                                        \
      ? 0                                                                                 \
      : HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) - bit_leading_zeros_##type(w);                  \
}

#define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_CEIL(type, ...)                                \
inline type bit_ceil_##type(type w)                                                       \
{                                                                                         \
   return (w == 0)                                                                        \
      ? 1                                                                                 \
      :(1ull << (HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) - bit_leading_zeros_##type(w - 1)));   \
}

#define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FLOOR(type, ...)                               \
inline type bit_floor_##type(type w)                                                      \
{                                                                                         \
   return (w == 0)                                                                        \
      ? 0                                                                                 \
      : (1ull << (HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) - 1 - bit_leading_zeros_##type(w)));  \
}

HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS, _)
HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS, _)
HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES, _)
HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE, _)
HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE, _)
HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_WIDTH, _)
HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_CEIL, _)
HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FLOOR, _)

#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_BIT_FLOOR
#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_BIT_CEIL
#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_BIT_WIDTH
#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_LEADING_ONE
#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_FIRST_TRAILING_ONE
#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_COUNT_ONES
#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_LEADING_ZEROS
#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_TRAILING_ZEROS

#endif /* C23 PROTOS */


#if !((__STDC_VERSION__ > 202311L) || (defined(__GNUC__) && (__GNUC__ >= 15)) || ((defined(__clang__) || defined(__INTEL_LLVM_COMPILER) || defined(__ARMCOMPILER_VERSION) || defined(__ibmxl__)) && (__clang_major__ >= 23)))

/* ------- NEW ------------- */

#define HUZLIB_BIT_IMPL_ROTL_GENERIC(w, rot) (((w) << (rot)) | ((w) >> (HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) - (rot))))
#define HUZLIB_BIT_IMPL_ROTR_GENERIC(w, rot) (((w) >> (rot)) | ((w) << (HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) - (rot))))

#define HUZLIB_BIT_IMPL_ROTL_64_SPLIT(ret, w, rot) do {                          \
   uint32_t __huzuq(__lo), __huzuq(__hi);                                        \
   if ((rot) < HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) / 2)                            \
   {                                                                             \
      __huzuq(__lo) = bit_rotate_left((uint32_t)(w), rot);                       \
      __huzuq(__hi) = bit_rotate_left((uint32_t)((w) >> 32), rot);               \
      BIT_SWAP_LS(rot, __huzuq(__lo), __huzuq(__hi));                            \
   }                                                                             \
   else                                                                          \
   {                                                                             \
      uint32_t __huzuq(__rot) = HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) - (rot);       \
      __huzuq(__lo) = bit_rotate_right((uint32_t)(w), __huzuq(__rot));           \
      __huzuq(__hi) = bit_rotate_right((uint32_t)((w) >> 32), __huzuq(__rot));   \
      BIT_SWAP_MS(__huzuq(__rot), __huzuq(__lo), __huzuq(__hi));                 \
   }                                                                             \
   *(ret) = (typeof(w))__huzuq(__lo) | ((typeof(w))__huzuq(__hi) << 32);         \
} while(0)

#define HUZLIB_BIT_IMPL_ROTR_64_SPLIT(ret, w, rot) do {                          \
   uint32_t __huzuq(__lo), __huzuq(__hi);                                        \
   if ((rot) < HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) / 2)                            \
   {                                                                             \
      __huzuq(__lo) = bit_rotate_right((uint32_t)(w), rot);                      \
      __huzuq(__hi) = bit_rotate_right((uint32_t)((w) >> 32), rot);              \
      BIT_SWAP_LS(rot, __huzuq(__lo), __huzuq(__hi));                            \
   }                                                                             \
   else                                                                          \
   {                                                                             \
      uint32_t __huzuq(__rot) = HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) - (rot);       \
      __huzuq(__lo) = bit_rotate_left((uint32_t)(w), __huzuq(__rot));            \
      __huzuq(__hi) = bit_rotate_left((uint32_t)((w) >> 32), __huzuq(__rot));    \
      BIT_SWAP_MS(__huzuq(__rot), __huzuq(__lo), __huzuq(__hi));                 \
   }                                                                             \
   *(ret) = (typeof(w))__huzuq(__lo) | ((typeof(w))__huzuq(__hi) << 32);         \
} while(0)

#define HUZLIB_BIT_IMPL_ROTL_128_SPLIT(ret, w, rot) do {                         \
   uint64_t __huzuq(__lo), __huzuq(__hi);                                        \
   if ((rot) < HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) / 2)                            \
   {                                                                             \
      __huzuq(__lo) = bit_rotate_left((uint64_t)(w), rot);                       \
      __huzuq(__hi) = bit_rotate_left((uint64_t)((w) >> 64), rot);               \
      BIT_SWAP_LS(rot, __huzuq(__lo), __huzuq(__hi));                            \
   }                                                                             \
   else                                                                          \
   {                                                                             \
      uint64_t __huzuq(__rot) = HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) - (rot);       \
      __huzuq(__lo) = bit_rotate_right((uint64_t)(w), __huzuq(__rot));           \
      __huzuq(__hi) = bit_rotate_right((uint64_t)((w) >> 64), __huzuq(__rot));   \
      BIT_SWAP_MS(__huzuq(__rot), __huzuq(__lo), __huzuq(__hi));                 \
   }                                                                             \
   *(ret) = (typeof(w))__huzuq(__lo) | ((typeof(w))__huzuq(__hi) << 64);         \
} while(0)

#define HUZLIB_BIT_IMPL_ROTR_128_SPLIT(ret, w, rot) do {                         \
   uint64_t __huzuq(__lo), __huzuq(__hi);                                        \
   if ((rot) < HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) / 2)                            \
   {                                                                             \
      __huzuq(__lo) = bit_rotate_right((uint64_t)(w), rot);                      \
      __huzuq(__hi) = bit_rotate_right((uint64_t)((w) >> 64), rot);              \
      BIT_SWAP_LS(rot, __huzuq(__lo), __huzuq(__hi));                            \
   }                                                                             \
   else                                                                          \
   {                                                                             \
      uint64_t __huzuq(__rot) = HUZLIB_BIT_INTERNAL_TYPE_WIDTH(w) - (rot);       \
      __huzuq(__lo) = bit_rotate_left((uint64_t)(w), __huzuq(__rot));            \
      __huzuq(__hi) = bit_rotate_left((uint64_t)((w) >> 64), __huzuq(__rot));    \
      BIT_SWAP_MS(__huzuq(__rot), __huzuq(__lo), __huzuq(__hi));                 \
   }                                                                             \
   *(ret) = (typeof(w))__huzuq(__lo) | ((typeof(w))__huzuq(__hi) << 64);         \
} while(0)


#if defined(_MSC_VER) || defined(__POCC__) || defined(__INTEL_COMPILER)
   #include <intrin.h>

   #if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__amd64__)

      #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_LEFT(type, ...)          \
      inline type bit_rotate_left_##type(type w, uint8_t rot)                    \
      {                                                                          \
         assert(rot <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type));                    \
         type ret;                                                               \
         switch(sizeof(type))                                                    \
         {                                                                       \
            case 1:  ret = _rotl8(w, rot);                           break;      \
            case 2:  ret = _rotl16(w, rot);                          break;      \
            case 4:  ret = _rotl(w, rot);                            break;      \
            case 8:  ret = _rotl64(w, rot);                          break;      \
            case 16: HUZLIB_BIT_IMPL_ROTL_128_SPLIT(&ret, w, rot);   break;      \
            default: ret = HUZLIB_BIT_IMPL_ROTL_GENERIC(w, rot);     break;      \
         }                                                                       \
         return ret;                                                             \
      }

      #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_RIGHT(type, ...)         \
      inline type bit_rotate_right_##type(type w, uint8_t rot)                   \
      {                                                                          \
         assert(rot <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type));                    \
         type ret;                                                               \
         switch(sizeof(type))                                                    \
         {                                                                       \
            case 1:  ret = _rotr8(w, rot);                           break;      \
            case 2:  ret = _rotr16(w, rot);                          break;      \
            case 4:  ret = _rotr(w, rot);                            break;      \
            case 8:  ret = _rotr64(w, rot);                          break;      \
            case 16: HUZLIB_BIT_IMPL_ROTR_128_SPLIT(&ret, w, rot);   break;      \
            default: ret = HUZLIB_BIT_IMPL_ROTR_GENERIC(w, rot);     break;      \
         }                                                                       \
         return ret;                                                             \
      }

   #else

      #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_LEFT(type, ...)          \
      inline type bit_rotate_left_##type(type w, uint8_t rot)                    \
      {                                                                          \
         assert(rot <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type));                    \
         type ret;                                                               \
         switch(sizeof(type))                                                    \
         {                                                                       \
            case 1:  ret = _rotl8(w, rot);                           break;      \
            case 2:  ret = _rotl16(w, rot);                          break;      \
            case 4:  ret = _rotl(w, rot);                            break;      \
            case 8:  HUZLIB_BIT_IMPL_ROTL_64_SPLIT(&ret, w, rot);    break;      \
            default: ret = HUZLIB_BIT_IMPL_ROTL_GENERIC(w, rot);     break;      \
         }                                                                       \
         return ret;                                                             \
      }

      #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_RIGHT(type, ...)         \
      inline type bit_rotate_right_##type(type w, uint8_t rot)                   \
      {                                                                          \
         assert(rot <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type));                    \
         type ret;                                                               \
         switch(sizeof(type))                                                    \
         {                                                                       \
            case 1:  ret = _rotr8(w, rot);                           break;      \
            case 2:  ret = _rotr16(w, rot);                          break;      \
            case 4:  ret = _rotr(w, rot);                            break;      \
            case 8:  HUZLIB_BIT_IMPL_ROTR_64_SPLIT(&ret, w, rot);    break;      \
            default: ret = HUZLIB_BIT_IMPL_ROTR_GENERIC(w, rot);     break;      \
         }                                                                       \
         return ret;                                                             \
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

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_LEFT(type, ...)             \
   inline type bit_rotate_left_##type(type w, uint8_t rot)                       \
   {                                                                             \
      assert(rot <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type));                       \
      type ret;                                                                  \
      switch(sizeof(type))                                                       \
      {                                                                          \
         case 4:  ret = __rotatel4(w, rot);                          break;      \
         case 8:  ret = __rotatel8(w, rot);                          break;      \
         case 16: HUZLIB_BIT_IMPL_ROTL_128_SPLIT(&ret, w, rot);      break;      \
         default: ret = HUZLIB_BIT_IMPL_ROTL_GENERIC(w, rot);        break;      \
      }                                                                          \
      return ret;                                                                \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_RIGHT(type, ...)            \
   inline type bit_rotate_right_##type(type w, uint8_t rot)                      \
   {                                                                             \
      assert(rot <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type));                       \
      type ret;                                                                  \
      switch(sizeof(type))                                                       \
      {                                                                          \
         case 4:  ret = __rotater4(w, rot);                          break;      \
         case 8:  ret = __rotater8(w, rot);                          break;      \
         case 16: HUZLIB_BIT_IMPL_ROTR_128_SPLIT(&ret, w, rot);      break;      \
         default: ret = HUZLIB_BIT_IMPL_ROTR_GENERIC(w, rot);        break;      \
      }                                                                          \
      return ret;                                                                \
   }

#else

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_LEFT(type, ...)             \
   inline type bit_rotate_left_##type(type w, uint8_t rot)                       \
   {                                                                             \
      assert(rot <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type));                       \
      return HUZLIB_BIT_IMPL_ROTL_GENERIC(w, rot);                               \
   }

   #define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_RIGHT(type, ...)            \
   inline type bit_rotate_right_##type(type w, uint8_t rot)                      \
   {                                                                             \
      assert(rot <= HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type));                       \
      return HUZLIB_BIT_IMPL_ROTR_GENERIC(w, rot);                               \
   }

#endif

HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_LEFT, _)
HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_RIGHT, _)

#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_RIGHT
#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_LEFT

#endif /* C2Y PROTOS */



#define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_LEFT_PART(type, ...)           \
inline type bit_rotate_left_part_##type(type w, uint8_t r, uint8_t m)            \
{                                                                                \
   assert(r < m && m < HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type));                    \
   if (r == 0 || m == 0)                                                         \
      return w;                                                                  \
                                                                                 \
   type mask = ((type)1 << m) - 1;                                               \
   type part = w & mask;                                                         \
   type high = w & ~mask;                                                        \
                                                                                 \
   type rotated = (type)((part << r) | (part >> (m - r))) & mask;                \
   return high | rotated;                                                        \
}

#define HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_RIGHT_PART(type, ...)          \
inline type bit_rotate_right_part_##type(type w, uint8_t r, uint8_t m)           \
{                                                                                \
   assert(r < m && m < HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type));                    \
   if (r == 0 || m == 0)                                                         \
      return w;                                                                  \
                                                                                 \
   type mask = ((type)1 << m) - 1;                                               \
   type part = w & mask;                                                         \
   type high = w & ~mask;                                                        \
                                                                                 \
   type rotated = (type)((part >> r) | (part << (m - r))) & mask;                \
   return high | rotated;                                                        \
}

HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_LEFT_PART, _)
HUZLIB_BIT_INTERNAL_TYPES(HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_RIGHT_PART, _)

#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_RIGHT_PART
#undef HUZLIB_BIT_INTERNAL_GENERATE_PROTO_ROTATE_LEFT_PART



#endif /* HUZLIB_BIT_IMPL */




#ifdef HUZLIB_BIT_TEST

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define UNITY_FRAMEWORK_SINGLE_HEADER_WRAPPER_IMPL
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

#define HUZLIB_BIT_INTERNAL_TEST_TYPES(x, ...)  \
   x(uint8_t, __VA_ARGS__)                      \
   x(uint16_t, __VA_ARGS__)                     \
   x(uint32_t, __VA_ARGS__)                     \
   x(uint64_t, __VA_ARGS__)                     \
   x(size_t, __VA_ARGS__)                       \
   x(uintptr_t, __VA_ARGS__)                    \
   x(unsigned, __VA_ARGS__)                     \
   x(uchar, __VA_ARGS__)                        \
   x(ushort, __VA_ARGS__)                       \
   x(uint, __VA_ARGS__)                         \
   x(ulong, __VA_ARGS__)                        \
   x(ullong, __VA_ARGS__)


#define HUZLIB_GENERATE_CTZ_TEST(type, ...)                 \
static void test_bit_ctz_##type(void)                       \
{                                                           \
   size_t WIDTH = HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);     \
   for (type i = 0; i < WIDTH; i++)                         \
   {                                                        \
      type w = (type)1 << i;                                \
      TEST_ASSERT_EQUAL(i, bit_ctz(w));                     \
   }                                                        \
   for (type i = 0; i < WIDTH - 1; i++)                     \
   {                                                        \
      type w = ((type)1 << i) | ((type)1 << (i + 1));       \
      TEST_ASSERT_EQUAL(i, bit_ctz(w));                     \
   }                                                        \
   TEST_ASSERT_EQUAL(WIDTH, bit_ctz((type)0));              \
}

#define HUZLIB_GENERATE_CLZ_TEST(type, ...)                 \
static void test_bit_clz_##type(void)                       \
{                                                           \
   size_t WIDTH = HUZLIB_BIT_INTERNAL_TYPE_WIDTH(type);     \
   for (type i = 0; i < WIDTH; i++)                         \
   {                                                        \
      type w = (type)1 << i;                                \
      TEST_ASSERT_EQUAL((WIDTH - i - 1), bit_clz(w));       \
   }                                                        \
   for (type i = 0; i < WIDTH; i++)                         \
   {                                                        \
      type w = ((type)1 << i) | ((type)1 << (i / 2));       \
      TEST_ASSERT_EQUAL((WIDTH - i - 1), bit_clz(w));       \
   }                                                        \
   TEST_ASSERT_EQUAL(WIDTH, bit_clz((type)0));              \
}


HUZLIB_BIT_INTERNAL_TEST_TYPES(HUZLIB_BIT_INTERNAL_ASSERT_UNSIGNED, _)
HUZLIB_BIT_INTERNAL_TEST_TYPES(HUZLIB_GENERATE_CTZ_TEST, _)
HUZLIB_BIT_INTERNAL_TEST_TYPES(HUZLIB_GENERATE_CLZ_TEST, _)


#define HUZLIB_BIT_RUN_TEST(type, func, ...)  RUN_TEST(func##_##type);

int main()
{
   UnityBegin("test/bit.h");

   HUZLIB_BIT_INTERNAL_TEST_TYPES(HUZLIB_BIT_RUN_TEST, test_bit_ctz)
   HUZLIB_BIT_INTERNAL_TEST_TYPES(HUZLIB_BIT_RUN_TEST, test_bit_clz)

   return UnityEnd();
}

#undef HUZLIB_BIT_RUN_TEST
#undef HUZLIB_BIT_INTERNAL_TEST_TYPES

#endif /* HUZLIB_BIT_TEST */




#undef HUZLIB_BIT_INTERNAL_DECLARE_PROTO
#undef HUZLIB_BIT_INTERNAL_ASSERT_UNSIGNED

#undef HUZLIB_BIT_INTERNAL_LIB_PROTOS
#undef HUZLIB_BIT_INTERNAL_C23_PROTOS
#undef HUZLIB_BIT_INTERNAL_C2Y_PROTOS
#undef HUZLIB_BIT_INTERNAL_TYPES


#endif /* HUZLIB_BIT_H */
