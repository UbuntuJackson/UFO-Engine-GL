#include "garbage_collector.h"
#include "gc_object.h"

namespace ufo{

void GarbageCollector::Collect(){
    for(gc::Object* addr : memory){
        addr->alive = false;
    }

    root->Mark();

    for(int i = memory.size()-1; i != -1; i--){
        if(!memory[i]->alive){
            //Console::PrintLine("Tried to delete", memory[i]);
            delete memory[i];
            memory.erase(memory.begin()+i);
        }
    }
    
}

}