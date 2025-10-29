#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace UFOEngineStudio{

class TextureWrapper{
public:
    SDL_Texture* texture;
    std::string alias;
    std::string easily_loadable_path_from_the_editors_perspective = "";
    bool edit_mode = false;
    TextureWrapper(SDL_Texture* _texture, std::string _name, std::string _easily_loadable_path_from_the_editors_perspective) : texture{_texture}, alias{_name}, easily_loadable_path_from_the_editors_perspective{_easily_loadable_path_from_the_editors_perspective}{}
};

class AssetManager{
public:
    std::unordered_map<std::string,std::unique_ptr<TextureWrapper>> textures;
    SDL_Renderer* renderer = nullptr;

    AssetManager() = default;

    AssetManager(SDL_Renderer* _renderer){
        renderer = _renderer;
    }

    void AddTexture(std::string _alias, std::string _working_directory, std::string _local_path){
        if(textures.count(_local_path)) return;

        Console::PrintLine(_working_directory, _local_path);
        SDL_Surface* example_surface = IMG_Load((_working_directory + _local_path).c_str());
        assert(example_surface != nullptr);
        SDL_Texture* example_texture = SDL_CreateTextureFromSurface(renderer ,example_surface);
        assert(example_texture != nullptr);
        SDL_DestroySurface(example_surface);

        textures.emplace(".."+_local_path,std::make_unique<TextureWrapper>(example_texture,".."+_alias, _local_path));
    }

    void AddPlaceholderTexture(std::string _alias, std::string _working_directory, std::string _path){
        Console::PrintLine(_working_directory, _path);
        SDL_Surface* example_surface = IMG_Load((_working_directory + _path).c_str());
        assert(example_surface != nullptr);
        SDL_Texture* example_texture = SDL_CreateTextureFromSurface(renderer,example_surface);
        assert(example_texture != nullptr);
        SDL_DestroySurface(example_surface);

        textures.emplace(_path,std::make_unique<TextureWrapper>(example_texture,_alias, _path));
    }

    void DeleteTexture(std::string _path){

        SDL_DestroyTexture(textures[_path]->texture);
        textures.erase(_path);

    }

    ~AssetManager(){
        for(auto && [k,v] : textures) SDL_DestroyTexture(v->texture);
        textures.clear();
    }

};

}