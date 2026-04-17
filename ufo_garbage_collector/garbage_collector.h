//Copyright (c) UbuntuJackson - UFO-Engine Software
// this is an experimental garbage collector for simpler C++ applications.

#pragma once
#include <vector>

namespace ufo::gc{
class Object;
}

namespace ufo{

class GarbageCollector{
public:
    //ufo::Heap<1000> heap;

    GarbageCollector(){
        //heap.Allocate<gc::Object>();
    }

    //This is basically all of the program's garbage-collected memory.
    std::vector<ufo::gc::Object*> memory;

    //This is a handle to the root of the program.
    ufo::gc::Object* root;

    //Small idea: Only run Object::Mark hierarchy when New is called
    //

    //This is basically used like the new keyword in Java.
    template <typename tType, typename ...tArgs>
    tType* New(tArgs&& ..._args){
        tType* m = new tType(_args...);

        //Just an idea, maybe the member_handles function could be resized beforehand, so that adding items does not take as much time.
        //m->member_handles.reserve(sizeof(*tType));

        memory.push_back(m);

        return m;
    }

    template <typename tType>
    tType* Add(tType* _ptr){
        memory.push_back(_ptr);
        return _ptr;
    }

    void Collect();
};

}
