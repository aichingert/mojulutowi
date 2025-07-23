#ifndef LU_GENERAL_H
#define LU_GENERAL_H

#include <stdint.h>

#define COUNT(arr) sizeof(arr) / sizeof(arr[0])

#define MIN(a, b) (a) > (b) ? (b) : (a)
#define MAX(a, b) (a) < (b) ? (b) : (a)

typedef uint64_t    u64;
typedef uint32_t    u32;
typedef uint16_t    u16;
typedef uint8_t     u8;

typedef int64_t     s64;
typedef int32_t     s32;
typedef int16_t     s16;
typedef int8_t      s8;

typedef float       f32;
typedef double      f64;


#endif /* LU_GENERAL_H */
