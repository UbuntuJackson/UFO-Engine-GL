#pragma once
#include <array>
#include <stddef.h>

namespace ufo{

template <int tSize>
class Heap{
public:

    struct Data{
        void* pointer;
        size_t size;
        bool free;
    };

    std::array<Data, tSize> heap;

    int current_size = 0;

    template<typename tType>
    tType* Allocate(){
        for(int i = 0; i< current_size; i++){
            if(heap[i].free) heap[i] = Data{(void*)(new tType()), sizeof(tType), true};
            return (tType*)(&heap[i]);
        }
        heap[current_size] = Data{(void*)(new tType()), sizeof(tType), true};
        current_size++;
        return (tType*)(&heap[current_size]-1);
    }

    ~Heap(){
        for(Data data : heap){
            delete data.pointer;
        }
    }
};

}
