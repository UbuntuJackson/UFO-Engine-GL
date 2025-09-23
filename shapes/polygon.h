#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include "../external/olcPixelGameEngine.h"
#include "ray2.h"

namespace ufo{

typedef std::vector<olc::vf2d> Polygon;
typedef std::vector<Ray2> LineSegmentPolygon;

}

#endif