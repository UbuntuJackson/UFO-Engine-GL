#pragma once

namespace ufo{

class ActorHeap{
public:
    class Memory{
    public:
        void* start_address;
        bool free;
        Memory* next_address = nullptr;
        Memory(void* _start_address, void* _next_address, bool _free)
        start_address{_start_address},
        next_address{_next_address},
        free{_free}
        {}

        size_t GetSize(){
            if(next_address == nullptr) return 0;
            return next_address-start_address;
        }
    };

    void FreeLinkedList(){

        Memory* iterator = &start_address;

        while(true){
            delete iterator;

            if(iterator->next_address == nullptr) break;
            iterator = iterator->next_address;

        }

    }

    void* current_address = nullptr;

    Memory* start_address;

    char heap[4096*600];

    void Initialise(){
        current_address = (void*)(heap);

        start_address = new Memory(current_address, current_address+sizeof(heap));
    }

    void* FindFreeMemory(size_t _size){
        Memory* iterator = start_address;

        while(true){
            if(iterator->free && iterator->GetSize() >= _size){

                iterator->free = false;
                if(iterator->GetSize() > _size){

                    //Make free memory chunk after this one
                    iterator->next = new Memory(
                        iterator->next,
                        iterator->next->next,
                        true
                    );
                }

                return iterator->address;
            }

            if(iterator->next_address == nullptr) break;

            iterator = iterator->next_address;

        }

        return nullptr;
    }

    template <typename tActor, typename ...tArgs>
    tActor* Allocate(tArgs&& ..._args){
        void* addr = FindFreeMemory(sizeof(tArgs));

        if(addr == nullptr) return nullptr;

        tActor* ptr = new(addr) tActor(_args...);

        return ptr;

    }
};

}
