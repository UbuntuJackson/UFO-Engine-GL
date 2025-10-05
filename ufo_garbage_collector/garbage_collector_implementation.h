#pragma once
#include "garbage_collector.h"
#include "engine_memory.h"

#define GARBAGE_COLLECTOR_IMPLEMENTATION()\

ufo::GarbageCollector gc;\

std::vector<ufo::gc::Object*> addresses;\

void InvokeGarbageCollector(){\

for(ufo::gc::Object* addr : gc.memory){\
    addr->alive = false;\
}\

for(const auto& address : addresses){\
    address->Mark();\
}\

for(int i = gc.memory.size()-1; i != -1; i--){\
    if(!gc.memory[i]->alive){\
        delete gc.memory[i];\
        gc.memory.erase(gc.memory.begin()+i);\
    }\
}\

}\
