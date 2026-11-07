<div align="center">

<div align="center">
```
 ██╗  ██╗██╗   ██╗███████╗██╗     ██╗██████╗
 ██║  ██║██║   ██║╚══███╔╝██║     ██║██╔══██╗
 ███████║██║   ██║  ███╔╝ ██║     ██║██████╔╝
 ██╔══██║██║   ██║ ███╔╝  ██║     ██║██╔══██╗
 ██║  ██║╚██████╔╝███████╗███████╗██║██████╔╝
 ╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚══════╝╚═╝╚═════╝
```
</div>

**Generic data structures and algorithms for C — no bloat, no templates, performance oriented.**

[![C11](https://img.shields.io/badge/C-C11-blue?style=flat-square)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Single Header](https://img.shields.io/badge/style-single--header-orange?style=flat-square)](#)
[![Zero Dependencies](https://img.shields.io/badge/deps-zero-red?style=flat-square)](#)

</div>

---

## The Problem

You write a stack for `int`. Then for `float`. Then for `struct Point`.

They look almost identical. You copy-paste. You rename. You ship three near-duplicate chunks of code that do the exact same thing.

C++ would fix this with templates — but templates have a cost. Every type gets its own private copy of the compiled functions baked into your binary. It works. But it bloats.

**There has to be a better way.**

---

## The Solution

`huzlib` uses some C tricks to provide you with a clean, generic way to use a stack *without* sacrificing on performance (maybe a little more compile time)

---

## What's Inside

```
huzlib/
├── stack.h          — dynamic generic stack
├── static-stack.h   — fixed-size generic stack (can use any memory allocator - malloc extension provided)
├── list.h           — generic linked list
├── avltree.h        — self-balancing binary tree
├── bit.h            — bit manipulation utilities
│
├── utils/
│   ├── bst.h        — embedded BST node (the kernel trick)
│   ├── assert.h     — compile-time and runtime assertions
│   ├── hints.h      — branch prediction and compiler hints
│   ├── types.h      — portable type aliases
│   └── xmacro.h     — X-macro helpers
│
└── docs/            — documentation per header
```

Every header follows the same internal layout — once you've read one, you've read them all:


```c
#ifndef HUZ_$$_H
#define HUZ_$$_H

   #ifndef HUZ_$$_INCLUDES
   #define HUZ_$$_INCLUDES
      /*
       * internal dependencies (utils/)
       * ------------------------------
       * To stay single-header, every internal dependency from "utils/" 
       * (assert.h, hints.h, etc.) is hand-inlined directly here. 
       *
       * The only external inclusions are freestanding libc headers
       * (<stddef.h>, <stdint.h>, <stdbool.h>).
       *
       * Every macro defined here is #ifndef guarded so it never
       * clobbers a definition the caller already has.
       */
   #endif /* HUZ_$$_INCLUDES */

   /*
    * header section
    * --------------
    * All public declarations live here, such as
    * type declarations, macro helpers etc.
    */

   #ifndef HUZ_$$_INLINE
   #define HUZ_$$_INLINE
      /*
       * inline section
       * --------------
       * Small, high-frequency functions (e.g. empty checks, size lookups).
       * Uses a visibility macro (HUZ_$$_INLINE_API) to flip behavior:
       * - Standard App / Static build  -> drops as 'static inline'
       * - Shared Library generation    -> compiles into exported symbols
       */
   #endif /* HUZ_$$_INLINE */

   /*
    * interface section
    * -----------------
    * function declarations (for heavy, out-of-line implementation functions)
    */

   #ifdef HUZ_$$_IMPL
      /*
       * implementation section
       * ----------------------
       * Definitions for every heavy, non-inlined function (e.g. memory scrubbing).
       * Only compiled when HUZ_$$_IMPL is defined — once,
       * in exactly one translation unit.
       */
   #endif /* HUZ_$$_IMPL */

   /*
    * translation section
    * -------------------
    * macro translation layer  (the generic call wrappers that route
    * to either the inline logic or the heavy implementation functions)
    */

   #ifdef HUZ_$$_TEST
      #include "pcg_basic.h"
      #include "unity.h"
      /*
       * testing section
       * ---------------
       * Self-contained unit tests using UnityTestFramework.
       * PCG-BASIC drives the random inputs for stress tests.
       */
   #endif /* HUZ_$$_TEST */

#endif /* HUZ_$$_H */
```

`$$` is a placeholder for the header name in all caps — `STATIC_STACK`, `AVLTREE`, `LIST`, etc.

---

## How to Use It

This is a **single-header library**. No build system. No linking. Just `#include`.

3 ways to get the implementation into your project:

---

### Method 1 — Definition Macro *(the classic stb way)*

The macro to define is always derived from the filename:

```
HUZ_ + filename in CAPS (hyphens → underscores) + _IMPL

static-stack.h  →  HUZ_STATIC_STACK_IMPL
avltree.h       →  HUZ_AVLTREE_IMPL
list.h          →  HUZ_LIST_IMPL
```

**In exactly one `.c` file**, define the implementation macro before including:

```c
// myapp.c  — do this ONCE across your whole project
#define HUZ_STACK_IMPL
#include "stack.h"
```

**Everywhere else**, just include normally:

```c
// other.c
#include "stack.h"
```

No CMake. No configure. No ritual.

---

### Method 2 — Direct Compilation *(header as translation unit)*

Don't want a dedicated `.c` file just to flip a macro? Tell the compiler to treat the header *itself* as a C source file using `-x c`, and pass the implementation flag on the command line:

```sh
# Compile the header directly into an object file
$ cc -DHUZ_STATIC_STACK_IMPL -x c -c static-stack.h -o static-stack.o
 
# Link it with your program as normal
$ cc main.c static-stack.o
```
 
`-x c` overrides the file extension — the compiler stops treating `.h` as a header-only include and compiles it as a full translation unit. The implementation lives in `static-stack.o`. Your `main.c` just includes the header without ever defining the macro.
 
Both methods produce the same binary. Pick whichever fits your workflow.
 
---

### Method 3 — Shared Library Compilation *(`.so` / `.dll`)*

If you want to compile `huzlib` as a standalone shared binary (allowing cross-language runtime engines like Python, Zig, or Rust FFI to tap into it), you can explicitly compile it into a shared object.

When doing this, defining `HUZ_$$_SHARED` tells the Inline Section to write exportable symbols directly into the binary layout so external dynamic linkers can locate them by name.

```sh
# Compile the header directly into a shared library (.so)
$ cc -DHUZ_STATIC_STACK_IMPL -DHUZ_STATIC_STACK_SHARED -x c -fPIC -shared static-stack.h -o libhuzstaticstack.so

# Link a C application against your compiled .so
# (Your C app gets inline performance for fast paths, but binds to the .so for slow paths)
$ cc main.c -L. -lhuzstaticstack -o app
```

---

### Running the Tests
 
Every header ships with its own self-contained test suite. To run it, add the `_TEST` flag alongside `_IMPL` — same naming rule applies:
 
```
HUZ_ + filename in CAPS + _TEST
 
static-stack.h  →  HUZ_STATIC_STACK_TEST
avltree.h       →  HUZ_AVLTREE_TEST
```
 
```sh
$ cc -DHUZ_STATIC_STACK_IMPL -DHUZ_STATIC_STACK_TEST -x c static-stack.h lib/unity.c lib/pcg_basic.c -o test
$ ./test
```
 
`unity` is the test framework. `pcg_basic` is a random number generator used to stress-test the data structures. Both live in `lib/` and only need to be compiled when running tests — they are never part of your actual binary.
 
---

## Quick Example — A Generic Stack

```c
#define HUZ_STATIC_STACK_IMPL
#include "static-stack.h"

int main(void)
{
    // create a stack of 64 floats
    struct STATIC_STACK(float, 64) s = STATIC_STACK_INIT(s);

    static_stack_push(&s, 3.14f);
    static_stack_push(&s, 2.71f);

    float top = static_stack_peek(&s);  // → returns 2.71f
    static_stack_pop(&s);

    return 0;
}
```

No type parameter at the call site. No code duplication in the binary. Clean.

---

## Compiler Support

Requires a C11 compiler with `_Generic` and `typeof` support.

| Compiler | Minimum Version |
|---|---|
| GCC | 4.9+ *(2014)* |
| Clang | 3.0+ *(2011)* |
| MSVC | VS 2019 16.8+ *(2020)* |
| Intel ICC | 15.0+ *(2014)* |
| Intel ICX/LLVM | 2021.1+ |
| ARMCLANG | 6.10+ *(2017)* |
| TinyCC | 0.9.27+ *(2017)* |
| IBM XL C | 13.1+ *(2013)* |
| Zig CC | 0.10.0+ *(2022)* |
| Pelles C | 8.0+ *(2015)* |
| Sun/Oracle Studio | 12.4+ *(2014)* |

> Full details and unsupported compilers listed in [`docs/compiler-support.md`](docs/compiler-support.md).

---

## Language Bindings *(coming soon)*

huzlib is being extended with translation layers for:

**Compiled languages** — Zig · Odin

**Interpreted / scripted** — Python · Lua

Each binding will wrap the C header interface cleanly so you can use these data structures from your language without rewriting them.

---

<div align="center">

*Built on ideas from the Linux kernel, the stb libraries, and a stubborn belief*
*that C can be expressive without being bloated.*

</div>
