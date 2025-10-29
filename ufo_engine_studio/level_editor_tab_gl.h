#pragma once
#include "../ufo_engine_studio/tab.h"
#include "../ufo_engine_studio/editor.h"
#include <engine.h>
#include <sprite_renderer.h>
#include <level.h>
#include "../ufo_engine_studio/controllable_camera.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui.h"
#include <gc_json.h>

namespace UFOEngineStudio{

class LevelEditorTab : public Tab{
public:
    ufo::Engine* engine = nullptr;
    Level* this_level = nullptr;

    LevelEditorTab(ufo::Engine* _engine, Editor* _editor) : Tab(), engine{_engine}{
        this_level = _editor->AddActor<Level>();
        this_level->AddActor<ControllableCamera>(Vector2f(0.0f, 0.0f));
        this_level->Load();
    }

    void OnActive(ImGuiID _local_dockspace_id , Editor* _editor, float _delta_time){

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar;

        ImGui::Begin(std::string("ActorTree###ActorTree"+std::to_string(id)).c_str());

        this_level->UpdateEditorTree(0);

        ImGui::End();

        ImGui::Begin("Properties");
        this_level->ViewProperties(0);
        ImGui::End();

        ImGui::Begin(name_and_imgui_id.c_str(), nullptr, window_flags);

        //Console::PrintLine("OnActive");
        LevelUpdatePhase(_delta_time);
        LevelDrawPhase(engine->graphics.get());

        ImGui::Image(
            (void*)(intptr_t)(dynamic_cast<ufo::OpenGLv4_5_Graphics*>(engine->graphics.get())->texture_id),
            ImVec2(engine->width, engine->height),
            ImVec2(0,0),
            ImVec2(1,-1)
        );

        ImGui::End();
    }

    void OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _program_state){
        ImGuiDockSpaceFill(_local_dockspace_id, ImGui::GetWindowSize(), name_and_imgui_id.c_str());
    }

    void LevelUpdatePhase(float _delta_time){
        this_level->UpdatePhrase(_delta_time);
    }

    void LevelDrawPhase(ufo::Graphics* _graphics){
        _graphics->BindFrameBuffer();
        
        glViewport(0, 0, this_level->size.x, this_level->size.y);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        this_level->DrawPhase(_graphics);

        _graphics->UnbindFrameBuffer();
    }

    void OnSave(Editor* _editor){
        auto level_json = this_level->GetAsJson(&gc);

        level_json->Write("test_level.json");
    }
};

}