#include "object.h"
#include "dynamic_array.h"
#include "../ufo_maths/ufo_maths.h"
#include "../src/graphics.h"
#include "../src/camera.h"

namespace ufo::gc{

class Actor : public Object{
public:
    Vector2f local_position;
    Engine* engine = nullptr;

    Vector2f GetGlobalPosition(){
        return local_position;
    }

    Actor(Vector2f _local_position): local_position{_local_position}{}

    virtual void OnUpdate(float _delta_time){

    }

    virtual void OnDraw(ufo::Graphics* _graphics, Camera* _camera){

    }

};

}
