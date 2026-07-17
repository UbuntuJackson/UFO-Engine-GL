#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "block.h"
#include "virtual_machine.h"

namespace ufo::visual{

std::unique_ptr<Block> Block::Operation(VirtualMachine* _vm) {}

//Definitionblocks

std::unique_ptr<Block> FunctionDefinitionBlock::Operation(VirtualMachine* _vm) {
    for(const auto& block : contents){
        _vm->stack.push_back(block.get());

    }
}

std::unique_ptr<Block> VariableDefinitionBlock::Operation(VirtualMachine* _vm) {}

//Call blocks

std::unique_ptr<Block> FunctionCallBlock::Operation(VirtualMachine* _vm) {
    //Need to access a FunctionDefinitionBlock here somehow

    _vm->function_definition_blocks.at(name)->Operation(_vm);
}

std::unique_ptr<Block> AddFloatBlock::Operation(VirtualMachine* _vm) {
    float a = dynamic_cast<FloatBlock*>(_vm->stack[0])->value;
    float b = dynamic_cast<FloatBlock*>(_vm->stack[1])->value;
    _vm->stack.push_back(std::make_unique<FloatBlock>(a+b));
}

}
