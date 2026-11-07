/**
 * defined macros
 * --------------
 *
 * Incoming:
 *    HUZ_$$_IMPL       - compiling as implementation (.c) file
 *    HUZ_$$_SHARED     - compiling as dynamicaly linked file
 *
 * Outgoing:
 *    HUZ_INTERNAL_API  - for internaly used functions (usualy starts with __huz_xxx)
 *    HUZ_INLINE_API    - for inline hinted functions (found in HUZ_$$_INLINE section)
 *    HUZ_IMPL_API      - for function not inline hinted (found in HUZ_$$_IMPL section)
 */

#if !(defined(__huz_inline__) && defined(__huz_noinline__) && defined(__huz_export__) && defined(__huz_import__))
   #error "Need huz inline export import hints"
#endif


// Clean previous definitions
#undef HUZ_INTERNAL_API
#undef HUZ_INLINE_API
#undef HUZ_IMPL_API

#ifndef NDEBUG

   #define HUZ_INTERNAL_API      static
   #define HUZ_INLINE_API        static __huz_noinline__
   #define HUZ_IMPL_API          __huz_noinline__

#else

   #ifdef HUZ_$$_SHARED

      #define HUZ_INTERNAL_API   static __huz_inline__

      #ifndef HUZ_$$_IMPL
         #define HUZ_INLINE_API  __huz_export__ __huz_noinline__
         #define HUZ_IMPL_API    __huz_export__ __huz_noinline__
      #else
         #define HUZ_INLINE_API  __huz_import__ __huz_inline__
         #define HUZ_IMPL_API    __huz_import__ __huz_noinline__
      #endif

   #else

      #define HUZ_INTERNAL_API   static __huz_inline__
      #define HUZ_INLINE_API     static __huz_inline__
      #define HUZ_IMPL_API

   #endif /* HUZ_SHARED */


#endif /* NDEBUG */
