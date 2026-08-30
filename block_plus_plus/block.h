#pragma once
#include "../src/widget.h"

namespace ufo::BlockPlusPlus{

class Block : ufo::Widget{
public:
};

class FunctionBlock : public Block{
public:
};

class ClassBlock : public Block{
public:
};

class Variable : public Block{
public:
    void OnSpawn(){
        //Add name node
        //Add value node

    }

};

}
