#include "math.h"

Vec2 lerp_v2(Vec2 a, Vec2 b, f32 t) {
    return (Vec2){
        .x = a.x * t + b.x * (1.0f - t),
        .y = a.y * t + b.y * (1.0f - t),
    };
}

