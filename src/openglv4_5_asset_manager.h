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

    void LoadTexture(const std::string& _path, const std::string& _name, bool _alpha);

    ufo::Texture2D LoadTextureFromFile(const std::string& _path, bool _alpha);

    void OnAddTexture(const std::string& _path, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    std::unordered_map<std::string, ufo::Shader> shaders;

    //ufo::Shader LoadShader(const std::string& _vertex_shader_path, const std::string& _fragment_shader_path, const std::string& _geometry_shader_path, const std::string& _name);
    ufo::Shader LoadShader(const char* _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path, const std::string& _name);

    ufo::Shader GetShader(const std::string& _name);

    ufo::Shader LoadShaderFromFile(const char* _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path);

    //Editor only
    void SaveAssets();

    void Clear();
};
