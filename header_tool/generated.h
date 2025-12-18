#include <functional>
#include <memory>

#include "UFO-Engine-GL/ufo_garbage_collector/gc_json.h"
#include "UFO-Engine-GL/src/generic_generator.h"
#include "UFO-Engine-GL/src/actor.h"
#include "../src/enemy_example.h"

namespace Generated{

class ActorGenerator : public ufo::GenericGenerator{

    std::map<std::string, std::function<std::unique_ptr<Actor>(ufo::gc::JsonMap* _json)>> factory_map;
    void Initialise(){
        factory_map.emplace(
            "game::Knight",
            [](ufo::gc::JsonMap* _json){
                float _x = _json->map.at("x")->AsFloat();
                float _y = _json->map.at("y")->AsFloat();
                auto instance = std::make_unique<game::Knight>(Vector2f(_x, _y));
                instance->speed = _json->map.at("x")->AsFloat();
                instance->iq = (int)(_json->map.at("x")->AsFloat());
                instance->slider = (int)(_json->map.at("x")->AsFloat());
                instance->type_of_knight = (int)(_json->map.at("x")->AsFloat());
                instance->name = _json->map.at("x")->AsString();
                instance->a = (int)(_json->map.at("x")->AsFloat());
                return std::move(instance);
            }
        );
        factory_map.emplace(
            "game::ColourRectangle",
            [](ufo::gc::JsonMap* _json){
                float _x = _json->map.at("x")->AsFloat();
                float _y = _json->map.at("y")->AsFloat();
                auto instance = std::make_unique<game::ColourRectangle>(Vector2f(_x, _y));
                instance->colour_hex = (int)(_json->map.at("x")->AsFloat());
                return std::move(instance);
            }
        );
    }
    std::unique_ptr<Actor> FromJson(ufo::gc::JsonMap* _json){return std::move(factory_map.at(_json->map.at("name")->AsString())(_json));}};

}
