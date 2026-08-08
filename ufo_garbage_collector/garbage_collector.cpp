#include "garbage_collector.h"
#include "object.h"

namespace ufo{

//And finally, the Collect method.
// this can run every frame optionally, and at the end of the program it is run one last time to free everything,
// even leftover stuff that wasn't reached before.

void GarbageCollector::Collect(){
    //Reset all the object's alive status before the new gc cycle
    for(ufo::gc::Object* addr : memory){
        addr->alive = false;
    }

    //Starting from the root.
    root->Mark();

    unsigned int number_of_objects_garbage_collected = 0u;

    //Now purging everything that is not alive anymore.
    for(int i = memory.size()-1; i != -1; i--){

        if(!memory[i]->alive){
            delete memory[i];
            memory.erase(memory.begin()+i);

            number_of_objects_garbage_collected+=1u;
        }
    }

    if(number_of_objects_garbage_collected != 0u) Console::PrintLine("[UFO-Engine] Beam Garbage Collector with root '" + root->garbage_collected_object_name + "' ran, collected",number_of_objects_garbage_collected,"instances of gc::Object");

}

}
