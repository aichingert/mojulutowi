#ifndef LU_MATH_H
#define LU_MATH_H

#include "lu.h"

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

Vec2 lerp_v2(Vec2, Vec2, f32);

#endif /* LU_MATH_H */
