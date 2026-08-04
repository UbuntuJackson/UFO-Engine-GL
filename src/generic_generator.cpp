#include "generic_generator.h"
#include <exception>
#include <memory>
#include <map>
#include <stdexcept>
#include <string>
#include "actor.h"
#include "animation.h"
#include "console.h"
#include "file_utils.h"
#include "sprite.h"
#include "camera.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../tilemap/tile_map.h"
#include "text.h"
#include "ufo_macros.h"
#include "../ufo_maths/ufo_maths.h"
#include "widget.h"
#include "button.h"
#include "platformer_rectangle_collision.h"
#include "collision_grid.h"
#include "background_sprite.h"
#include "rectangular_area.h"
#include "animation_cluster.h"

#ifdef UFO_ENGINE_STUDIO
#include "../src/editor_property.h"
#include "../ufo_engine_studio/utility_objects/controllable_camera.h"
#endif //UFO_ENGINE_STUDIO

namespace ufo{

std::string GenericGenerator::GetBaseClassOf(std::string _class_name){
    if(!inheritence_map.count(_class_name)){
        return _class_name;
    }

    return GetBaseClassOf(inheritence_map.at(_class_name));
}

void GenericGenerator::InitialiseActorClassJsons(const std::string& _game_directory){

    actor_jsons_with_unaltered_default_properties.clear();
    inheritence_map.clear();

    const std::string structured_classes_full_path = _game_directory+"/structured_classes.json";

    if(!FileSystem::FileExists(structured_classes_full_path)) return;

    ufo::gc::JsonMap* j_structured_classes = gc::JsonRead(&gc, structured_classes_full_path);

    if(j_structured_classes->IsNull()) Console::PrintLine(__UFO_PRETTY_FUNCTION__, "j_structured_classes is invalid");

    if(!j_structured_classes->AsMap().count("contents")){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "j_structured_classes does not contain contents");
    }

    auto structured_classes_contents = j_structured_classes->AsMap().at("contents")->AsArray();

    for(const auto& entry : structured_classes_contents){
        std::string class_name = entry->AsMap().at("class")->AsMap().at("name")->AsString();

        const auto extends_classes = entry->AsMap().at("class")->AsMap().at("extends")->AsArray();

        if(extends_classes.size() > 0){
            std::string extends_class = extends_classes[0]->AsString();
            inheritence_map.emplace(class_name,extends_class);
        }

        for(const auto& mac : entry->AsMap().at("macros")->AsArray()){
            if(mac->AsMap().at("name")->AsString() == "ufo_actor_config"){
                const auto args = mac->AsMap().at("args")->AsArray();

                if(args.size() == 1){
                    std::string actor_config_path = args[0]->AsString();

                    if(ufo::FileSystem::FileExists(_game_directory+"/"+actor_config_path)){

                        const auto& actor_config_json = gc::JsonRead(&gc, _game_directory+"/"+actor_config_path)->AsMap().at("actors")->AsArray();

                        for(const auto actor_json : actor_config_json){
                            if(actor_json->AsMap().at("name")->AsString() == "Main"){
                                actor_jsons_with_unaltered_default_properties.emplace(class_name, actor_json->AsJsonMap());
                                Console::PrintLine("Found class",class_name,actor_config_path);
                            }
                        }
                    }

                }
            }
        }
    }
}

