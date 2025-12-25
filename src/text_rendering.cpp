#include "glm/glm.hpp"
#include "../glad/include/glad/glad.h"
#include "shader.h"
#include <ft2build.h>
#include <string>
#include "engine.h"
#include "../utils/console.h"
#include "openglv4_5_asset_manager.h"
#include "text_rendering.h"

namespace ufo{

void TextRenderer::Init(Engine* _engine){

    shader = _engine->asset_manager.LoadShader(
        std::string(_engine->engine_path+"/shaders/text_vertex.cs").c_str(),
        std::string(_engine->engine_path+"/shaders/text_fragment.cs").c_str(),
        nullptr, "text_shader");

    Console::PrintLine("Engine width, height =",_engine->width, _engine->height);

    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(_engine->width),
        static_cast<float>(_engine->height), 0.0f
    );

    shader.Use();
    shader.SetMatrix4("projection", projection);

    FT_Library ft;

    if(FT_Init_FreeType(&ft)){
        Console::PrintLine("Error, could not initialise FreeType Library");
        throw;
    }

    std::string font_name = "../UFO-Engine/res/UbuntuSansMono-Italic[wght].ttf";

    if(font_name.empty()){
        Console::PrintLine("Error, failed to load font name");
        throw;
    }

    FT_Face face;

    if(FT_New_Face(ft, font_name.c_str(), 0, &face)){
        Console::PrintLine("Error, Failed to load font");
        throw;
    }
    else{
        FT_Set_Pixel_Sizes(face, 0, 48);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for(int c = 0; c < 128; c++){
            if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
                Console::PrintLine("Error, Failed to load Glyph");
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer

            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };

            characters.insert(std::pair<char, Character>(c, character));



        }
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}

void TextRenderer::RenderText(const std::string& _text, float _x, float _y, float _scale, glm::vec4 _colour){
    shader.Use();

    glUniform3f(glGetUniformLocation(shader.shader_program_id, "textColor"), _colour.x, _colour.y, _colour.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;
    for(c = _text.begin(); c != _text.end(); c++){
        Character ch = characters[*c];

        float xpos = _x+ch.bearing.x * _scale;
        float ypos = (characters['H'].bearing.y -ch.bearing.y)*_scale + _y;

        float w = ch.size.x * _scale;
        float h = ch.size.y * _scale;

        float vertecies[6][4] = {
            {xpos, ypos + h, 0.0f, 1.0f},
            {xpos+w, ypos,     1.0f, 0.0f},
            {xpos, ypos,   0.0f, 0.0f},

            {xpos, ypos+h,   0.0f, 1.0f},
            {xpos+w, ypos+h,   1.0f, 1.0f},
            {xpos+w, ypos, 1.0f, 0.0f}
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

}
