/*

    DISCLAIMER

    This garbage collector and the components associated with it are
    purely experimental. For the time being it is reserved for research-
    related purposes
    /jackson.

*/

#pragma once
#include <vector>
namespace ufo{

class Object;

class GarbageCollector{
public:
    std::vector<gc::Object*> memory;
    gc::Object* root;

    template <typename tType, typename ...tArgs>
    tType* New(tArgs&& ..._args){
        tType* m = new tType(_args...);
        
        memory.push_back(m);

        return m;
    }

    void Collect();
};

}