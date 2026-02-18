#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../utils/console.h"
#include "garbage_collector.h"

namespace ufo::gc{

class MemberVariableHandleBase{

public:
    friend ufo::gc::Object;

    virtual bool IsAlive() = 0;
    virtual void Mark() = 0;
    virtual ~MemberVariableHandleBase(){
    };
};

template<typename tType>
class MemberVariableHandle : public MemberVariableHandleBase{
    friend ufo::gc::Object;
    tType** handle;
public:
    MemberVariableHandle(tType** _handle)
    :
    handle{_handle}
    {

    }

    bool IsAlive(){
        return (*handle)->alive;
    }

    void Mark(){
        if((*handle)== nullptr) return;
        (*handle)->Mark();
    }
};

class Root;

//The base object for garabage collectable objects
class Object{
public:
    friend MemberVariableHandleBase;
    friend Root;
//Friending the GarbageCollector :)
friend GarbageCollector;

    //To make it easier to tell what is destroyed and created, every object can be given a name.
    std::string name  = "<Unnamed>";

private:
    std::vector<std::unique_ptr<MemberVariableHandleBase>> member_handles;
    template<typename tType>
    void MakeMarkableSafe(tType** _handle_to_member){
        //Console::PrintLine("before cast:", _);

        member_handles.push_back(std::make_unique<MemberVariableHandle<tType>>(_handle_to_member));
        //Console::PrintLine("after cast", members.back(), "\n");
    }

    //The members array. It holds pointers to every field that you want to be reached by the garbage collector.
    std::vector<Object**> members;
public:
    //The variable for whether object is alive or not
    bool alive = false;

protected:
    template<typename tType>
    void MakeMarkable(tType** _){
        MakeMarkableSafe(_);
    }

    //This is just a handle in case marking requires some sort of special treatment for extensions to the system, like the Json holder classes I made a while ago.
    virtual void OnMark(){

    }

public:
    //This is the actual mark function. It declares it alive before the next cycle.
    // If this is reached, the object will be spared from the rapture, like a good christian.
    void Mark(){
        if(alive) return;

        OnMark();

        alive = true;

        for(auto&& addr : member_handles){
            addr->Mark();
        }
    }

    //This is just my way of telling what is destroyed. Check the output!
    virtual ~Object(){
        //Console::Print("Destroyed object", name, this, "\n");
    }
/*protected:
    //Callback approach
    void mark(Object* _obj){
        if(_obj != nullptr) _obj->Mark();
    }

    void mark(DynamicArray<Object*>& _obj){
        _obj.OnMark();
        }*/
};

//Inherit from this as the root of your program.
class Root : public Object{
public:
    //Friending Object :)
    friend ufo::gc::Object;

    //The GarbageCollectoro lives here.
    GarbageCollector gc;

    Root(){

        gc.root = this;
        gc.memory.push_back(this);

    }

    ~Root(){

        //You have to release the references to all objects to do one last garbage collection for the last set of long lived memory
        member_handles.clear();
        gc.Collect();
    }

};

}
