#pragma once
#include "../utils/console.h"
#include "ufo_maths.h"
void WrapiTest(){
    for(int i = -10; i < 10; i++){
        Console::Out(ufoMaths::Wrapi(i, 0, 4));
    }

    Console::Out("---");

    for(int i = -10; i < 10; i++){
        Console::Out(ufoMaths::Wrapi(i, -4, 0));
    }

    for(int i = -10; i < 10; i++){
        Console::Out(ufoMaths::Wrapi(i, 0, 1));
    }
}