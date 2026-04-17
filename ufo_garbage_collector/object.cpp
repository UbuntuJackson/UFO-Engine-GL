#include <vector>
#include <string>
#include <memory>
#include "../utils/console.h"
#include "garbage_collector.h"
#include "object.h"
#include "dynamic_array.h"

namespace ufo::gc{

//Callback approach
void Object::mark(Object* _obj){
    if(_obj != nullptr) _obj->Mark();
}

void Object::mark(DynamicArray<Object*>& _obj){
    _obj.Mark();
}

}
