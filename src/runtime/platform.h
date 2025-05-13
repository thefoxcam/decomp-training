#ifndef RUNTIME_PLATFORM_H
#define RUNTIME_PLATFORM_H

/*
    It's not required to use these everywhere, these are mainly for sections
    like 64-bit integers.
*/

/// A signed 8-bit integer
typedef signed char s8;

/// A signed 16-bit integer
typedef signed short s16;

/// A signed 32-bit integer
typedef signed long s32;

/// A signed 64-bit integer
typedef signed long long s64;

/// An unsigned 8-bit integer
typedef unsigned char u8;

/// An unsigned 16-bit integer
typedef unsigned short u16;

/// An unsigned 32-bit integer
typedef unsigned long u32;

/// An unsigned 64-bit integer
typedef unsigned long long u64;

/// A 32-bit floating-point number
typedef float f32;

/// A 64-bit floating-point number
typedef double f64;

#endif
