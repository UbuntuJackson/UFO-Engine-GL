#pragma once
#include <string>
#include "widget.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>
#include <unordered_map>
#include "texture_2d.h"
#include "font.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "actor.h"

namespace ufo{

class Text : public Widget{
private:

    //std::string text = "Hello world";

    Texture2D texture;

    bool is_wrapping = false;

public:
    std::unordered_map<std::string, std::string> language_to_text;

    Font font = Font("../UFO-Engine/res/fonts-japanese-gothic.ttf", 30.0f);

    void SetText(const std::string& _text){
        language_to_text[engine->language] = _text;
        OnIrregularUpdate();
    }

    std::string GetTextFromLanguageMap(){
        if(!language_to_text.count(engine->language)){
            return language_to_text.at("English");
        }
        return language_to_text.at(engine->language);
    }

    std::string GetText(){
        return language_to_text[engine->language];
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
        if(language_to_text[engine->language] == "") return;

        if(texture.id != 0) texture.Delete();

        SDL_Surface* surface_original = TTF_RenderText_Blended(font.GetFont(), GetTextFromLanguageMap().c_str(), 0,  (SDL_Color){255,255,255,255});

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
        Widget::OnViewProperties(_level_editor_tab, _index);

        if(ImGui::InputText("Text", &language_to_text[engine->language])) OnIrregularUpdate();

        if(ImGui::BeginCombo("Languages###Languages", engine->language.c_str())){

            for(int i = 0; i < engine->languages.size(); ++i){
                bool is_selected = (engine->language == engine->languages[i]);

                if(ImGui::Selectable(engine->languages[i].c_str(), &is_selected)){
                    engine->language = engine->languages[i];
                    OnIrregularUpdate();
                }

                if(is_selected){
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
    }

    void OnWidgetDraw(ufo::Graphics* _graphics){
        if(language_to_text[engine->language] == "") return;
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

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera){

    }

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc){
        Console::PrintLine("Does this even run?");

        ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);
        ufo::gc::JsonArray* tiles = _gc->New<ufo::gc::JsonArray>();


        auto j_language_to_text = _gc->New<ufo::gc::JsonMap>();
        for(const auto& [k,v] : language_to_text) j_language_to_text->map.emplace(k,_gc->New<ufo::gc::JsonString>(v));

        parent_class_as_json->map.emplace("language_to_text", j_language_to_text);
        return parent_class_as_json;
    }

};

}
