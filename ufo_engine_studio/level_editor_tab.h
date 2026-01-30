#pragma once
#include "../ufo_engine_studio/tab.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "../src/actor.h"
#include <vector>
#include "../ufo_maths/ufo_maths.h"

class Level;

namespace ufo{
    class Engine;
    class Graphics;
    class Editor;
}

namespace UFOEngineStudio{

class LevelEditorTab : public Tab{
public:
    enum Tools{ SELECT, PLACE, ERASE };

    Tools current_tool = Tools::SELECT;

    ufo::Engine* engine = nullptr;

    Actor* spawn_cursor = nullptr;
    Level* this_level = nullptr;
    std::string currently_viewed_properties_actor_name;
    float window_to_engine_ratio = 1.0f;
    std::unique_ptr<Actor> replace_with_actor;
    std::string header_file;

    //Important to note that this counts the distance from the 0,0 of the computer screen
    Vector2f level_viewport_position;

    //This represents where above the level viewport you're hovering your cursor
    Vector2f mouse_position_over_screenspace;
    Vector2f former_mouse_position_over_screenspace;

    //This is for multi-dragdropping and single dragdropping too
    std::vector<Actor::DraggedActorWhereAbouts> drag_dropped_actors;

    //So, this flag might not be needed, it's just a coding practice I've had for a while that I set a flag for things to be done later
    // It's just for resetting all actors to de-selected before selecting something else.
    bool reset_selection_status = false;

    //This is for the selection tool you use to select multiple items.
    Vector2f rectangle_selection_tool_start_position = {0.0f, 0.0f};
    //This is the selection tool you use to select multiple items but in world-coordinates
    ufo::Rectangle selection_rectangle_world_space = ufo::Rectangle(Vector2f(0.0f, 0.0f),Vector2f(0.0f, 0.0f));

    LevelEditorTab(ufo::Engine* _engine, Editor* _editor);

    //This gets the selection rectangle, but a more friendly version of it, for example it can't have a negative width and height, as
    // it flips coordinates, allowing to select from bottom right to top left.
    ufo::Rectangle GetSelectionRectangle();

    //Important to note that the return value is meant from drawing things in the viewport with ImGui drawlist, which means
    // that the x and y values are relative to 0,0 of the computer screen
    Vector2f TranslateToEditorScreenSpace(Vector2f _position);

    void Initialise();

    //This refreshes the tree, as well as actor trees and runs the UFO-Engine Header Tool, and generates generated.h.
    void Refresh();

    //When the tab is open this runs
    void OnActive(ImGuiID _local_dockspace_id , Editor* _editor, float _delta_time);

    void OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _program_state);

    void LevelDrawPhase(ufo::Graphics* _graphics);

    void OnSave(Editor* _editor);
};

}
