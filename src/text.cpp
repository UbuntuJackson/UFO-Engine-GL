#include <string>
#include "bit_map_font.h"
#include "console.h"
#include "ufo_macros.h"
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
#include "../ufo_engine_studio/editor.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../ufo_engine_studio/file_dialogue.h"
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

    if(engine->asset_manager.textures.count("res/gfx/unifont.png")) engine->asset_manager.LoadBitMapFont("res/gfx/unifont.png", 18, 18);
    else Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error, couldn't find bitmap font", "res/gfx/unifont.png");
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
    if(!use_bit_map_font) _graphics->DrawPartialSprite(
        texture,
        GetGlobalPosition(),
        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 0.0f),
        Vector2f(texture.width, texture.height),
        0.0f,
        ufo::Colour(255,255,255,255),
        "partial_sprite_shader"
    );
    else{
        engine->asset_manager.bit_map_fonts.at(bit_map_font_key).Draw(_graphics, language_to_text[engine->language], GetGlobalPosition(), Vector2f(1.0f, 1.0f),
        "partial_sprite_shader",ufo::Colour(255,255,255,255));
    }
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

void Text::OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    if(ImGui::BeginTabItem("Fonts")){

        ImGui::BeginChild("FontsChildWindow");

        if(ImGui::InputText("Search###SearchAssetBrowser", &_level_editor_tab->asset_browser_search)){

        }

        ImGui::Separator();

        bool font_was_erased = false;
        std::string name_of_erased_texture = "";

        std::vector<std::string> font_names;
        for(const auto& [name, bit_map_font] : engine->asset_manager.bit_map_fonts){
            bool search_is_in_word = false;

            for(int c = 0; c < (int)name.size(); c++){
                bool found_match_from_this_character = true;

                for(int d = 0; d < (int)_level_editor_tab->asset_browser_search.size(); d++){
                    if(c+d > (int)name.size()-1) continue;

                    if(_level_editor_tab->asset_browser_search[d]!=name[c+d]){
                        found_match_from_this_character = false;
                    }
                }

                if(found_match_from_this_character) search_is_in_word = true;
            }

            if(search_is_in_word) font_names.push_back(name);
        }
        std::sort(font_names.begin(), font_names.end(), [](const std::string& _a,const std::string& _b){
            return _a<_b;
        });

        for(const std::string& name : font_names){

            auto& texture = engine->asset_manager.textures.at(name);

            float w = (float)texture.width;
            float h = (float)texture.height;

            bool view_asset_details = ImGui::CollapsingHeader(std::string("###view_asset_details"+name).c_str(), nullptr, ImGuiTreeNodeFlags_SpanTextWidth);

            ImGui::SameLine();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
            ImGui::ImageButton(name.c_str(),
                (void*)(intptr_t)texture.id,
                ImVec2(32.0f*w/h, 32.0f),
                ImVec2(0,0),
                ImVec2(1,1),
                ImVec4(0,0,0,1)
            );
            ImGui::PopStyleVar();

            if(ImGui::IsItemHovered()) ImGui::SetTooltip(name.c_str(), "%s");

            if(view_asset_details){
                if(ImGui::Button(std::string("Unload Font###UnloadTexture"+name).c_str())){
                    name_of_erased_texture = name;
                    font_was_erased = true;
                }

                ImGui::SameLine();

                if(ImGui::Button(std::string("Apply Font###UseTexture"+name).c_str())){
                    bit_map_font_key = name;
                }

                ImGui::InputInt("Character Width", &engine->asset_manager.bit_map_fonts.at(name).character_width);
                ImGui::InputInt("Character Height", &engine->asset_manager.bit_map_fonts.at(name).character_height);

                ImGui::Text(std::string("width: " + std::to_string(w) + " height: "+std::to_string(h)).c_str(),"%s");
                ImGui::Text(("name: "+name).c_str(),"%s");
                ImGui::Text(texture.permanent ? "Status: Permanent" : "Status: Temporary");
            }

        }

        if(font_was_erased && name_of_erased_texture != ""){

            engine->asset_manager.bit_map_fonts.erase(name_of_erased_texture);
            _level_editor_tab->editor->ResourcesEdited();

            if(bit_map_font_key == name_of_erased_texture) bit_map_font_key = "";

        }

        ImGui::EndChild();

        ImGui::EndTabItem();
    }

    if(ImGui::BeginTabItem("Textures")){

        if(ImGui::Button("[+] Add Texture")){
            SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenTexture, _level_editor_tab, engine->window, nullptr, 0, _level_editor_tab->editor->opened_directory_path.c_str(), true);
        }

        if(ImGui::InputText("Search###SearchAssetBrowser", &_level_editor_tab->asset_browser_search)){

        }

        ImGui::Separator();

        if(ImGui::BeginChild("MyAssetsChildWindow")){

            bool texture_was_erased = false;
            std::string name_of_erased_texture = "";

            std::vector<std::string> texture_names;
            for(const auto& [name, texture] : engine->asset_manager.textures){
                bool search_is_in_word = false;

                for(int c = 0; c < (int)name.size(); c++){
                    bool found_match_from_this_character = true;

                    for(int d = 0; d < (int)_level_editor_tab->asset_browser_search.size(); d++){
                        if(c+d > (int)name.size()-1) continue;

                        if(_level_editor_tab->asset_browser_search[d]!=name[c+d]){
                            found_match_from_this_character = false;
                        }
                    }

                    if(found_match_from_this_character) search_is_in_word = true;
                }

                if(search_is_in_word) texture_names.push_back(name);
            }
            std::sort(texture_names.begin(), texture_names.end(), [](const std::string& _a,const std::string& _b){
                return _a<_b;
            });

            for(const std::string& name : texture_names){

                auto& texture = engine->asset_manager.textures.at(name);

                float w = (float)texture.width;
                float h = (float)texture.height;

                bool view_asset_details = ImGui::CollapsingHeader(std::string("###view_asset_details"+name).c_str(), nullptr, ImGuiTreeNodeFlags_SpanTextWidth);

                ImGui::SameLine();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
                ImGui::ImageButton(name.c_str(),
                    (void*)(intptr_t)texture.id,
                    ImVec2(32.0f*w/h, 32.0f),
                    ImVec2(0,0),
                    ImVec2(1,1),
                    ImVec4(0,0,0,1)
                );
                ImGui::PopStyleVar();

                if(ImGui::IsItemHovered()) ImGui::SetTooltip(name.c_str(), "%s");

                if(view_asset_details){
                    if(ImGui::Button(std::string("Unload Texture###UnloadTexture"+name).c_str())){
                        name_of_erased_texture = name;
                        texture_was_erased = true;
                    }
                    ImGui::SameLine();
                    if(ImGui::Button(std::string("Crete Font###AddCostume"+name).c_str())){
                        engine->asset_manager.LoadBitMapFont(name, 1, 1);

                    }
                    ImGui::Text(std::string("width: " + std::to_string(w) + " height: "+std::to_string(h)).c_str(),"%s");
                    ImGui::Text(("name: "+name).c_str(),"%s");
                    ImGui::Text(texture.permanent ? "Status: Permanent" : "Status: Temporary");
                }

            }

            if(texture_was_erased && name_of_erased_texture != "placeholder_icon"){
                engine->asset_manager.textures.at(name_of_erased_texture).Delete();
                engine->asset_manager.textures.erase(name_of_erased_texture);
                _level_editor_tab->editor->ResourcesEdited();

                if(bit_map_font_key == name_of_erased_texture) bit_map_font_key = "placeholder_icon";

            }

            ImGui::EndChild();

        }

        ImGui::EndTabItem();
    }
}

#endif //UFO_ENGINE_STUDIO

}
