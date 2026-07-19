#include <string>
#include "engine.h"
#include "openglv4_5_asset_manager.h"
#include "asset_json.h"
#include "../ufo_garbage_collector/gc_json.h"

namespace ufo{

void AssetJson::Read(const std::string& _path, const std::string& _opened_directory_path, OpenGLv4_5_AssetManager* _asset_manager, ufo::Engine* _engine){
    auto j = ufo::gc::JsonRead(&gc, _path);
    if(j->map.count("assets")){
        auto arr = j->map.at("assets")->AsArray();
        for(const auto& j_relative_texture_path : arr){
            const std::string relative_texture_path = j_relative_texture_path->AsString();
            Console::PrintLine("AssetJson::Read:",_opened_directory_path+"/"+relative_texture_path);
            _asset_manager->LoadTexture(_opened_directory_path+"/"+relative_texture_path,relative_texture_path,true);
            if(_asset_manager->textures.count(relative_texture_path)) _asset_manager->textures.at(relative_texture_path).permanent = true;
        }
    }
    if(j->map.count("shaders")){
        auto arr_shaders = j->map.at("shaders")->AsArray();
        for(const auto& j_relative_shader_path : arr_shaders){

            const std::string relative_shader_path = j_relative_shader_path->AsString();

            Console::PrintLine("AssetJson::Read:",_opened_directory_path+relative_shader_path);

            const std::string vertex_shader_path = _opened_directory_path+"/"+relative_shader_path+"/vertex.glsl";
            const std::string fragment_shader_path = _opened_directory_path+"/"+relative_shader_path+"/fragment.glsl";
            const std::string geometry_shader_path = _opened_directory_path+"/"+relative_shader_path+"/geometry.glsl"; //Unused for now

            //Here the first two characters are removed, which always have to be ..
            // would be simpler if the default path was just the project root after all
            _asset_manager->LoadShader(vertex_shader_path.c_str(),fragment_shader_path.c_str(), nullptr, relative_shader_path);

            //If the shader doesn't exist, continue
            if(!_asset_manager->shaders.count(relative_shader_path)) continue;

            glm::mat4 projection = glm::ortho(
                0.0f, static_cast<float>(_engine->width),
                static_cast<float>(_engine->height), 0.0f,
                -1.0f, 0.0f
            );

            _asset_manager->GetShader(relative_shader_path).Use();
            _asset_manager->GetShader(relative_shader_path).SetInt("image", 0);
            _asset_manager->GetShader(relative_shader_path).SetMatrix4("projection", projection);
        }
    }
}

void AssetJson::ReadEditor(const std::string& _path, const std::string& _opened_directory_path, OpenGLv4_5_AssetManager* _asset_manager, ufo::Engine* _engine){
    auto j = ufo::gc::JsonRead(&gc, _path);

    if(j->map.count("assets")){
        auto arr_textures = j->map.at("assets")->AsArray();
        for(const auto& j_relative_texture_path : arr_textures){

            const std::string relative_texture_path = j_relative_texture_path->AsString();

            Console::PrintLine("AssetJson::Read:",relative_texture_path);

            //Here the first two characters are removed, which always have to be ..
            // would be simpler if the default path was just the project root after all
            _asset_manager->LoadTexture(_opened_directory_path+"/"+relative_texture_path,relative_texture_path,true);
            if(_asset_manager->textures.count(relative_texture_path)) _asset_manager->textures.at(relative_texture_path).permanent = true;
        }
    }

    if(j->map.count("shaders")){
        auto arr_shaders = j->map.at("shaders")->AsArray();
        for(const auto& j_relative_shader_path : arr_shaders){

            const std::string relative_shader_path = j_relative_shader_path->AsString();

            Console::PrintLine("AssetJson::Read: Loading Shader",_opened_directory_path+relative_shader_path);

            const std::string vertex_shader_path = _opened_directory_path+"/"+relative_shader_path+"/vertex.glsl";
            const std::string fragment_shader_path = _opened_directory_path+"/"+relative_shader_path+"/fragment.glsl";
            const std::string geometry_shader_path = _opened_directory_path+"/"+relative_shader_path+"/geometry.glsl"; //Unused for now

            //Here the first two characters are removed, which always have to be ..
            // would be simpler if the default path was just the project root after all
            _asset_manager->LoadShader(vertex_shader_path.c_str(),fragment_shader_path.c_str(), nullptr, relative_shader_path);

            //If the shader doesn't exist, continue
            if(!_asset_manager->shaders.count(relative_shader_path)) continue;

            glm::mat4 projection = glm::ortho(
                0.0f, static_cast<float>(_engine->width),
                static_cast<float>(_engine->height), 0.0f,
                -1.0f, 0.0f
            );

            _asset_manager->GetShader(relative_shader_path).Use();
            _asset_manager->GetShader(relative_shader_path).SetInt("image", 0);
            _asset_manager->GetShader(relative_shader_path).SetMatrix4("projection", projection);
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
