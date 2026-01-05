#include <exception>
#include <memory>
#include <map>
#include <stdexcept>
#include <string>
#include "actor.h"
#include "animation.h"
#include "sprite.h"
#include "camera.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_engine_studio/utility_objects/controllable_camera.h"
#include "../tilemap/tile_map.h"
#include "text.h"
#include "widget.h"
#include "button.h"

namespace ufo{

void GenericGenerator::Initialise(){
	factory_map.emplace(
        "Actor",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<Actor>(Vector2f(_x, _y));
            instance->editor_name = name;
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "TileMap",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
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
        "Text",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::Text>(Vector2f(_x, _y));

            try{
                for(auto& [k,v] : _json->map.at("language_to_text")->AsMap()){
                    instance->language_to_text[k] = v->AsString();
                }
                instance->is_wrapping = (bool)_json->map.at("is_wrapping")->AsFloat();
                instance->editor_name = name;
            } catch(const std::exception& _error){
                Console::PrintLine("[UFO-Engine] GenericGenerator, Error finding attribute in json representing Text instance", _error.what());
            }
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "Button",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::Button>(Vector2f(_x, _y));
            instance->editor_name = name;

            try{
                for(auto& [k,v] : _json->map.at("language_to_text")->AsMap()){
                    instance->language_to_text[k] = v->AsString();
                }
                instance->is_wrapping = (bool)_json->map.at("is_wrapping")->AsFloat();
            } catch(const std::exception& _error){
                Console::PrintLine("[UFO-Engine] GenericGenerator, Error finding attribute in json representing Button instance", _error.what());
            }
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "Widget",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::Widget>(Vector2f(_x, _y));

            try{
                auto j_rectangle = _json->map.at("rectangle")->AsMap();
                float widget_x = j_rectangle.at("x")->AsFloat();
                float widget_y = j_rectangle.at("y")->AsFloat();
                float widget_w = j_rectangle.at("w")->AsFloat();
                float widget_h = j_rectangle.at("h")->AsFloat();
                instance->rectangle.position.x = widget_x;
                instance->rectangle.position.y = widget_y;
                instance->rectangle.size.x = widget_w;
                instance->rectangle.size.y = widget_h;
                instance->editor_name = name;

            } catch(const std::exception& _error){
                Console::PrintLine("[UFO-Engine] GenericGenerator, Error finding attribute in json representing Widget instance", _error.what());
            }
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "Camera",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<Camera>(Vector2f(_x, _y));
            instance->editor_name = name;

            try{
                instance->clamp = (bool)_json->map.at("clamp")->AsFloat();
                instance->scale = _json->map.at("scale")->AsFloat();

            } catch(const std::exception& _error){
                Console::PrintLine("[UFO-Engine] GenericGenerator: Could not find properties for json representing Camera instance", _error.what());
            }

            return std::move(instance);
        }
    );

    factory_map.emplace(
        "Level",
        [](ufo::gc::JsonMap* _json){
            std::string name = _json->map.at("name")->AsString();
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<Level>();
            instance->editor_name = name;

            try{
                float size_x = _json->map.at("size_x")->AsFloat();
                float size_y = _json->map.at("size_y")->AsFloat();
                instance->size.x = size_x;
                instance->size.y = size_y;
            } catch(const std::exception& _error){
                Console::PrintLine("[UFO-Engine] GenericGenerator: Could not find properties for json representing Level instance", _error.what());
            }

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
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ControllableCamera>(Vector2f(_x, _y));
            instance->editor_name = name;
            return std::move(instance);
        }
    );
    factory_map.emplace(
        "Sprite",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
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

            std::string name = _json->map.at("name")->AsString();

            auto instance = std::make_unique<Sprite>(
            	_key,
            	Vector2f(_x, _y),
            	Vector2f(_offset_x, _offset_y),
            	Vector2f(_frame_size_x, _frame_size_y),
            	Vector2f(_scale_x, _scale_y), _rotation, _frame_index);

            instance->editor_name = name;

            return std::move(instance);
        }
        );

        factory_map.emplace(
            "Animation",
            [](ufo::gc::JsonMap* _json){
                float _x = _json->map.at("x")->AsFloat();
                float _y = _json->map.at("y")->AsFloat();

                std::string name = _json->map.at("name")->AsString();

                auto instance = std::make_unique<Animation>(
                   	Vector2f(_x, _y));

                try{

                    for(const auto& j_costume : _json->map.at("costumes")->AsArray()){
                        Animation::Costume costume;
                        costume.key = j_costume->AsMap().at("key")->AsString();
                        costume.local_position.x = j_costume->AsMap().at("local_position_x")->AsFloat();
                        costume.local_position.y = j_costume->AsMap().at("local_position_y")->AsFloat();
                        costume.offset.x = j_costume->AsMap().at("offset_x")->AsFloat();
                        costume.offset.y = j_costume->AsMap().at("offset_y")->AsFloat();
                        costume.frame_size.x = j_costume->AsMap().at("frame_size_x")->AsFloat();
                        costume.frame_size.y = j_costume->AsMap().at("frame_size_y")->AsFloat();
                        costume.scale.x = j_costume->AsMap().at("scale_x")->AsFloat();
                        costume.scale.y = j_costume->AsMap().at("scale_y")->AsFloat();
                        costume.rotation = j_costume->AsMap().at("rotation")->AsFloat();
                        costume.frame_index = j_costume->AsMap().at("frame_index")->AsFloat();
                        costume.animation_speed = j_costume->AsMap().at("animation_speed")->AsFloat();

                        instance->costumes.emplace(costume.key, costume);
                    }
                    instance->key = _json->map.at("current_costume")->AsString();
                    instance->preview = _json->map.at("preview")->AsFloat();
                } catch(const std::exception& _error){
                    Console::PrintLine("[UFO-Engine] GenericGenerator: Could not find properties for json representing Animation instance");
                }

                instance->editor_name = name;

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
