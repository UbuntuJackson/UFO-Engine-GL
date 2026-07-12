#include <stdexcept>
#include <unordered_map>
#include <string>
#include "console.h"
#include "texture_2d.h"
#include "../external/stb_image.h"
#include "shader.h"
#include "openglv4_5_asset_manager.h"
#include "asset_json.h"
#include "../src/engine.h"
#include <filesystem>

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/editor.h"
#endif //UFO_ENGINE_STUDIO

namespace ufo{

//Todo: Different intialise function for the Editor would be a lot cleaner, eventhough this isn't as bad as it seems.
// What happens within this class upon initialisation and deinitialisation is going to be very conditional no matter how
// you twist and turn it.
void OpenGLv4_5_AssetManager::Initialise(ufo::Engine* _engine){
    if(_engine->in_editor) throw std::runtime_error(
        "[UFO-Engine] OpenGLv4_5_AssetManager::Initialise: Called when in editor. Use Initialise_UFOEngineStudio instead."
    );

    LoadTexture(_engine->engine_path+"/res/placeholder_icon.png", "placeholder_icon", true);

    if(!textures.count("placeholder_icon")){
        Console::PrintLine("Could not load placeholder_icon");
        throw;
    }

    Console::PrintLine("OpenGLv4_5_AssetManager reading from path", std::filesystem::current_path().c_str());
    AssetJson j;
    save_path = "../loaded_assets.json";
    if(!ufo::FileSystem::FileExists(save_path)) ufo::FileSystem::Write(save_path, "{\"assets\":[],\"shaders\":[]}");
    j.Read(save_path, "..", this, _engine);

}

OpenGLv4_5_AssetManager::~OpenGLv4_5_AssetManager(){

}

void OpenGLv4_5_AssetManager::LoadTexture(const std::string& _path, const std::string& _name, bool _alpha){
    if(!ufo::FileSystem::FileExists(_path)){
        Console::PrintLine("[UFO-Engine] OpenGLv4_5_AssetManager::LoadTexture: Error, could not find image at path",_path);
        return;
    }
    textures[_name] = LoadTextureFromFile(_path, _alpha);
}

ufo::Texture2D OpenGLv4_5_AssetManager::LoadTextureFromFile(const std::string& _path, bool _alpha){
    ufo::Texture2D texture;
    if(_alpha){
        texture.internal_format = GL_RGBA;
        texture.image_format = GL_RGBA;
    }
    else{
        texture.internal_format = GL_RGB;
        texture.image_format = GL_RGB;
    }

    int width;
    int height;
    int number_of_channels;

    //In LearnOpenGL they apparently don't do much more than passing in the number_of_channels variable
    // This doesn't seem to do anything but setting it, which implies a mistake
    // Instead I have decided that I'll request a number of channels based on the _alpha boolean
    // replacing 0 with _alpha ? 4 : 3
    unsigned char* data = stbi_load(_path.c_str(), &width, &height, &number_of_channels, _alpha ? 4 : 3);

    texture.Generate((int)width, (int)height, data);

    Console::PrintLine("[UFO-Engine] Loading texture of size", width, height);

    stbi_image_free(data);

    return texture;
}

#ifdef UFO_ENGINE_STUDIO
void OpenGLv4_5_AssetManager::Initialise_UFOEngineStudio(UFOEngineStudio::Editor* _editor,ufo::Engine* _engine){
    LoadTexture(_engine->engine_path+"/res/placeholder_icon.png", "placeholder_icon", true);
    LoadTexture("../UFO-Engine/res/actor_icon.png","actor_icon", true);


    save_path = _editor->opened_directory_path+"/loaded_assets.json";
    if(!ufo::FileSystem::FileExists(save_path)) ufo::FileSystem::Write(save_path, "{\"assets\":[],\"shaders\":[]}");

    Console::PrintLine(save_path);
    AssetJson j;
    j.ReadEditor(save_path,_editor->opened_directory_path, this, _engine);

}

void OpenGLv4_5_AssetManager::OnAddTexture(const std::string& _path, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    std::string relative_path = ufo::FileSystem::GetRelativePath(_path, _level_editor_tab->editor->opened_directory_path);

    Console::PrintLine("OnAddTexture",relative_path);

    _level_editor_tab->engine->asset_manager.LoadTexture(_path, relative_path, true);

    _level_editor_tab->engine->asset_manager.textures.at(relative_path).permanent = true;

}
#endif

bool OpenGLv4_5_AssetManager::LoadShader(const char* _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path, const std::string& _name){
    ufo::Shader shader;

    Console::PrintLine("[UFO-Engine] Loading shaders",_vertex_shader_path, _fragment_shader_path);

    bool is_shader_properly_compiled = shader.Compile(_vertex_shader_path, _fragment_shader_path, _geometry_shader_path);

    if(is_shader_properly_compiled){
        shaders.emplace(_name, shader);
        return true;
    }

    return false;
}

ufo::Shader& OpenGLv4_5_AssetManager::GetShader(const std::string& _name){
    return shaders.at(_name);
}

void OpenGLv4_5_AssetManager::SaveAssets(){
    AssetJson j;
    j.Write(this);
}

void OpenGLv4_5_AssetManager::Clear(){

    for(auto iterator : textures){
        glDeleteTextures(1, &iterator.second.id);
    }

    for(auto shader_iterator : shaders){
        glDeleteProgram(shader_iterator.second.shader_program_id);
    }
}

}
