#ifndef SHAPE_H
#define SHAPE_H

#include "../external/olcPixelGameEngine.h"
#include "../actor/actor.h"
#include "../drawing_system/drawing_system.h"
#include "../camera/camera.h"
#include "../console/console.h"
/*class ShapeBase : Actor{

};*/

template<typename tShape>
class Shape : public Actor{
public:
    bool always_visible;
    olc::vf2d offset;
    Shape(tShape _shape, olc::vf2d _position, bool _always_visible = false, olc::vf2d _offset = {0.0f, 0.0f}) : shape{_shape}, offset{_offset}, always_visible{_always_visible}, Actor(_position){}
    tShape shape;
    void OnUpdate(){
        shape.position = position+offset;
    }

    void UpdateGlobalPosition(olc::vf2d _position){
        Actor::UpdateGlobalPosition(_position);

        shape.position = position+offset;
        
    }

    void OnDebugDraw(Camera* _camera){
        
        (&shape)->Draw(_camera);
        
    }

    
};

#endif