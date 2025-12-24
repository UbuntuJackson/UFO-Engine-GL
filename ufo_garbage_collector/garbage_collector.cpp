#include "garbage_collector.h"
#include "engine_memory.h"

namespace ufo{

void GarbageCollector::Collect(){
    for(gc::Object* addr : memory){
        addr->alive = false;
    }

    root->Mark();

    unsigned int number_of_objects_deleted = 0u;

    for(int i = memory.size()-1; i != -1; i--){
        if(!memory[i]->alive){
            delete memory[i];
            memory.erase(memory.begin()+i);

            number_of_objects_deleted++;
        }
    }

    if(number_of_objects_deleted!=0u) Console::PrintLine("[UFO-Engine] Running Beam Garbage Collector. Deleted",number_of_objects_deleted,"objects");

}

}
