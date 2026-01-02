#include <unordered_map>
#include <string>
#include "texture_2d.h"
#include "../external/stb_image.h"
#include "shader.h"
#include "openglv4_5_asset_manager.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "asset_json.h"
#include "../src/engine.h"
#include "../ufo_engine_studio/editor.h"
#include <filesystem>

//Todo: Different intialise function for the Editor would be a lot cleaner, eventhough this isn't as bad as it seems.
// What happens within this class upon initialisation and deinitialisation is going to be very conditional no matter how
// you twist and turn it.
void OpenGLv4_5_AssetManager::Initialise(ufo::Engine* _engine){
    LoadTexture(_engine->engine_path+"/res/placeholder_icon.png", "placeholder_icon", true);

    if(!_engine->in_editor){
        Console::PrintLine("OpenGLv4_5_AssetManager reading from path", std::filesystem::current_path().c_str());
        AssetJson j;
        save_path = "../loaded_assets.json";
        if(!ufo::FileSystem::FileExists(save_path)) ufo::FileSystem::Write(save_path, "{\"assets\":[]}");
        j.Read(save_path, "", this);
    }
    else{
        save_path = _engine->level->DynamicCast<UFOEngineStudio::Editor>()->opened_directory_path+"/loaded_assets.json";
        if(!ufo::FileSystem::FileExists(save_path)) ufo::FileSystem::Write(save_path, "{\"assets\":[]}");

        Console::PrintLine(save_path);
        AssetJson j;
        j.ReadEditor(save_path,_engine->level->DynamicCast<UFOEngineStudio::Editor>()->opened_directory_path, this);
    }
}

OpenGLv4_5_AssetManager::~OpenGLv4_5_AssetManager(){

}

void OpenGLv4_5_AssetManager::LoadTexture(const std::string& _path, const std::string& _name, bool _alpha){
    textures[_name] = LoadTextureFromFile(_path, _alpha);
}

ufo::Texture2D OpenGLv4_5_AssetManager::LoadTextureFromFile(const std::string& _path, bool _alpha){
    ufo::Texture2D texture;
    if(_alpha){
        texture.internal_format = GL_RGBA;
        texture.image_format = GL_RGBA;
    }

    int width;
    int height;
    int number_of_channels;

    //In LearnOpenGL they apparently don't do much more than passing in the number_of_channels variable
    // This doesn't seem to do anything but setting it, which implies a mistake
    // Instead I have decided that I'll request a number of channels based on the _alpha boolean
    // replacing 0 with _alpha ? 4 : 3
    unsigned char* data = stbi_load(_path.c_str(), &width, &height, &number_of_channels, _alpha ? 4 : 3);

    texture.Generate(width, height, data);

    Console::PrintLine("[UFO-Engine] Loading texture of size", width, height);

    stbi_image_free(data);

    return texture;
}

void OpenGLv4_5_AssetManager::OnAddTexture(const std::string& _path, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    std::string relative_path = ufo::FileSystem::GetRelativePath(_path, _level_editor_tab->editor->opened_directory_path);

    Console::PrintLine("OnAddTexture",relative_path);

    _level_editor_tab->engine->asset_manager.LoadTexture(_path, ".."+relative_path, true);

    _level_editor_tab->engine->asset_manager.textures.at(".."+relative_path).permanent = true;

}

/*ufo::Shader OpenGLv4_5_AssetManager::LoadShader(const std::string& _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path, const std::string& _name){
    return LoadShader(_vertex_shader_path.c_str(), _fragment_shader_path, _geometry_shader_path, _name);
}*/

ufo::Shader OpenGLv4_5_AssetManager::LoadShader(const char* _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path, const std::string& _name){
    shaders[_name] = LoadShaderFromFile(_vertex_shader_path, _fragment_shader_path, _geometry_shader_path);
    return shaders[_name];
}

ufo::Shader OpenGLv4_5_AssetManager::GetShader(const std::string& _name){
    return shaders[_name];
}

ufo::Shader OpenGLv4_5_AssetManager::LoadShaderFromFile(const char* _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path){
    ufo::Shader shader;

    Console::PrintLine("[UFO-Engine] Loading shaders",_vertex_shader_path, _fragment_shader_path);

    shader.Compile(_vertex_shader_path, _fragment_shader_path, _geometry_shader_path);

    /*shader.Initialise();
    if(_vertex_shader_path != nullptr) shader.AttachVertexShader(std::string(_vertex_shader_path));
    if(_fragment_shader_path != nullptr) shader.AttachFragmentShader(std::string(_fragment_shader_path));
    if(_geometry_shader_path != nullptr) shader.AttachGeometryShader(std::string(_geometry_shader_path));*/

    return shader;
}

void OpenGLv4_5_AssetManager::SaveAssets(){
    AssetJson j;
    j.Write(save_path, this);
}

void OpenGLv4_5_AssetManager::Clear(){

    for(auto iterator : textures){
        glDeleteTextures(1, &iterator.second.id);
    }
}
