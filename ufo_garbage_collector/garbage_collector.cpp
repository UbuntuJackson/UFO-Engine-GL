#include "garbage_collector.h"
#include "engine_memory.h"

namespace ufo{

void GarbageCollector::Collect(){
    for(gc::Object* addr : memory){
        addr->alive = false;
    }

    root->Mark();

    for(int i = memory.size()-1; i != -1; i--){
        if(!memory[i]->alive){
            delete memory[i];
            memory.erase(memory.begin()+i);
        }
    }
    
}

}