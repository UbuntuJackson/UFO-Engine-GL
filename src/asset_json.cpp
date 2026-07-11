#include <string>
#include "openglv4_5_asset_manager.h"
#include "asset_json.h"
#include "../ufo_garbage_collector/gc_json.h"

void AssetJson::Read(const std::string& _path, const std::string& _opened_directory_path, OpenGLv4_5_AssetManager* _asset_manager){
    auto j = ufo::gc::JsonRead(&gc, _path);
    auto arr = j->map.at("assets")->AsArray();
    for(const auto& a : arr){
        Console::PrintLine("AssetJson::Read:",_opened_directory_path+"/"+a->AsString());
        _asset_manager->LoadTexture(_opened_directory_path+"/"+a->AsString(),a->AsString(),true);
        _asset_manager->textures.at(a->AsString()).permanent = true;
    }
}

void AssetJson::ReadEditor(const std::string& _path, const std::string& _opened_directory_path, OpenGLv4_5_AssetManager* _asset_manager){
    auto j = ufo::gc::JsonRead(&gc, _path);
    auto arr = j->map.at("assets")->AsArray();
    for(const auto& a : arr){
        Console::PrintLine("AssetJson::Read:",_opened_directory_path+a->AsString());

        //Here the first two characters are removed, which always have to be ..
        // would be simpler if the default path was just the project root after all
        _asset_manager->LoadTexture(_opened_directory_path+"/"+a->AsString(),a->AsString(),true);
        if(_asset_manager->textures.count(a->AsString())) _asset_manager->textures.at(a->AsString()).permanent = true;
    }
}

void AssetJson::Write(OpenGLv4_5_AssetManager* _asset_manager){
    auto j = gc.New<ufo::gc::JsonMap>();

    auto asset_arr = gc.New<ufo::gc::JsonArray>();

    for(const auto& [k,v] : _asset_manager->textures){
        if(v.permanent) asset_arr->array.push_back(gc.New<ufo::gc::JsonString>(k));
    }

    j->map.emplace("assets", asset_arr);

    j->Write(_asset_manager->save_path);
}
