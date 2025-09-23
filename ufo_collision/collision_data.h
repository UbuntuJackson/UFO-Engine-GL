#pragma once
#include "../external/olcPixelGameEngine.h"
#include "../shapes/ray2.h"

enum PillCollidedSegment{
    LEFT_SIDE,
    RIGHT_SIDE,
    TOP_CIRCLE,
    BOTTOM_CIRCLE
};

struct CollisionData{
    float collision_time;
    float other_collision_time;
    olc::vf2d intersection_point;
    bool is_hit;
    float cross_product_z_component;
    float other_cross_product_z_component;
};

struct RayVsCircleCollisionData{
    float collision_time;
    float other_collision_time;
    olc::vf2d intersection_point;
    olc::vf2d other_intersection_point;
    olc::vf2d collision_normal;
    bool is_hit;
};

struct PillResolutionData{
    float collision_time;
    Ray2 line_segment;
    int pill_collided_segment;
};