#pragma once

#include <memory>
#include "actor.h"
#include "../ufo_garbage_collector/gc_json.h"

namespace ufo{

class GenericGenerator{
public:
    virtual void Initialise() = 0;
    virtual std::unique_ptr<Actor> FromJson(ufo::gc::JsonMap* _json) = 0;
};

}