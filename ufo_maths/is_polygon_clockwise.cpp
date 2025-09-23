#include "is_polygon_clockwise.h"
namespace ufoMaths{
bool IsPolygonClockwise(ufo::Polygon _polygon){
    float sum = 0.0f;
    for(int i = 0; i < _polygon.size(); i++){
        float x0 = _polygon[i].x;
        float y0 = _polygon[i].y;
        float x1 = _polygon[(i+1)%(_polygon.size())].x;
        float y1 = _polygon[(i+1)%(_polygon.size())].y;
        sum += (x1-x0)*(y1+y0);
    }
    return (sum < 0.0f);
    /*
        The reason this claims clock-wise when sum is under 0 instead of over 0, like in conventional maths, is because
        when you have a clock, and look at it in a mirror, it spins in the opposite direction. Same when you have the screen flipped
        along the y-axis.
    */
}
}