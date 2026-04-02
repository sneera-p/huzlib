# Compiler Support: `typeof`/`decltype` and `_Generic`

## ✅ Compiler Support

The following list contains the minimum version of compilers supported by
this library. The reason for this is the requirement to have the compiler
tools ```__typeof__(x) / __decltype(x) ``` and ```_Generic(x, ...)```

| Compiler Family              | Minimum Version              | Year   |
|------------------------------|------------------------------|--------|
| **Clang**                    | 3.0+                         | 2011   |
| **GCC**                      | 4.9+                         | 2014   |
| **Intel ICC**                | 15.0+                        | 2014   |
| **Intel ICX/LLVM**           | 2021.1+                      | 2021   |
| **ARMCLANG**                 | 6.10+                        | 2017   |
| **TinyCC**                   | 0.9.27+                      | 2017   |
| **MSVC**                     | VS 2019 16.8+                | 2020   |
| **Pelles C**                 | 8.0+                         | 2015   |
| **Sun/Oracle Studio**        | 12.4+                        | 2014   |
| **IBM XL C**                 | 13.1+                        | 2013   |
| **NVCC (CUDA)**              | 9.0+                         | 2017   |
| **Zig CC**                   | 0.10.0+                      | 2022   |

## ⚠️ Not Supported

| Compiler                    | Reason                                    |
|-----------------------------|-------------------------------------------|
| **ARMCC**                   | No `_Generic` support                     |
| **DJGPP**                   | Stuck at GCC 2.95                         |
| **IAR Embedded**            | C99 only in C mode                        |
| **Keil MDK**                | C99 only                                  |
| **TI Compilers**            | No C11 or modern features                 |
| **Watcom C**                | C89/C99 partial only                      |
| **Borland C++**             | Legacy, no modern C support               |
| **Turbo C**                 | C89 only                                  |
| **SDCC**                    | 8-bit targets, limited                    |

