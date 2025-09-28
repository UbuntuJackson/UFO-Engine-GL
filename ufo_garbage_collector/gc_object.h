#pragma once
#include <vector>
#include "garbage_collector.h"
#include "../utils/console.h"

namespace ufo{

namespace gc{

class Object{
public:

    Object(){}

    bool alive = false;

    std::vector<Object*> addresses;

    template<typename tType>
    tType* MakeMember(tType* _address){
        addresses.push_back(_address);
        return _address;
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

    void Mark(){
        for(Object* addr : addresses){
            addr->Mark();
            addr->alive = true;
        }
    }

    /*void ResetAliveFalse(){
        alive = false;
        for(const auto& addr : addresses){
            addr->ResetAliveFalse();
        }
    }*/

    virtual ~Object(){
        //Console::PrintLine(this);
    }

};

template<typename tType>
class ObjectPtr{
public:
    //std::vector<>

    tType* ptr;
};

class Main : public Object{
public:
    GarbageCollector gc;

    Main(){
        gc.root = this;
    }

};

}

}