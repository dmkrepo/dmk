# DMK

Header-only library

#### dmk.h
Various definitions and helper macroses.
Compiler, os and cpu architecture detection.
Defines `DMK_ARCH_X32`, `DMK_ARCH_X64`, `DMK_ARCH_SSE`, `DMK_ARCH_AVX`, `DMK_OS_WIN`, `DMK_COMPILER_MSVC`, 
`DMK_COMPILER_GNU`, `DMK_COMPILER_CLANG` etc

#### dmk_assert.h
Assertions and debug functions.

`DMK_ASSERT_EQ`, `DMK_ASSERT_NE`, `DMK_ASSERT_LT`, `DMK_ASSERT_GT`, `DMK_ASSERT_LE`, `DMK_ASSERT_GE`, 
`DMK_ASSERT_NULL`, `DMK_ASSERT_NOTNULL`
checks if condition is true, otherwise detailed message is written to std::err 
(define DMK_ERROR_STREAM before including this header to redirect output)

#### dmk_fraction.h

Fraction type used in time measurements.

#### dmk_time.h

Time-related functions and benchmarking.

#### dmk_memory.h

Memory allocation etc.

### License

GPL 2.0

### Compability
* MSVC 2015
* GCC 4.8+
* Clang 3.5+
