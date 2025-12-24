#include "../imgui/imgui.h"
#include "../ufo_engine_studio/dock_utils.h"
#include "editor.h"

namespace UFOEngineStudio{
    void Tutorial(Editor* _editor){
        if (ImGui::BeginPopupModal("Tutorial", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextWrapped(
                "Thank you for checking out UFO-Engine Studio. This is an in-house engine and does not necessarily use standard C++ practices. "
                "The UFO-Engine is aiming to be a game-engine for 2D platformers and RPGs, and is currently under construction.\n"

                "All game-objects in UFO-Engine are referred to as Actor. This term was chosen partly to distinguish it from other concepts like 'entities'"
                "Everything that inherits from the baseclass 'Actor' is an actor. Like players, sprites, even tilemaps."

                "To place out actors in the UFO-Engine you create a new level file (.ason), vi file -> new -> level."
                "You then go to the level tree -> right-click -> new actor. Choose your preferred actor and there you go."

                "To create your own custom actors, you use the C++ programming language."
            );

            if(ImGui::Button("Create example_player.ufo.h")){

            }

            ImGui::TextWrapped("As you see, unlike ordinary .h files, .ufo.h files are parsed by the UFO-Engine Header Tool."
                "Since .h files can contain more than one class, you need to mark your class with the ufo_class(), or ufo_category() macro."
            );

            ImGui::Separator();

            if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

            ImGui::SameLine();
            if (ImGui::Button("Next", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

            ImGui::SetItemDefaultFocus();

        }
    }
}
