#pragma once
#include <vector>

namespace ufo{

namespace gc{
class Object;
}

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