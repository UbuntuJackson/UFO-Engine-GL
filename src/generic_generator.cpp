#include "generic_generator.h"
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
#include "actor_component_loader.h"
#include "platformer_rectangle_collision.h"
#include "collision_grid.h"
#include "../src/editor_property.h"
#include "background_sprite.h"

namespace ufo{

void GenericGenerator::Initialise(){
	factory_map.emplace(
        "ufo::Actor",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<Actor>(Vector2f(_x, _y));
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "ufo::CollisionGrid",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::CollisionGrid>(Vector2f(_x, _y));
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "ufo::PlatformerRectangleCollision",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::PlatformerRectangleCollision>(Vector2f(_x, _y));
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "ufo::TileMap",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::TileMap>(Vector2f(_x, _y));

            try{
                auto tiles = _json->map.at("tiles")->AsArray();
                instance->tilemap_data.clear();
                instance->tilemap_data.reserve(tiles.size());

                for(const auto& tile : tiles){
                    instance->tilemap_data.push_back((int)tile->AsFloat());
                }

                instance->number_of_columns = (int)_json->map.at("number_of_columns")->AsFloat();
                instance->number_of_rows = (int)_json->map.at("number_of_rows")->AsFloat();

                instance->visible = (int)_json->map.at("visible")->AsFloat();

            } catch(const std::exception& _error){
                Console::PrintLine("[UFO-Engine] GenericGenerator, Error finding attribute in json representing TileMap instance", _error.what());
            }
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "ufo::Text",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::Text>(Vector2f(_x, _y));

            try{
                for(auto& [k,v] : _json->map.at("language_to_text")->AsMap()){
                    instance->language_to_text[k] = v->AsString();
                }
                instance->is_wrapping = (bool)_json->map.at("is_wrapping")->AsFloat();
            } catch(const std::exception& _error){
                Console::PrintLine("[UFO-Engine] GenericGenerator, Error finding attribute in json representing Text instance", _error.what());
            }
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "ufo::Button",
        [](ufo::gc::JsonMap* _json){

            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::Button>(Vector2f(_x, _y));

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
        "ufo::Widget",
        [](ufo::gc::JsonMap* _json){

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


            } catch(const std::exception& _error){
                Console::PrintLine("[UFO-Engine] GenericGenerator, Error finding attribute in json representing Widget instance", _error.what());
            }
            return std::move(instance);
        }
    );

    factory_map.emplace(
        "ufo::Camera",
        [](ufo::gc::JsonMap* _json){

            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<Camera>(Vector2f(_x, _y));

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
        "ufo::Level",
        [](ufo::gc::JsonMap* _json){

            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<Level>();

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
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<UFOEngineStudio::ControllableCamera>(Vector2f(_x, _y));
            return std::move(instance);
        }
    );
    factory_map.emplace(
        "ufo::Sprite",
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

            auto instance = std::make_unique<Sprite>(
            	_key,
            	Vector2f(_x, _y),
            	Vector2f(_offset_x, _offset_y),
            	Vector2f(_frame_size_x, _frame_size_y),
            	Vector2f(_scale_x, _scale_y), _rotation, _frame_index);

            return std::move(instance);
        }
        );

        factory_map.emplace(
            "ufo::Animation",
            [](ufo::gc::JsonMap* _json){
                float _x = _json->map.at("x")->AsFloat();
                float _y = _json->map.at("y")->AsFloat();

                auto instance = std::make_unique<Animation>(
                   	Vector2f(_x, _y));

                return std::move(instance);
            }
        );

        factory_map.emplace(
            "ufo::BackgroundSprite",
            [](ufo::gc::JsonMap* _json){
                float _x = _json->map.at("x")->AsFloat();
                float _y = _json->map.at("y")->AsFloat();

                auto instance = std::make_unique<ufo::BackgroundSprite>(
                   	Vector2f(_x, _y));

                return std::move(instance);
            }
        );
}

std::unique_ptr<Actor> GenericGenerator::FromJsonInGame(ufo::gc::JsonMap* _json){
	if(factory_map.count(_json->map.at("class_name")->AsString())){
	    std::unique_ptr<Actor> instance = factory_map.at(_json->map.at("class_name")->AsString())(_json);

		instance->class_name = _json->map.at("class_name")->AsString();
		instance->editor_name = _json->AsMap().at("name")->AsString();

		instance->OnLoadDefaultProperties(_json);

	    return std::move(instance);
	}
	else{
	    Console::PrintLine("std::unique_ptr<Actor> GenericGenerator::FromJson: Could not find type",_json->map.at("class_name")->AsString());
					return std::move(factory_map.at("Actor")(_json));
	}
}

std::unique_ptr<Actor> GenericGenerator::JsonToActorTree(ufo::GarbageCollector* _gc, ufo::gc::JsonMap* _json){

    std::unique_ptr<Actor> actor = FromJson(_json);

    for(ufo::gc::Json* j : _json->AsMap().at("actors")->AsArray()){
        auto j_map = dynamic_cast<ufo::gc::JsonMap*>(j);

        if(j_map) actor->AddActorUniquePtr(JsonToActorTree(_gc, j_map));
        else Console::PrintLine("std::unique_ptr<Actor> Actor::FromJsonToActor: Non-JsonMap item found in json");
    }

    OnJsonToActorTree(actor.get(), _json);

    return std::move(actor);

}

void GenericGenerator::JsonToActorTreeInGameComponentLoad(Actor* _actor, ufo::gc::JsonMap* _json){
    try{

		_actor->import_mode = _json->map.at("import_mode")->AsFloat();
		if(_actor->import_mode == Actor::ImportModes::WRAPPED){
                Console::PrintLine("new actor queue size before whiping",_actor->new_actor_queue.size(), _actor->actors.size());
                _actor->new_actor_queue.clear();

		    //Importing components instead of the entire actor.
			ActorComponentLoader actor_component_loader;
			actor_component_loader.Load(this, _actor);

		}

	}catch(const std::exception& _error){
	    Console::PrintLine("[UFO-Engine] GenericGenerator::FromJsonInGame: Could not find data 'import_mode'");
	}
}

std::unique_ptr<Actor> GenericGenerator::FromJson(ufo::gc::JsonMap* _json){
	if(factory_map.count(_json->map.at("base_class_name")->AsString())){
	    //float local_position_x = _json->map.at("x")->AsFloat();
		//float local_position_y = _json->map.at("y")->AsFloat();

	    std::unique_ptr<Actor> instance = factory_map.at(_json->map.at("base_class_name")->AsString())(_json);

		instance->class_name = _json->map.at("class_name")->AsString();
		instance->editor_name = _json->map.at("name")->AsString();
		try{
		    instance->is_imported = (bool)_json->map.at("is_imported")->AsFloat();
			instance->import_mode = _json->map.at("import_mode")->AsFloat();
		}catch(const std::exception& _error){
		    Console::PrintLine("[UFO-Engine] GenericGenerator::FromJson: Could not find data 'is_imported'");
		}
		instance->OnLoadDefaultProperties(_json);

		auto custom_properties = _json->map.at("custom_editor_properties")->AsMap();

		for(const auto& [k,v] : custom_properties){
		    //Iterate through custom properties

			if(!v->AsMap().count("hint")) continue;

			std::string hint = v->AsMap().at("hint")->AsString();
			if(hint == "EditorPropertyCheckBox"){

			    try{
    			    std::string name = v->AsMap().at("name")->AsString();
    				bool value = (bool)v->AsMap().at("value")->AsFloat();

                    instance->editor_properties.push_back(std::make_unique<ufo::EditorPropertyCheckBox>(name, name, value));
				} catch(const std::exception& _error){
				    Console::PrintLine(
								"[UFO-Engine] GenericGenerator::FromJson: Failed at loading custom property of type EditorPropertyCheckBox of type",
								instance->class_name, "and name",
								instance->editor_name);
				}
			}
			else if(hint == "EditorPropertyInt"){
                try{
     			    std::string name = v->AsMap().at("name")->AsString();
    				int value = v->AsMap().at("value")->AsFloat();

                    instance->editor_properties.push_back(std::make_unique<ufo::EditorPropertyInt>(name, name, value));
                } catch(const std::exception& _error){
                    Console::PrintLine(
                				"[UFO-Engine] GenericGenerator::FromJson: Failed at loading custom property of type EditorPropertyInt of type",
                				instance->class_name, "and name",
                				instance->editor_name);
                }
			}
			/*else if(hint == "EditorPropertyFloatSlider"){
                try{
     			    std::string name = v->AsMap().at("name")->AsString();
    				int value = v->AsMap().at("value")->AsFloat();

                    instance->editor_properties.push_back(std::make_unique<Actor::EditorPropertyFloatSlider>(name, name, value));
                } catch(const std::exception& _error){
                    Console::PrintLine(
                				"[UFO-Engine] GenericGenerator::FromJson: Failed at loading custom property of type EditorPropertyInt of type",
                				instance->class_name, "and name",
                				instance->editor_name);
                }
                }*/
			else if(hint == "EditorPropertyFloat"){
                try{
     			    std::string name = v->AsMap().at("name")->AsString();
    				float value = v->AsMap().at("value")->AsFloat();

                    instance->editor_properties.push_back(std::make_unique<ufo::EditorPropertyFloat>(name, name, value));
                } catch(const std::exception& _error){
                    Console::PrintLine(
                				"[UFO-Engine] GenericGenerator::FromJson: Failed at loading custom property of type EditorPropertyFloat of type",
                				instance->class_name, "and name",
                				instance->editor_name);
                }
			}
			else if(hint == "EditorPropertyString"){
                try{
     			    std::string name = v->AsMap().at("name")->AsString();
    				std::string value = v->AsMap().at("value")->AsString();

                    instance->editor_properties.push_back(std::make_unique<ufo::EditorPropertyString>(name, name, value));
                } catch(const std::exception& _error){
                    Console::PrintLine(
                				"[UFO-Engine] GenericGenerator::FromJson: Failed at loading custom property of type EditorPropertyFloat of type",
                				instance->class_name, "and name",
                				instance->editor_name);
                }
			}
			else{

			}
		}

	    return std::move(instance);
	}
	else{
	    Console::PrintLine("std::unique_ptr<Actor> GenericGenerator::FromJson: Could not find type",_json->map.at("base_class_name")->AsString());
					return std::move(factory_map.at("ufo::Actor")(_json));
	}
}


}
