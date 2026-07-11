#include <string>
#include "widget.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>
#include <unordered_map>
#include "texture_2d.h"
#include "font.h"
#include "actor.h"
#include "text.h"
#include "engine.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#endif

namespace ufo{

void Text::SetText(const std::string& _text){
    language_to_text[engine->language] = _text;
    OnIrregularUpdate();
}

std::string Text::GetTextFromLanguageMap(){
    if(!language_to_text.count(engine->language)){
        return language_to_text.at("English");
    }
    return language_to_text.at(engine->language);
}

std::string Text::GetText(){
    return language_to_text[engine->language];
}

Text::Text(Vector2f _) : Widget(_){
    base_class_name = "ufo::Text";
    class_name = base_class_name;
}

void Text::OnSpawn(){
    OnIrregularUpdate();
}

Text::~Text(){
    texture.Delete();
}

void Text::OnIrregularUpdate(){
    if(language_to_text[engine->language] == "") return;

    if(texture.id != 0) texture.Delete();

    SDL_Surface* surface_original = nullptr;

    if(!is_wrapping) surface_original = TTF_RenderText_Blended(font.GetFont(), GetTextFromLanguageMap().c_str(), 0,  (SDL_Color){255,255,255,255});
    else{
        Widget* parent_widget = parent->DynamicCast<Widget>();
        if(parent_widget){
            surface_original = TTF_RenderText_Blended_Wrapped(font.GetFont(), GetTextFromLanguageMap().c_str(), 0, (SDL_Color){255,255,255,255}, parent_widget->rectangle.size.x);
        }
        else{
            surface_original = TTF_RenderText_Blended_Wrapped(font.GetFont(), GetTextFromLanguageMap().c_str(), 0, (SDL_Color){255,255,255,255}, engine->width-GetGlobalPosition().x);
        }
    }

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

void Text::OnWidgetDraw(ufo::Graphics* _graphics){
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

ufo::gc::JsonMap* Text::GetAsJson(ufo::GarbageCollector* _gc){
    Console::PrintLine("Does this even run?");

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

    parent_class_as_json->map.emplace("is_wrapping", _gc->New<ufo::gc::JsonNumber>(is_wrapping));

    auto j_language_to_text = _gc->New<ufo::gc::JsonMap>();
    for(const auto& [k,v] : language_to_text) j_language_to_text->map.emplace(k,_gc->New<ufo::gc::JsonString>(v));

    parent_class_as_json->map.emplace("language_to_text", j_language_to_text);
    return parent_class_as_json;
}

#ifdef UFO_ENGINE_STUDIO

void Text::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    Widget::OnViewProperties(_level_editor_tab, _index);

    if(ImGui::InputTextMultiline("Text", &language_to_text[engine->language])) OnIrregularUpdate();
    if(ImGui::Checkbox("Wrap", &is_wrapping)){
        OnIrregularUpdate();
    }

    if(ImGui::BeginCombo("Languages###Languages", engine->language.c_str())){

        for(int i = 0; i < (int)engine->languages.size(); ++i){
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

void Text::OnUpdateEditorViewport([[maybe_unused]] UFOEngineStudio::Editor* _editor, [[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    Vector2f pos_min = GetGlobalPosition()+editor_hitbox.position;
    Vector2f pos_max = GetGlobalPosition()+editor_hitbox.position+editor_hitbox.size;

    ImVec2 im_viewport_pos = ImGui::GetItemRectMin();

    Vector2f viewport_pos = Vector2f(im_viewport_pos.x, im_viewport_pos.y);

    //Vector2f cursor_pos = Vector2f(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

    ImVec2 content_pos = ImGui::GetWindowPos();
    ImVec2 window_pos = ImGui::GetMainViewport()->Pos;

    Vector2f editor_viewport_pos = Vector2f(viewport_pos.x-window_pos.x,viewport_pos.y-window_pos.y);

    Vector2f mouse_position_over_screenspace = engine->mouse.position-editor_viewport_pos;

    Vector2f world_mouse = mouse_position_over_screenspace;
    Vector2f former_world_mouse = engine->mouse.former_position-editor_viewport_pos;

    bool parent_is_widget = false;

    if(parent){
        if(parent->DynamicCast<Widget>()) parent_is_widget = true;
    }

    if(ufo::Maths::RectangleVsPoint(ufo::Rectangle(GetGlobalPosition()+editor_hitbox.position, editor_hitbox.size),world_mouse) && !parent_is_widget){
        //Console::PrintLine("Overlapping");
        if(engine->mouse.is_left_button_held){
            Vector2f dp = world_mouse - former_world_mouse;

            local_position += dp;

        }

    }

}

void Text::OnDrawGizmos([[maybe_unused]] ufo::Graphics* _graphics, [[maybe_unused]] Camera* _camera, [[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

#endif //UFO_ENGINE_STUDIO

}