void GenericGenerator::Initialise(ufo::Engine* _engine){

    engine = _engine;

    //Gotta add all the ufo classes to class_jsons

    //Also gotta add all .asons to actor_jsons_with_unaltered_default_properties

	factory_map.emplace(
        "ufo::Actor",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<Actor>(Vector2f(_x, _y));
            return instance;
        }
    );

	factory_map.emplace(
        "ufo::AnimationCluster",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::AnimationCluster>(Vector2f(_x, _y));
            return instance;
        }
    );

    factory_map.emplace(
        "ufo::CollisionGrid",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::CollisionGrid>(Vector2f(_x, _y));
            return instance;
        }
    );

    factory_map.emplace(
        "ufo::PlatformerRectangleCollision",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::PlatformerRectangleCollision>(Vector2f(_x, _y));
            return instance;
        }
    );

    factory_map.emplace(
        "ufo::TileMap",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::TileMap>(Vector2f(_x, _y));

            try{
                instance->OnLoadDefaultProperties(_json);

            } catch(const std::exception& _error){
                Console::PrintLine(__UFO_PRETTY_FUNCTION__,"Error finding attribute in json representing TileMap instance", _error.what());
            }
            return instance;
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
                Console::PrintLine(__UFO_PRETTY_FUNCTION__,"Error finding attribute in json representing Text instance", _error.what());
            }
            return instance;
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
                Console::PrintLine(__UFO_PRETTY_FUNCTION__,"Error finding attribute in json representing Button instance", _error.what());
            }
            return instance;
        }
    );

    factory_map.emplace(
        "ufo::Widget",
        [](ufo::gc::JsonMap* _json){

            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<ufo::Widget>(Vector2f(_x, _y));

            return instance;
        }
    );

    factory_map.emplace(
        "ufo::Camera",
        [](ufo::gc::JsonMap* _json){

            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<Camera>(Vector2f(_x, _y));

            return instance;
        }
    );

    factory_map.emplace(
        "ufo::Level",
        [](ufo::gc::JsonMap* _json){

            auto instance = std::make_unique<Level>();

            return instance;
        }
    );

#ifdef UFO_ENGINE_STUDIO
    factory_map.emplace(
        "ControllableCamera",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();
            auto instance = std::make_unique<UFOEngineStudio::ControllableCamera>(Vector2f(_x, _y));
            return instance;
        }
    );
#endif //UFO_ENGINE_STUDIO

    factory_map.emplace(
        "ufo::Sprite",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();

            auto instance = std::make_unique<Sprite>(
            	Vector2f(_x, _y));

            return instance;
        }
        );

    factory_map.emplace(
        "ufo::Animation",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();

            auto instance = std::make_unique<Animation>(
               	Vector2f(_x, _y));

            return instance;
        }
    );

    factory_map.emplace(
        "ufo::BackgroundSprite",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();

            auto instance = std::make_unique<ufo::BackgroundSprite>(
               	Vector2f(_x, _y));

            return instance;
        }
    );

    factory_map.emplace(
        "ufo::RectangularArea",
        [](ufo::gc::JsonMap* _json){
            float _x = _json->map.at("x")->AsFloat();
            float _y = _json->map.at("y")->AsFloat();

            auto instance = std::make_unique<ufo::RectangularArea>(
               	Vector2f(_x, _y));

            return instance;
        }
    );
}

std::unique_ptr<Actor> GenericGenerator::JsonToActorTree([[maybe_unused]] ufo::GarbageCollector* _gc, ufo::gc::JsonMap* _json){

    std::unique_ptr<Actor> actor = FromJson(_json);

    return actor;

}

#ifdef UFO_ENGINE_STUDIO

//Not done yet
std::unique_ptr<Actor> GenericGenerator::SpawnAtRuntime(const std::string& _class_name, Vector2f _local_position){
    if(factory_map_runtime.count(GetBaseClassOf(_class_name))){

	    std::unique_ptr<Actor> instance = factory_map_runtime.at(GetBaseClassOf(_class_name))(_local_position);

		return instance;
    }


    throw;
}

