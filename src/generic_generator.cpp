#include <exception>
#include <memory>
#include <map>
#include <stdexcept>
#include <string>
#include "actor.h"
#include "sprite.h"
#include "camera.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_engine_studio/utility_objects/controllable_camera.h"
#include "../tilemap/tile_map.h"

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
        "TileMap",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsMap().at("value")->AsFloat();
            float _y = _json->map.at("y")->AsMap().at("value")->AsFloat();
            auto instance = std::make_unique<TileMap>(Vector2f(_x, _y));

            try{
                auto tiles = _json->map.at("tiles")->AsArray();
                instance->tilemap_data.clear();
                instance->tilemap_data.reserve(tiles.size());

                for(const auto& tile : tiles){
                    instance->tilemap_data.push_back((int)tile->AsFloat());
                }

                instance->number_of_columns = (int)_json->map.at("number_of_columns")->AsFloat();
                instance->number_of_rows = (int)_json->map.at("number_of_rows")->AsFloat();

                instance->editor_name = name;
            } catch(const std::exception& _error){
                Console::PrintLine("[UFO-Engine] GenericGenerator, Error finding attribute in json representing TileMap instance", _error.what());
            }
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
        "Level",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsMap().at("value")->AsFloat();
            float _y = _json->map.at("y")->AsMap().at("value")->AsFloat();
            auto instance = std::make_unique<Level>();
            instance->editor_name = name;

            ufo::gc::Json* j_tilesets = _json->map.at("tilesets");
            for(const auto& j_tileset : j_tilesets->AsArray()){

                instance->tileset_manager.tileset_data.push_back(
                    TilesetData{
                        j_tileset->AsMap().at("name")->AsString(),
                        (int)j_tileset->AsMap().at("columns")->AsFloat(),
                        (int)j_tileset->AsMap().at("tileset_start_id")->AsFloat(),
                        j_tileset->AsMap().at("image_width")->AsFloat(),
                        j_tileset->AsMap().at("image_height")->AsFloat(),
                        j_tileset->AsMap().at("tile_width")->AsFloat(),
                        j_tileset->AsMap().at("tile_height")->AsFloat(),
                        (int)j_tileset->AsMap().at("tile_count")->AsFloat()
                    }
                );
            }

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
            float _x = _json->map.at("x")->AsMap().at("value")->AsFloat();
            float _y = _json->map.at("y")->AsMap().at("value")->AsFloat();
            std::string _key = "placeholder_icon";
            float _offset_x = 0.0f;
            float _offset_y = 0.0f;
            float _frame_size_x = 32.0f;
            float _frame_size_y = 32.0f;
            float _scale_x = 0.0f;
            float _scale_y = 0.0f;
            float _rotation = 0.0f;
            int _frame_index = 0;
            try{
                _key = _json->map.at("key")->AsString();
                _offset_x = _json->map.at("offset_x")->AsFloat();
                _offset_y = _json->map.at("offset_y")->AsFloat();
                _frame_size_x = _json->map.at("frame_size_x")->AsFloat();
                _frame_size_y = _json->map.at("frame_size_y")->AsFloat();
                _scale_x = _json->map.at("scale_x")->AsFloat();
                _scale_y = _json->map.at("scale_y")->AsFloat();
                _rotation = _json->map.at("rotation")->AsFloat();
                _frame_index = (int)_json->map.at("frame_index")->AsFloat();
            } catch(const std::exception& _error){
                Console::PrintLine("[UFO-Engine] GenericGenerator: Could not find properties for json representing Sprite instance");
            }

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
		instance->editor_name = _json->AsMap().at("name")->AsString();

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
		try{
		    instance->is_imported = (bool)_json->map.at("is_imported")->AsFloat();
		}catch(const std::exception& _error){
		    Console::PrintLine("[UFO-Engine] GenericGenerator::FromJson: Could not find data 'is_imported'");
		}

		auto custom_properties = _json->map.at("custom_editor_properties")->AsMap();

		Console::PrintLine("Was able to load custom properties at least once");

		for(const auto& [k,v] : custom_properties){
		    //Iterate through custom properties

			if(!v->AsMap().count("hint")) continue;

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
