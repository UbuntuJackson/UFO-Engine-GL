#include <unordered_map>
#include <string>
#include "texture_2d.h"
#include "../external/stb_image.h"
#include "shader.h"
#include "openglv4_5_asset_manager.h"

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
    
    stbi_image_free(data);

    return texture;
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

    Console::PrintLine(_vertex_shader_path, _fragment_shader_path);

    shader.Compile(_vertex_shader_path, _fragment_shader_path, _geometry_shader_path);

    /*shader.Initialise();
    if(_vertex_shader_path != nullptr) shader.AttachVertexShader(std::string(_vertex_shader_path));
    if(_fragment_shader_path != nullptr) shader.AttachFragmentShader(std::string(_fragment_shader_path));
    if(_geometry_shader_path != nullptr) shader.AttachGeometryShader(std::string(_geometry_shader_path));*/
    
    return shader;
}

void OpenGLv4_5_AssetManager::Clear(){
    for(auto iterator : textures){
        glDeleteTextures(1, &iterator.second.id);
    }
}