std::unique_ptr<Actor> GenericGenerator::FromJson(ufo::gc::JsonMap* _json){
    std::string class_name = _json->map.at("class_name")->AsString();
	std::string editor_name = _json->map.at("name")->AsString();

	if(factory_map.count(GetBaseClassOf(class_name))){
	    //float local_position_x = _json->map.at("x")->AsFloat();
		//float local_position_y = _json->map.at("y")->AsFloat();

	    std::unique_ptr<Actor> instance = factory_map.at(GetBaseClassOf(class_name))(_json);

		instance->class_name = class_name;
		instance->editor_name = editor_name;

		bool is_custom_class = class_name != GetBaseClassOf(class_name);

		instance->import_mode = is_custom_class ? Actor::ImportModes::CUSTOM_CLASS : Actor::ImportModes::BUILT_IN_CLASS;

		if(is_custom_class && !instance->is_editor_hit_box_unique_per_instance){

            if(actor_jsons_with_unaltered_default_properties.count(instance->class_name)){
                ufo::gc::JsonMap* class_json = actor_jsons_with_unaltered_default_properties.at(instance->class_name);
                if(class_json->map.count("editor_hitbox")){
                    auto j_editor_hitbox = class_json->map.at("editor_hitbox")->AsMap();
                    instance->editor_hitbox = ufo::Rectangle(
                        Vector2f(j_editor_hitbox.at("x")->AsFloat(),
                                 j_editor_hitbox.at("y")->AsFloat()),
                        Vector2f(j_editor_hitbox.at("width")->AsFloat(),
                                 j_editor_hitbox.at("height")->AsFloat()
                        ));
                }
            }

		}

		instance->engine = engine;

		instance->OnLoadDefaultProperties(_json);

		if(instance->import_mode == Actor::ImportModes::BUILT_IN_CLASS){
            for(ufo::gc::Json* j : _json->AsMap().at("actors")->AsArray()){
                auto j_map = dynamic_cast<ufo::gc::JsonMap*>(j);

                if(j_map) instance->AddActorUniquePtr(FromJson(j->AsJsonMap()));
                else Console::PrintLine("std::unique_ptr<Actor> Actor::FromJsonToActor: Non-JsonMap item found in json");
            }
		}
		if(_json->map.count("custom_editor_properties")){
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
    								__UFO_PRETTY_FUNCTION__,"Failed loading custom property of type EditorPropertyCheckBox of type",
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
                    				__UFO_PRETTY_FUNCTION__,"Failed at loading custom property of type EditorPropertyInt of type",
                    				instance->class_name, "and name",
                    				instance->editor_name);
                    }
    			}
    			else if(hint == "EditorPropertyFloat"){
                    try{
         			    std::string name = v->AsMap().at("name")->AsString();
        				float value = v->AsMap().at("value")->AsFloat();

                        instance->editor_properties.push_back(std::make_unique<ufo::EditorPropertyFloat>(name, name, value));
                    } catch(const std::exception& _error){
                        Console::PrintLine(
                        				__UFO_PRETTY_FUNCTION__,"Failed at loading custom property of type EditorPropertyFloat of type",
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
                    				__UFO_PRETTY_FUNCTION__,"Failed at loading custom property of type EditorPropertyFloat of type",
                    				instance->class_name, "and name",
                    				instance->editor_name);
                    }
    			}
    			else{

    			}
    		}
		}

	    return instance;
	}
	else{
	    Console::PrintLine("std::unique_ptr<Actor> GenericGenerator::FromJson: Could not find type",class_name,"with base type",GetBaseClassOf(class_name));

		return std::make_unique<ufo::Actor>(Vector2f(0.0f, 0.0f));
	}
}
#else

    std::unique_ptr<Actor> GenericGenerator::FromJson(ufo::gc::JsonMap* _json){

        //Stupid ass placeholder

        return std::make_unique<Actor>(Vector2f(0.0f, 0.0f));

    }

#endif

std::unique_ptr<Actor> GenericGenerator::FromJsonInGame(ufo::gc::JsonMap* _json){
	if(factory_map.count(_json->map.at("class_name")->AsString())){
	    std::unique_ptr<Actor> instance = factory_map.at(_json->map.at("class_name")->AsString())(_json);

		instance->class_name = _json->map.at("class_name")->AsString();
		instance->editor_name = _json->AsMap().at("name")->AsString();
		//instance->import_mode = _json->map.at("import_mode")->AsFloat();

		try{
		    if(instance->class_name != instance->base_class_name){
          		ufo::gc::JsonMap* class_json = actor_jsons_with_unaltered_default_properties.at(instance->class_name);

                instance->engine = engine;
          		instance->OnLoadDefaultProperties(class_json);

                for(const auto& j_actor : class_json->AsMap().at("actors")->AsArray()){
                    instance->AddActorUniquePtr(FromJsonInGame(j_actor->AsJsonMap()));
                }
			}
    	    else{
                instance->engine = engine;
            	instance->OnLoadDefaultProperties(_json);
                for(const auto& j_actor : _json->AsMap().at("actors")->AsArray()){
                    instance->AddActorUniquePtr(FromJsonInGame(j_actor->AsJsonMap()));
                }
    		}

		}
		catch(const std::exception& _error){
            Console::PrintLine("std::unique_ptr<Actor> GenericGenerator::FromJson:","Could not find class json for class",instance->class_name);
		}

	    return instance;
	}
	else{
	    Console::PrintLine("std::unique_ptr<Actor> GenericGenerator::FromJson: Could not find type",_json->map.at("class_name")->AsString());
					return factory_map.at("ufo::Actor")(_json);
	}
}


}
