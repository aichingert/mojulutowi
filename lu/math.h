#ifndef LU_MATH_H
#define LU_MATH_H

#include "lu.h"

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

typedef struct Vec2 {
    f32 x;
    f32 y;
} Vec2;

typedef struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
} Vec3;

typedef struct Vec4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vec4;

Vec2 lu_lerp_v2(Vec2, Vec2, f32);

#endif /* LU_MATH_H */
