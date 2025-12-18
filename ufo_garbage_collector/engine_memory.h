#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "garbage_collector.h"
#include "../utils/console.h"

namespace ufo{

namespace gc{

class Object{
public:
friend GarbageCollector;

    bool alive = false;

    /*void* operator new(size_t i)
    {
        return malloc(i);
    }

    void operator delete(void* p)
    {
        free(p);
    }*/

public:

    std::vector<Object*> addresses;

    //Plan for potential local reference count algorithm
    std::unordered_map<Object*, int> local_reference_count;

protected:

    template<typename tType>
    tType* MakeMember(tType* _address){
        addresses.push_back(_address);
        return _address;
    }

public:

    void Decrement(Object* _variable){
        for(int i = addresses.size()-1; i != -1; i--){
            if(addresses[i] == _variable){
                Console::PrintLine("Removing reference", _variable);
                addresses.erase(addresses.begin()+i);
                break;
            }

        }
    }

    Object(){

    }

    template<typename tTypeA, typename tTypeB>
    tTypeB* Set(tTypeA* _variable, tTypeB* _address){
        for(int i = addresses.size()-1; i != -1; i--){
            if(addresses[i] == _variable){
                addresses.erase(addresses.begin()+i);
                break;
            }

        }

        addresses.push_back(_address);
        return _address;
    }

    virtual void OnMark(){

    }

    void Mark(){
        OnMark();
        alive = true;
        for(Object* addr : addresses){
            addr->Mark();
        }
    }

    /*void ResetAliveFalse(){
        alive = false;
        for(const auto& addr : addresses){
            addr->ResetAliveFalse();
        }
    }*/

    virtual ~Object(){
        Console::PrintLine("gc::Object::~Object",this);
    }

};

class Root : public Object{
public:
    GarbageCollector gc;

    Root(){
        gc.root = this;
    }

    ~Root(){
        //You have to release the references to all objects to do one last garbage collection for the last set of long lived memory
        addresses.clear();
        gc.Collect();
    }

};

}

}
