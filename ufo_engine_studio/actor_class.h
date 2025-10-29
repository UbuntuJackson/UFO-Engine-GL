#pragma once
#include <string>
#include <vector>
#include <memory>
#include "property.h"

namespace UFOEngineStudio{

struct ActorClass{
    std::string name;
    std::string header_file = "";
    bool is_custom_class = false;
    std::vector<std::unique_ptr<Property>> exported_properties;
};

struct ActorVariant{
    int class_id;
    std::string category;
};

}