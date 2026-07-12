#include <string>
#include "engine.h"
#include "openglv4_5_asset_manager.h"
#include "asset_json.h"
#include "../ufo_garbage_collector/gc_json.h"

namespace ufo{

void AssetJson::Read(const std::string& _path, const std::string& _opened_directory_path, OpenGLv4_5_AssetManager* _asset_manager, ufo::Engine* _engine){
    auto j = ufo::gc::JsonRead(&gc, _path);
    auto arr = j->map.at("assets")->AsArray();
    for(const auto& a : arr){
        Console::PrintLine("AssetJson::Read:",_opened_directory_path+"/"+a->AsString());
        _asset_manager->LoadTexture(_opened_directory_path+"/"+a->AsString(),a->AsString(),true);
        _asset_manager->textures.at(a->AsString()).permanent = true;
    }
    if(j->map.count("shaders")){
        auto arr_shaders = j->map.at("shaders")->AsArray();
        for(const auto& a : arr_shaders){
            Console::PrintLine("AssetJson::Read:",_opened_directory_path+a->AsString());

            const std::string vertex_shader_path = _opened_directory_path+"/"+a->AsString()+".vertex.cs";
            const std::string fragment_shader_path = _opened_directory_path+"/"+a->AsString()+".fragment.cs";
            const std::string geometry_shader_path = _opened_directory_path+"/"+a->AsString()+".geometry.cs"; //Unused for now

            //Here the first two characters are removed, which always have to be ..
            // would be simpler if the default path was just the project root after all
            _asset_manager->LoadShader(vertex_shader_path.c_str(),fragment_shader_path.c_str(), nullptr, a->AsString());

            glm::mat4 projection = glm::ortho(
                0.0f, static_cast<float>(_engine->width),
                static_cast<float>(_engine->height), 0.0f,
                -1.0f, 0.0f
            );

            _asset_manager->GetShader(a->AsString()).Use();
            _asset_manager->GetShader(a->AsString()).SetInt("image", 0);
            _asset_manager->GetShader(a->AsString()).SetMatrix4("projection", projection);
        }
    }
}

void AssetJson::ReadEditor(const std::string& _path, const std::string& _opened_directory_path, OpenGLv4_5_AssetManager* _asset_manager, ufo::Engine* _engine){
    auto j = ufo::gc::JsonRead(&gc, _path);

    if(j->map.count("assets")){
        auto arr_textures = j->map.at("assets")->AsArray();
        for(const auto& a : arr_textures){
            Console::PrintLine("AssetJson::Read:",_opened_directory_path+a->AsString());

            //Here the first two characters are removed, which always have to be ..
            // would be simpler if the default path was just the project root after all
            _asset_manager->LoadTexture(_opened_directory_path+"/"+a->AsString(),a->AsString(),true);
            if(_asset_manager->textures.count(a->AsString())) _asset_manager->textures.at(a->AsString()).permanent = true;
        }
    }

    if(j->map.count("shaders")){
        auto arr_shaders = j->map.at("shaders")->AsArray();
        for(const auto& a : arr_shaders){
            Console::PrintLine("AssetJson::Read:",_opened_directory_path+a->AsString());

            const std::string vertex_shader_path = _opened_directory_path+"/"+a->AsString()+".vertex.cs";
            const std::string fragment_shader_path = _opened_directory_path+"/"+a->AsString()+".fragment.cs";
            const std::string geometry_shader_path = _opened_directory_path+"/"+a->AsString()+".geometry.cs"; //Unused for now

            //Here the first two characters are removed, which always have to be ..
            // would be simpler if the default path was just the project root after all
            _asset_manager->LoadShader(vertex_shader_path.c_str(),fragment_shader_path.c_str(), nullptr, a->AsString());

            glm::mat4 projection = glm::ortho(
                0.0f, static_cast<float>(_engine->width),
                static_cast<float>(_engine->height), 0.0f,
                -1.0f, 0.0f
            );

            _asset_manager->GetShader(a->AsString()).Use();
            _asset_manager->GetShader(a->AsString()).SetInt("image", 0);
            _asset_manager->GetShader(a->AsString()).SetMatrix4("projection", projection);
        }
    }
}

void AssetJson::Write(OpenGLv4_5_AssetManager* _asset_manager){
    auto j = gc.New<ufo::gc::JsonMap>();

    auto texture_arr = gc.New<ufo::gc::JsonArray>();

    for(const auto& [k,v] : _asset_manager->textures){
        if(v.permanent) texture_arr->array.push_back(gc.New<ufo::gc::JsonString>(k));
    }

    j->map.emplace("assets", texture_arr);


    auto shader_arr = gc.New<ufo::gc::JsonArray>();

    for(const auto& [k,v] : _asset_manager->shaders){
        if(!v.permanent) shader_arr->array.push_back(gc.New<ufo::gc::JsonString>(k));
    }

    j->map.emplace("shaders", shader_arr);

    j->Write(_asset_manager->save_path);
}

}
