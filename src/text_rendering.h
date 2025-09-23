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

    void RenderText(const std::string& _text, float _x, float _y, float _scale, glm::vec4 _colour){
        shader.Use();

        glUniform3f(glGetUniformLocation(shader.shader_program_id, "textColor"), _colour.x, _colour.y, _colour.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        std::string::const_iterator c;
        for(c = _text.begin(); c != _text.end(); c++){
            Character ch = characters[*c];

            float xpos = _x+ch.bearing.x * _scale;
            float ypos = _y-(ch.size.y - ch.bearing.y) * _scale;

            float w = ch.size.x * _scale;
            float h = ch.size.y * _scale;

            float vertecies[6][4] = {
                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos, ypos,     0.0f, 1.0f},
                {xpos+w, ypos,   1.0f, 1.0f},

                {xpos, ypos+h,   0.0f, 0.0f},
                {xpos+w, ypos,   1.0f, 1.0f},
                {xpos+w, ypos+h, 1.0f, 0.0f}
            };

            glBindTexture(GL_TEXTURE_2D, ch.texture_id);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertecies), vertecies);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0,6);

            _x += (ch.advance >> 6) * _scale;

        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Test(){
        RenderText(";;;__:_:_:%¤/&%#¤(&#¤(¤(¤(&%))))", 20.0f, 20.0f, 0.5f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
};

}