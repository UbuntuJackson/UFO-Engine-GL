#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../utils/console.h"
#include "../file/file.h"
#include "shader.h"
#include "../utils/opengl_utils.h"
#include "../utils/ufo_macros.h"

namespace ufo{

Shader::Shader(){
    
}

void
Shader::Initialise(){
    shader_program_id = glCreateProgram();
    glLinkProgram(shader_program_id);
    Console::PrintLine("Shader::Shader() ran", shader_program_id);
}

void Shader::Compile(const char* _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path){
    
    unsigned int vertex_shader;
    unsigned int fragment_shader;
    unsigned int geometry_shader;

    if(!File::Exists(_vertex_shader_path) || !File::Exists(_fragment_shader_path)){
        Console::PrintLine("One of the shaders don't exist");
    }

    std::string vertex_shader_source_str = File(_vertex_shader_path).GetAsString();
    std::string fragment_shader_source_str = File(_fragment_shader_path).GetAsString();

    const char* vertex_shader_source = vertex_shader_source_str.c_str();
    const char* fragment_shader_source = fragment_shader_source_str.c_str();

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    CheckCompileErrors(vertex_shader, "VERTEX");

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    CheckCompileErrors(fragment_shader, "FRAGMENT");

    shader_program_id = glCreateProgram();
    Console::PrintLine("Shader program id:",shader_program_id);
    glAttachShader(shader_program_id, vertex_shader);
    glAttachShader(shader_program_id, fragment_shader);
    glLinkProgram(shader_program_id);
    CheckCompileErrors(shader_program_id, "PROGRAM");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

}

void Shader::AttachVertexShader(std::string _path){

    if(!File::Exists(_path)){
        Console::PrintLine("Shader does not exist at path:", _path);
        return;
    }

    //Comiling vertex shader
    std::string vertex_shader_source = File(_path).GetAsString();

    const char* vertex_shader_source_as_c_str = vertex_shader_source.c_str();
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source_as_c_str, NULL);
    glCompileShader(vertex_shader);

    CheckCompileErrors(vertex_shader, "VERTEX");

    glAttachShader(shader_program_id, vertex_shader);

    glLinkProgram(shader_program_id);
    CheckCompileErrors(shader_program_id, "PROGRAM");

    glDeleteShader(vertex_shader);
}

void Shader::AttachFragmentShader(std::string _path){

    if(!File::Exists(_path)){
        Console::PrintLine("Shader does not exist at path:", _path);
        return;
    }

    //Comiling fragment shader
    std::string fragment_shader_source = File(_path).GetAsString();

    const char* fragment_shader_source_as_c_str = fragment_shader_source.c_str();
    
    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source_as_c_str, NULL);
    glCompileShader(fragment_shader);

    CheckCompileErrors(fragment_shader, "FRAGMENT");

    glAttachShader(shader_program_id, fragment_shader);

    glLinkProgram(shader_program_id);
    CheckCompileErrors(shader_program_id, "PROGRAM");

    glDeleteShader(fragment_shader);
}

void Shader::AttachGeometryShader(std::string _path){
    //Comiling geometry shader
    if(!File::Exists(_path)){
        Console::PrintLine("Shader does not exist at path:", _path);
        return;
    }

    std::string geometry_shader_source = File(_path).GetAsString();

    const char* geometry_shader_source_as_c_str = geometry_shader_source.c_str();
    
    unsigned int geometry_shader;
    geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry_shader, 1, &geometry_shader_source_as_c_str, NULL);
    glCompileShader(geometry_shader);

    CheckCompileErrors(geometry_shader, "GEOMETRY");

    glAttachShader(shader_program_id, geometry_shader);

    glLinkProgram(shader_program_id);
    CheckCompileErrors(shader_program_id, "PROGRAM");

    glDeleteShader(geometry_shader);
}

void Shader::Use(){
    if(shader_program_id == 0) Console::PrintLine("Error, shader is uninitialised");
    glUseProgram(shader_program_id);
    GetGLError(__UFO_PRETTY_FUNCTION__, -1);
    
}

void Shader::SetFloat(const char *_name, float _value, bool _use_shader){
    if(_use_shader) Use();
    glUniform1f(glGetUniformLocation(shader_program_id, _name), _value);
}
void Shader::SetInt(const char* _name, int _value, bool _use_shader){
    if(_use_shader) Use();
    glUniform1i(glGetUniformLocation(shader_program_id, _name), _value);
}
void Shader::SetVector2f(const char *_name, const glm::vec2 &_value, bool _use_shader){
    if(_use_shader) Use();
    glUniform2f(glGetUniformLocation(shader_program_id, _name), _value.x, _value.y);
}
void Shader::SetVector3f(const char *_name, const glm::vec3 &_value, bool _use_shader){
    if(_use_shader) Use();
    glUniform3f(glGetUniformLocation(shader_program_id, _name), _value.x, _value.y, _value.z);
}

void Shader::SetVector4f(const char *_name, const glm::vec4 &_value, bool _use_shader){
    if(_use_shader) Use();
    glUniform4f(glGetUniformLocation(shader_program_id, _name), _value.x, _value.y, _value.z, _value.w);
}

void Shader::SetMatrix4(const char *_name, const glm::mat4 &_matrix, bool _use_shader){
    if(_use_shader) Use();
    glUniformMatrix4fv(glGetUniformLocation(shader_program_id, _name), 1, false, glm::value_ptr(_matrix));
}

void Shader::CheckCompileErrors(unsigned int _object, const std::string& _type){
    int success = 1;

    const int info_log_size = 1024;

    char info_log[info_log_size];

    if(_type != "PROGRAM"){
        glGetShaderiv(_object, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(_object, info_log_size, NULL, info_log);
            Console::PrintLine("Error::Shader: Compile-time error: Type:", _type, info_log, "\n");

        }
    }
    else{
        glGetProgramiv(_object, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(_object, info_log_size, NULL, info_log);
            Console::PrintLine("Error::Shader: Link-time error: Type:", _type, info_log, "\n");

        }
    }
}

}