#ifndef CROSS_PRODUCT_H
#define CROSS_PRODUCT_H

#include "../shapes/vector3.h"

Vector3 CrossProduct(Vector3 _a, Vector3 _b){
    float x = (_a.y*_b.z)-(_a.z*_b.y);
    float y = (_a.z*_b.x)-(_a.x*_b.z);
    float z = (_a.x*_b.y)-(_a.y*_b.x);
    return Vector3{x, y, z};
}

#endif