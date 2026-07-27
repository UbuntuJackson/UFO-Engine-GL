#include <algorithm>
#include <map>
#include <memory>
#include "block.h"
#include "virtual_machine.h"

namespace ufo::visual{

VirtualMachine::VirtualMachine(){

    auto variable = std::make_unique<VariableDefinitionBlock>();
    variable->value = std::make_unique<FloatBlock>(5);
    main_func.contents.push_back(std::move(variable));
    auto print_block = std::make_unique<PrintBlock>();
    print_block->value = std::make_unique<FloatBlock>(5);
    main_func.contents.push_back(std::move(print_block));

}

}
