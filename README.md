<div align="center">

```
 ██╗  ██╗██╗   ██╗███████╗██╗     ██╗██████╗
 ██║  ██║██║   ██║╚══███╔╝██║     ██║██╔══██╗
 ███████║██║   ██║  ███╔╝ ██║     ██║██████╔╝
 ██╔══██║██║   ██║ ███╔╝  ██║     ██║██╔══██╗
 ██║  ██║╚██████╔╝███████╗███████╗██║██████╔╝
 ╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚══════╝╚═╝╚═════╝
```

**Generic data structures and algorithms for C — no bloat, no templates, no nonsense.**

[![C11](https://img.shields.io/badge/C-C11-blue?style=flat-square)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![License](https://img.shields.io/badge/license-GPL--2.0-green?style=flat-square)](LICENSE)
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

## The Insight (a short story in 3 acts)

### Act 1 — Macros Can Write Code

> *"What if I didn't write it by hand at all?"*

The first idea: use the C preprocessor as a code generator.

```c
// Define once...
DEFINE_STACK(int, 32, my_stack)   // → goes in your .h file
GENERATE_STACK(int, 32, my_stack) // → goes in your .c file

// Use it
stack_push(int, &my_stack, 42);
```

Macros expand at compile time and write the boilerplate *for* you. Clean. Clever.

But there's a catch — you still carry the type name everywhere you call it. And the compiled binary? Still has three separate stacks. Three separate `_push`. Three separate `_pop`. The bloat survived.

---

### Act 2 — The Linux Kernel's Secret

> *"What if the code never cared about the type at all?"*

Deep inside the Linux kernel lives one of the most elegant tricks in all of C. Andrea Arcangeli's `rbtree`.

The classic way to embed a type into a BST:

```c
// The naive way — one tree per type
struct tensor_node {
    struct tensor_node *left, *right;
    struct tensor data;          // ← tied forever to 'tensor'
};
```

The kernel way:

```c
// The smart way — one tree for every type
struct bst_node {
    struct bst_node *left, *right;
    // no data here
};

struct tensor_node {
    struct bst_node bst;         // ← the tree lives here
    struct tensor data;          // ← your data lives here
};
```

Separate the *mechanics* of a tree (rotate, rebalance, link) from the *meaning* of the data. The tree operates on `bst_node`. It never sees `tensor`. It never has to.

One set of functions. Every type. Zero extra code. **Legendary.**

---

### Act 3 — A Stack is Just Bytes

> *"What if we took that idea all the way?"*

A stack does not care what you push. It pushes bytes in. It pops bytes out. It counts. That's it.

So we create one internal type that works purely with bytes:

```c
struct __internal_stack {
    size_t len_bytes;
    unsigned char buf[]; // flexible array member — no fixed size
};
```

Your real stack — say, a stack of 32 `int`s — looks like this in memory:

```
[ len_bytes | int | int | int | ... ]
```

The internal type looks like this:

```
[ len_bytes | byte | byte | byte | ... ]
```

**Same layout. Same addresses. Different glasses looking at the same thing.**

A macro converts your stack into the internal type at call time:

```c
#define static_stack_push(stack, val) \
    __static_stack_push(__cast(stack), sizeof((stack)->buf[0]), &(val))
```

The function pushes bytes. The macro extracts the byte size of your element, does the cast, and calls through. The caller sees typed values. The function sees bytes.

**Write once. Compile once. Works for every type you throw at it.**

---

## What's Inside

```
huzlib/
├── stack.h          — dynamic generic stack
├── static-stack.h   — fixed-size generic stack (no malloc)
├── list.h           — generic linked list
├── avltree.h        — self-balancing AVL tree
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
#ifndef HUZLIB_$$_H
#define HUZLIB_$$_H

   #ifndef HUZLIB_$$_INCLUDES
   #define HUZLIB_$$_INCLUDES
      /*
       * includes
       * --------
       * To stay single-header, every internal dependency from "utils/" (assert.h, hints.h, etc.)
       * is hand-inlined directly here. The only external inclusions are 
       * freestanding libc headers (<stddef.h>, <stdint.h>, <stdbool.h>).
       *
       * Every macro defined here is #ifndef guarded so it never
       * clobbers a definition the caller already has.
       */
   #endif /* HUZLIB_$$_INCLUDES */

   /*
    * header section
    * --------------
    * All public declarations live here, in this order:
    * 1. type declarations
    */

   #ifndef HUZLIB_$$_INLINE_IMPL
   #define HUZLIB_$$_INLINE_IMPL
      /*
       * inline section
       * --------------
       * Small, high-frequency functions (e.g. empty checks, size lookups).
       * Uses a visibility macro (HUZ_$$_INLINE_API) to flip behavior:
       * - Standard App / Static build  -> drops as 'static inline'
       * - Shared Library generation    -> compiles into exported symbols
       */
   #endif /* HUZLIB_$$_INLINE_IMPL */

   /*
    * interface & translation section
    * -------------------------------
    * 2. function declarations (for heavy, out-of-line implementation functions)
    * 3. macro translation layer  (the generic call wrappers that route to either
    *    the inline logic or the heavy implementation functions)
    */

   #ifdef HUZLIB_$$_IMPL
      /*
       * implementation section
       * ----------------------
       * Definitions for every heavy, non-inlined function (e.g. memory scrubbing).
       * Only compiled when HUZLIB_$$_IMPL is defined — once,
       * in exactly one translation unit.
       */
   #endif /* HUZLIB_$$_IMPL */

   #ifdef HUZLIB_$$_TEST
      #include "pcg_basic.h"
      #include "unity.h"
      /*
       * testing section
       * ---------------
       * Self-contained unit tests using UnityTestFramework.
       * PCG-BASIC drives the random inputs for stress tests.
       */
   #endif /* HUZLIB_$$_TEST */

#endif /* HUZLIB_$$_H */
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
HUZLIB_ + filename in CAPS (hyphens → underscores) + _IMPL

static-stack.h  →  HUZLIB_STATIC_STACK_IMPL
avltree.h       →  HUZLIB_AVLTREE_IMPL
list.h          →  HUZLIB_LIST_IMPL
```

**In exactly one `.c` file**, define the implementation macro before including:

```c
// myapp.c  — do this ONCE across your whole project
#define HUZLIB_STACK_IMPL
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
$ cc -DHUZLIB_STACK_IMPL -x c -c static-stack.h -o static-stack.o
 
# Link it with your program as normal
$ cc main.c static-stack.o
```
 
`-x c` overrides the file extension — the compiler stops treating `.h` as a header-only include and compiles it as a full translation unit. The implementation lives in `static-stack.o`. Your `main.c` just includes the header without ever defining the macro.
 
Both methods produce the same binary. Pick whichever fits your workflow.
 
---

### Method 3 — Shared Library Compilation *(`.so` / `.dll`)*

If you want to compile `huzlib` as a standalone shared binary (allowing cross-language runtime engines like Python, Zig, or Rust FFI to tap into it), you can explicitly compile it into a shared object.

When doing this, defining `HUZLIB_$$_SHARED` tells the Inline Section to write standard, exportable machine-code symbols directly into the binary layout so external dynamic linkers can locate them by name.

```sh
# Compile the header directly into a shared library (.so)
$ cc -DHUZLIB_STATIC_STACK_IMPL -DHUZLIB_STATIC_STACK_SHARED -x c -fPIC -shared static-stack.h -o libhuzstaticstack.so

# Link a C application against your compiled .so
# (Your C app gets inline performance for fast paths, but binds to the .so for slow paths)
$ cc main.c -L. -lhuzstaticstack -o app
```

---

### Running the Tests
 
Every header ships with its own self-contained test suite. To run it, add the `_TEST` flag alongside `_IMPL` — same naming rule applies:
 
```
HUZLIB_ + filename in CAPS + _TEST
 
static-stack.h  →  HUZLIB_STATIC_STACK_TEST
avltree.h       →  HUZLIB_AVLTREE_TEST
```
 
```sh
$ cc -DHUZLIB_STATIC_STACK_IMPL -DHUZLIB_STATIC_STACK_TEST -x c static-stack.h lib/unity.c lib/pcg_basic.c -o test
$ ./test
```
 
`unity` is the test framework. `pcg_basic` is a random number generator used to stress-test the data structures. Both live in `lib/` and only need to be compiled when running tests — they are never part of your actual binary.
 
---

## Quick Example — A Generic Stack

```c
#define HUZLIB_STATIC_STACK_IMPL
#include "static-stack.h"

// Declare a stack of 64 floats
STATIC_STACK(float, 64, float_stack);

int main(void)
{
    struct float_stack s = STATIC_STACK_INIT(s);

    static_stack_push(&s, 3.14f);
    static_stack_push(&s, 2.71f);

    float *top = static_stack_peek(&s);  // → points to 2.71f
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

**Compiled languages** — Rust · Zig · Odin · C++

**Interpreted / scripted** — Python · Lua

Each binding will wrap the C header interface cleanly so you can use these data structures from your language without rewriting them.

---

## Why Not Just Use C++ / Rust?

Sometimes you can't. Embedded targets, kernel code, legacy codebases, or just personal preference — C is often the only option or the right one.

But even when you *can*, there's a deeper reason to stay in C.

C++ templates feel generic. But every instantiation is its own compiled copy. The binary swells quietly — you never asked for five versions of `sort`, but you got them. And that's before you touch virtual functions. A vtable lookup is an indirect call through a pointer. The CPU cannot predict it. The branch predictor stalls. The optimizer goes blind — it cannot inline through a pointer it doesn't know at compile time. You pay for a runtime dispatch you never wanted, on code you thought was static.

Rust is better disciplined, but monomorphization is still monomorphization. `Vec<i32>` and `Vec<f64>` are still two separate blobs of compiled machine code. `dyn Trait` buys you one copy at the cost of — again — a vtable.

huzlib takes neither trade. One compiled function. No copies. No vtables. No hidden indirection. The only thing the CPU sees is a direct call and a byte operation. Everything else is a macro that dissolved at compile time.

**You always know exactly what the machine is doing. That's the point.**

---

## License

GPL-2.0. See [`LICENSE`](LICENSE).

---

<div align="center">

*Built on ideas from the Linux kernel, the stb libraries, and a stubborn belief*
*that C can be expressive without being bloated.*

</div>
