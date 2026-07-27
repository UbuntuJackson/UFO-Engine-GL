#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include "block.h"

namespace ufo::visual{

class VirtualMachine{
public:


    class Operation{

    };

    FunctionDefinitionBlock main_func;

    VirtualMachine();

    std::map<std::string, std::unique_ptr<FunctionDefinitionBlock>> function_definition_blocks;
    std::map<std::string, std::unique_ptr<Block>> variable_definition_blocks;
    std::vector<FunctionCallBlock*> function_execution_stack;

    void Update(){

    }

};

}
