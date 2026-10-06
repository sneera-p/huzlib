#undef HUZ_INTERNAL_API
#undef HUZ_INLINE_API
#undef HUZ_IMPL_API

#ifndef NDEBUG

   #define HUZ_INTERNAL_API      static
   #define HUZ_INLINE_API        static __huzlib_noinline__
   #define HUZ_IMPL_API          __huzlib_noinline__

#else

   #ifdef HUZ_SHARED

      #define HUZ_INTERNAL_API   static __huzlib_inline__

      #ifndef HUZ_IMPL
         #define HUZ_INLINE_API  static __huzlib_inline__
      #else
         #define HUZ_INLINE_API  __huzlib_export__ __huzlib_noinline__
      #endif

      #define HUZ_IMPL_API       __huzlib_export__ __huzlib_noinline__

   #else

      #define HUZ_INTERNAL_API   static __huzlib_inline__
      #define HUZ_INLINE_API     static __huzlib_inline__
      #define HUZ_IMPL_API

   #endif /* HUZ_SHARED */


#endif /* NDEBUG */

#undef HUZ_IMPL
#undef HUZ_SHARED
