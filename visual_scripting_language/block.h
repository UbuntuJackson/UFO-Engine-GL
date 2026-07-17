#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ufo::visual{

class VirtualMachine;

class Block{
public:
    std::vector<std::unique_ptr<Block>> blocks;

    virtual std::unique_ptr<Block> Operation(VirtualMachine* _vm);
};

//Definitionblocks

class FunctionDefinitionBlock : public Block{
public:
    std::vector<std::unique_ptr<Block>> parameters;

    std::vector<std::unique_ptr<Block>> contents;

    std::unique_ptr<Block> Operation(VirtualMachine* _vm) override;
};

class VariableDefinitionBlock : public Block{
public:
    std::string name;

    std::unique_ptr<Block> value;

    std::unique_ptr<Block> Operation(VirtualMachine* _vm) override;

};

//Call blocks

class FunctionCallBlock : public Block{
public:
    std::vector<std::unique_ptr<Block>> arguments;
    std::string name;

    std::unique_ptr<Block> Operation(VirtualMachine* _vm) override;
};

class FloatBlock : public Block{
public:
    float value;
    FloatBlock(float _value) : value{_value}{}
};

class AddFloatBlock : public Block{
public:

    std::unique_ptr<Block> Operation(VirtualMachine* _vm) override;
};

//

}
