#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ufo{

class Shader{
public:
    int INFO_LOG_SIZE = 512;
    
    unsigned int shader_program_id = 0;

    Shader();
    void Initialise();

    void Compile(const char* _vertex_shader_path, const char* _fragment_shader_path, const char* _geometry_shader_path = nullptr);

    void AttachVertexShader(std::string _path);

    void AttachFragmentShader(std::string _path);

    void AttachGeometryShader(std::string _path);

    void Use();
    
    void SetFloat(const char *_name, float _value, bool _use_shader = false);

    void SetInt(const char* _name, int _value, bool _use_shader = false);

    void SetVector2f(const char *_name, const glm::vec2 &_value, bool _use_shader = false);

    void SetVector3f(const char *_name, const glm::vec3 &_value, bool _use_shader = false);

    void SetVector4f(const char *_name, const glm::vec4 &_value, bool _use_shader = false);

    void SetMatrix4(const char *_name, const glm::mat4 &_matrix, bool _use_shader = false);

    void CheckCompileErrors(unsigned int _object, const std::string& _type);

};

}