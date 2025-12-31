#pragma once
#include <string>
#include "widget.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>
#include "texture_2d.h"
#include "font.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "actor.h"

namespace ufo{

class Text : public Widget{
private:

    std::string text = "Hello world";

    Texture2D texture;

public:
    Font font = Font("../UFO-Engine/res/fonts-japanese-gothic.ttf", 30.0f);

    void SetText(const std::string& _text){
        text = _text;
        OnIrregularUpdate();
    }

    std::string GetText(){
        return text;
    }

    Text(Vector2f _) : Widget(_){
        base_class_name = "Text";
        class_name = base_class_name;
    }

    void OnSpawn(){
        OnIrregularUpdate();
    }


    ~Text(){
        texture.Delete();
    }


    void OnIrregularUpdate(){
        if(text == "") return;

        if(texture.id != 0) texture.Delete();

        SDL_Surface* surface_original = TTF_RenderText_Blended(font.GetFont(), text.c_str(), 0,  (SDL_Color){255,255,255,255});

        SDL_Surface* surface = SDL_CreateSurface(
            surface_original->w, surface_original->h,
            SDL_PIXELFORMAT_RGBA32);
        SDL_BlitSurface(surface_original, NULL, surface, NULL);

        if(!surface){
            Console::PrintLine("Failed to TTF_RenderTextBlended", SDL_GetError());
        }

        const SDL_PixelFormatDetails* pixel_format_details = SDL_GetPixelFormatDetails(surface->format);

        if(!pixel_format_details){
            Console::PrintLine("Failed to get SDL_PixelFormatDetails*",SDL_GetError());
        }


        unsigned int texture_format = GL_BGRA;
        unsigned int number_of_colours = pixel_format_details->bytes_per_pixel;

        if(number_of_colours == 4){
            if(pixel_format_details->Rmask == 0x000000ff) texture_format = GL_RGBA;
            else texture_format = GL_BGRA;

        }
        else{
            if(pixel_format_details->Rmask == 0x000000ff) texture_format = GL_RGB;
            else texture_format = GL_BGR;
        }

        Console::PrintLine("Number of colours", texture_format == GL_RGBA);

        texture.image_format = texture_format;
        texture.internal_format = texture_format;

        texture.Generate((unsigned int)surface->w, (unsigned int)surface->h, (unsigned char*)surface->pixels);

        SDL_DestroySurface(surface);

    }

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
        if(ImGui::InputText("Text", &text)) OnIrregularUpdate();
    }

    void OnWidgetDraw(ufo::Graphics* _graphics){
        if(text == "") return;
        _graphics->DrawPartialSprite(
            texture,
            GetGlobalPosition(),
            Vector2f(0.0f, 0.0f),
            Vector2f(1.0f, 1.0f),
            Vector2f(0.0f, 0.0f),
            Vector2f(texture.width, texture.height),
            0.0f,
            ufo::Colour(255,255,255,255)
        );
    }

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc){
        Console::PrintLine("Does this even run?");

        ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);
        ufo::gc::JsonArray* tiles = _gc->New<ufo::gc::JsonArray>();


        parent_class_as_json->map.emplace("text", _gc->New<ufo::gc::JsonString>(text));
        return parent_class_as_json;
    }

};

}
