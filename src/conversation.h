#pragma once
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

class Conversation : public Actor{
public:
    int current_widget = ufo::Maths::NULL_ID;

    void OnSpawn(){

    }

    void OnDraw(){

    }
};

}
