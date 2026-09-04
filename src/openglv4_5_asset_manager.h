#pragma once

#include <unordered_map>
#include <string>
#include <SDL3_mixer/SDL_mixer.h>
#include "bit_map_font.h"
#include "texture_2d.h"
#include "shader.h"

#ifdef UFO_ENGINE_STUDIO
namespace UFOEngineStudio{
    class LevelEditorTab;
    class Editor;

}
#endif

namespace ufo{
    class Engine;


class OpenGLv4_5_AssetManager{
public:
    std::string save_path;

    OpenGLv4_5_AssetManager() = default;
    void Initialise(ufo::Engine* _engine);

#ifdef UFO_ENGINE_STUDIO
    template<typename tType>
    std::vector<std::string> SearchForAsset(std::unordered_map<std::string,tType>& _assets, const std::string& _asset_browser_search){
        std::vector<std::string> texture_names;
        for(const auto& [name, texture] : _assets){
            bool search_is_in_word = false;

            for(int c = 0; c < (int)name.size(); c++){
                bool found_match_from_this_character = true;

                for(int d = 0; d < (int)_asset_browser_search.size(); d++){
                    if(c+d > (int)name.size()-1) continue;

                    if(_asset_browser_search[d]!=name[c+d]){
                        found_match_from_this_character = false;
                    }
                }

                if(found_match_from_this_character) search_is_in_word = true;
            }

            if(search_is_in_word) texture_names.push_back(name);
        }
        std::sort(texture_names.begin(), texture_names.end(), [](const std::string& _a,const std::string& _b){
            return _a<_b;
        });

        return texture_names;
    }

    void Initialise_UFOEngineStudio(UFOEngineStudio::Editor* _editor,ufo::Engine* _engine);
    //This is a callback function that is called upon reading a .png from an SDL file dialogue.
    void OnAddTexture(const std::string& _path, UFOEngineStudio::Editor* _editor);

#endif

    ~OpenGLv4_5_AssetManager();
    OpenGLv4_5_AssetManager(OpenGLv4_5_AssetManager&&) = delete;
    OpenGLv4_5_AssetManager(OpenGLv4_5_AssetManager&) = delete;

    //Name, Texture
    std::unordered_map<std::string, ufo::Texture2D> textures;

    //Todo: This one doesn't only load a texture, but it also emplaces it, warranting a namechange.
    void LoadTexture(const std::string& _path_to_project, const std::string& _path_relative_to_project, const std::string& _name, bool _alpha);

    //Here all shaders are stored and I want to keep it like that.
    std::unordered_map<std::string, ufo::Shader> shaders;

    //ufo::Shader LoadShader(const std::string& _vertex_shader_path, const std::string& _fragment_shader_path, const std::string& _geometry_shader_path, const std::string& _name);

    //Todo: This one has the same issue as LoadTexture, it doesn't actually load the shader, it emplaces it.
    bool LoadShader(const char* _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path, const std::string& _name);

    //Todo: You don't really need a get function for assets...
    ufo::Shader& GetShader(const std::string& _name);

     std::unordered_map<std::string, MIX_Audio*> audio;
     //void LoadAudio(const std::string& _path, const std::string& _name){}

     std::unordered_map<std::string, BitMapFont> bit_map_fonts;
     void AddBitMapFont(const std::string& _name, int _character_width, int _character_height);

    //Editor only. Todo: When the editor refreshes or closes I want the assets to be saved in the project folder. However,
    // right now it only does so upon closing the editor.
    void SaveAssets();

    //This is called somewhere in the Main class together with the other cleanup function calls.
    void Clear();

};
}
