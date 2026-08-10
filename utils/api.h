#undef HUZLIB_INTERNAL_API
#undef HUZLIB_INLINE_API
#undef HUZLIB_IMPL_API

#ifndef NDEBUG

   #define HUZLIB_INTERNAL_API      static
   #define HUZLIB_INLINE_API        static __huzlib_noinline__
   #define HUZLIB_IMPL_API          __huzlib_noinline__

#else

   #ifdef HUZLIB_SHARED

      #define HUZLIB_INTERNAL_API   static __huzlib_inline__

      #ifndef HUZLIB_IMPL
         #define HUZLIB_INLINE_API  static __huzlib_inline__
      #else
         #define HUZLIB_INLINE_API  __huzlib_export__ __huzlib_noinline__
      #endif

      #define HUZLIB_IMPL_API       __huzlib_export__ __huzlib_noinline__

   #else

      #define HUZLIB_INTERNAL_API   static __huzlib_inline__
      #define HUZLIB_INLINE_API     static __huzlib_inline__
      #define HUZLIB_IMPL_API

   #endif /* HUZLIB_SHARED */


#endif /* NDEBUG */

#undef HUZLIB_IMPL
#undef HUZLIB_SHARED
