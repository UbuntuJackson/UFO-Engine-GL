#pragma once
#include <string>
#include <functional>
#include <memory>
#include <map>
#include <actor.h>
#include <garbage_collector.h>
#include <gc_json.h>

class ActorFactoryGenerator : public ufo::gc::Root{
public:
    std::map<std::string, std::function<std::unique_ptr<Actor>()>> factory_map;

    //Actor takes a class
    void Generate(const std::string& _path){

        std::string code_segment;

        ufo::gc::JsonMap* exported_variables = ufo::gc::JsonRead(&gc, _path);

        //Classname absolutely has to have the namespace attached to it. It's not enough to have Player if it's in a namespace
        // you have to have for example game::Player if it is in a namespace.
        std::string class_name = exported_variables->AsMap().at("class_name")->AsString();
        code_segment += "if(actor_json->AsMap().at(\"class_name\") == "+class_name+"){";

        for(const auto& variable : exported_variables->AsMap().at("exported_variables")->AsArray()){
            
            auto map = variable->AsMap();
            std::string data_type = map.at("data_type")->AsString();
            std::string name = map.at("name")->AsString();
            std::string value = map.at("value")->AsString();
            
            code_segment+="instance->"+name+"="+value+";";
        }

        code_segment += "}";

    }
    
};