namespace ufo::visual{

#include <algorithm>
#include <map>
#include <memory>
#include "block.h"

class VirtualMachine{
public:
    class Operation{

    };

    std::vector<std::unique_ptr<Block>> stack;

    std::map<std::string, std::unique_ptr<FunctionDefinitionBlock>> function_definition_blocks;
    std::map<std::string, std::unique_ptr<VariableDefinitionBlock>> variable_definition_blocks;

};

}
