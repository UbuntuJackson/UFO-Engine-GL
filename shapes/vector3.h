#ifndef VECTOR_3_H
#define VECTOR_3_H

struct Vector3{
    float x;
    float y;
    float z;
    Vector3 operator+(Vector3 _other){
        return Vector3{x+_other.x, y+_other.y, z+_other.z};
    }
    Vector3 operator-(Vector3 _other){
        return Vector3{x-_other.x, y-_other.y, z-_other.z};
    }
};

#endif