#pragma once

#include <unordered_map>
#include <string>
#include "texture_2d.h"
#include "shader.h"
#include "../file/file_utils.h"

namespace UFOEngineStudio{
    class LevelEditorTab;

}

namespace ufo{
    class Engine;
}

class OpenGLv4_5_AssetManager{
public:
    std::string save_path;

    OpenGLv4_5_AssetManager() = default;
    void Initialise(ufo::Engine* _engine);
    ~OpenGLv4_5_AssetManager();
    OpenGLv4_5_AssetManager(OpenGLv4_5_AssetManager&&) = delete;
    OpenGLv4_5_AssetManager(OpenGLv4_5_AssetManager&) = delete;

    //Name, Texture
    std::unordered_map<std::string, ufo::Texture2D> textures;

    //Todo: This one doesn't only load a texture, but it also emplaces it, warranting a namechange.
    void LoadTexture(const std::string& _path, const std::string& _name, bool _alpha);

    //This one loads the texture from file.
    ufo::Texture2D LoadTextureFromFile(const std::string& _path, bool _alpha);

    //This is a callback function that is called upon reading a .png from an SDL file dialogue.
    void OnAddTexture(const std::string& _path, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    //Here all shaders are stored and I want to keep it like that.
    std::unordered_map<std::string, ufo::Shader> shaders;

    //ufo::Shader LoadShader(const std::string& _vertex_shader_path, const std::string& _fragment_shader_path, const std::string& _geometry_shader_path, const std::string& _name);

    //Todo: This one has the same issue as LoadTexture, it doesn't actually load the shader, it emplaces it.
    ufo::Shader LoadShader(const char* _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path, const std::string& _name);

    //Todo: You don't really need a get function for assets...
    ufo::Shader GetShader(const std::string& _name);

    ufo::Shader LoadShaderFromFile(const char* _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path);

    //Editor only. Todo: When the editor refreshes or closes I want the assets to be saved in the project folder. However,
    // right now it only does so upon closing the editor.
    void SaveAssets();

    //This is called somewhere in the Main class together with the other cleanup function calls.
    void Clear();
};
