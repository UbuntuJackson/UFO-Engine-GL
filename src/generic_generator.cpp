#include <memory>
#include <map>
#include <string>
#include "actor.h"
#include "sprite.h"
#include "camera.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_engine_studio/utility_objects/controllable_camera.h"

namespace ufo{

void GenericGenerator::Initialise(){
	factory_map.emplace(
        "Actor",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsMap().at("value")->AsFloat();
            float _y = _json->map.at("y")->AsMap().at("value")->AsFloat();
            auto instance = std::make_unique<Actor>(Vector2f(_x, _y));
            instance->editor_name = name;
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "Camera",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsMap().at("value")->AsFloat();
            float _y = _json->map.at("y")->AsMap().at("value")->AsFloat();
            auto instance = std::make_unique<Camera>(Vector2f(_x, _y));
            instance->editor_name = name;
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "ControllableCamera",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsMap().at("value")->AsFloat();
            float _y = _json->map.at("y")->AsMap().at("value")->AsFloat();
            auto instance = std::make_unique<ControllableCamera>(Vector2f(_x, _y));
            instance->editor_name = name;
            return std::move(instance);
        }
    );
    factory_map.emplace(
        "Sprite",
        [](ufo::gc::JsonMap* _json){
        	std::string _key = _json->map.at("key")->AsString();
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            float _offset_x = _json->map.at("offset_x")->AsFloat();
            float _offset_y = _json->map.at("offset_y")->AsFloat();
            float _frame_size_x = _json->map.at("frame_size_x")->AsFloat();
            float _frame_size_y = _json->map.at("frame_size_y")->AsFloat();
            float _scale_x = _json->map.at("scale_x")->AsFloat();
            float _scale_y = _json->map.at("scale_y")->AsFloat();
            float _rotation = _json->map.at("rotation")->AsFloat();
            int _frame_index = (int)_json->map.at("frame_index")->AsFloat();

            auto instance = std::make_unique<Sprite>(
            	_key,
            	Vector2f(_x, _y),
            	Vector2f(_offset_x, _offset_y),
            	Vector2f(_frame_size_x, _frame_size_y),
            	Vector2f(_scale_x, _scale_y), _rotation, _frame_index);
            return std::move(instance);
        }
        );
}

std::unique_ptr<Actor> GenericGenerator::FromJsonInGame(ufo::gc::JsonMap* _json){
	if(factory_map.count(_json->map.at("class_name")->AsString())){
	    std::unique_ptr<Actor> instance = factory_map.at(_json->map.at("class_name")->AsString())(_json);

		instance->class_name = _json->map.at("class_name")->AsString();

		auto custom_properties = _json->map.at("custom_editor_properties")->AsArray();

	    return std::move(instance);
	}
	else{
	    Console::PrintLine("std::unique_ptr<Actor> GenericGenerator::FromJson: Could not find type",_json->map.at("class_name")->AsString());
					return std::move(factory_map.at("Actor")(_json));
	}
}

std::unique_ptr<Actor> GenericGenerator::FromJson(ufo::gc::JsonMap* _json){
	if(factory_map.count(_json->map.at("base_class_name")->AsString())){
	    std::unique_ptr<Actor> instance = factory_map.at(_json->map.at("base_class_name")->AsString())(_json);

		instance->class_name = _json->map.at("class_name")->AsString();

		auto custom_properties = _json->map.at("custom_editor_properties")->AsMap();

		Console::PrintLine("Was able to load custom properties at least once");

		for(const auto& [k,v] : custom_properties){
		    //Iterate through custom properties
			std::string hint = v->AsMap().at("hint")->AsString();
			if(hint == ""){

			}
			else if(hint == ""){

			}
			else if(hint == ""){

			}
			else if(hint == ""){

			}
			else{

			}
			}

	    return std::move(instance);
	}
	else{
	    Console::PrintLine("std::unique_ptr<Actor> GenericGenerator::FromJson: Could not find type",_json->map.at("base_class_name")->AsString());
					return std::move(factory_map.at("Actor")(_json));
	}
}


}
