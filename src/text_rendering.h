#pragma once
#include <map>
#include "glm/glm.hpp"
#include "../glad/include/glad/glad.h"
#include "shader.h"
#include <ft2build.h>
#include <string>
#include "../utils/console.h"
#include FT_FREETYPE_H

struct Character
{
    unsigned int texture_id;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

namespace ufo{

class TextRenderer{
public:
    unsigned int VAO;
    unsigned int VBO;
    std::map<GLint, Character> characters;
    Shader shader;

    void Init(Engine* _engine);

    void RenderText(const std::string& _text, float _x, float _y, float _scale, glm::vec4 _colour);

    void Test(){
        //Console::PrintLine("TextRendering::Test");
        RenderText("The quick brown fox.", 20.0f, 0.0f, 5.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
};

}
