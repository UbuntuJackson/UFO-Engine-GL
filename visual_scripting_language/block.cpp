#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "../utils/console.h"
#include "block.h"
#include "virtual_machine.h"

namespace ufo::visual{

std::unique_ptr<Block> Block::Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) {}

std::unique_ptr<Block> Block::Clone(){
    return std::make_unique<NullBlock>();
}

//Definitionblocks

std::unique_ptr<Block> FunctionDefinitionBlock::Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) {

    std::map<std::string, Block*> local_variables_and_args;

    for(auto& [identifier, arg] : _args){
        local_variables_and_args.emplace(identifier, arg.get());
    }

    for(const auto& block : contents){
        block->Operation(_vm, _args);

    }

    return std::make_unique<NullBlock>();
}

std::unique_ptr<Block> VariableDefinitionBlock::Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) {

    _vm->variable_definition_blocks.emplace(name, value->Clone());

    return std::make_unique<NullBlock>();

}

//Call blocks

std::unique_ptr<Block> FunctionCallBlock::Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) {
    //Need to access a FunctionDefinitionBlock here somehow

    _vm->function_definition_blocks.at(name)->Operation(_vm, arguments);

    return std::make_unique<NullBlock>();

}

std::unique_ptr<Block> PrintBlock::Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) {

    value->Print();

    return std::make_unique<NullBlock>();
}

std::unique_ptr<Block> AddFloatBlock::Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) {
    float a = dynamic_cast<FloatBlock*>(_args.at("_first").get())->value;
    float b = dynamic_cast<FloatBlock*>(_args.at("_second").get())->value;
    return std::make_unique<FloatBlock>(a+b);
}

//NullBlock

std::unique_ptr<Block> NullBlock::Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) {
    return nullptr;
}

std::unique_ptr<Block> ReturnBlock::Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) {
    return nullptr;
}

}
