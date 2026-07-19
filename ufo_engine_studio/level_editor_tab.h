#pragma once
#include <vector>
#include "../ufo_engine_studio/tab.h"
#include "../imgui/imgui.h"
#include "../ufo_maths/ufo_maths.h"
#include "../src/actor.h"
#include "actor_undo_and_redo.h"

namespace ufo{
    class Level;
    class Engine;
    class Graphics;
    class Editor;
}

namespace UFOEngineStudio{

class LevelEditorTab : public Tab{
public:

    // This is the actor currently used as the layer. It can be of type ufo::Level, ufo::CollisionGrid, and ufo::TileMap
    int currently_edited_actor_in_viewport = ufo::Maths::NULL_ID;
    std::vector<int> selected_actors;

    int actor_dedicated_to_viewport = ufo::Maths::NULL_ID;
    int inspected_actor = ufo::Maths::NULL_ID;

    //This is for multi-dragdropping and single dragdropping too
    std::vector<ufo::Actor::DraggedActorWhereAbouts> drag_dropped_actors;

    enum Tools{
        //For general viewport?
        NONE , SELECT,RESIZE, SCALE, ROTATE, PLACE, ERASE, /*EDIT_TILEMAP,*/ MOVE_ACTOR_CLUSTER, MULTI_SELECT,ESTABLISH_MULTI_SELECT,
        //TileMap
        TILE_MAP_FILL_BUCKET, TILE_MAP_BRUSH, TILE_MAP_ERASER, TILE_MAP_RESIZE, TILE_MAP_RECTANGLE_SELECTION, TILE_MAP_RECTANGLE_SELECTION_RESIZE, TILE_MAP_PASTE, TILE_MAP_SELECTION_DELETE

    };

    std::string asset_browser_search;

    bool focused_actor_found = false;
    bool show_multi_selection_right_click_pop_up_menu = false;

    struct UndoRedoAction{
        int actor_id;
        Tools tool;
        std::unique_ptr<ufo::ActorChange> actor_change;
    };

    Tools current_tool = Tools::SELECT;
    UndoRedoAction current_undo_redo_action = UndoRedoAction{ufo::Maths::NULL_ID, Tools::NONE, nullptr};

    void SubmitUndoRedoAction();

    ufo::Engine* engine = nullptr;

    ufo::Actor* spawn_cursor = nullptr;
    ufo::Level* this_level = nullptr;
    std::string currently_viewed_properties_actor_name;
    float window_to_engine_ratio = 1.0f;

    //Important to note that this counts the distance from the 0,0 of the computer screen
    Vector2f level_viewport_position;

    //This represents where above the level viewport you're hovering your cursor, except proportional to the resized viewport
    // in other words in-game screenspace
    Vector2f mouse_position_over_screenspace;
    Vector2f former_mouse_position_over_screenspace;

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
    void ResourcesEdited();

    //When the tab is open this runs
    void OnActive(ImGuiID _local_dockspace_id , Editor* _editor, float _delta_time);

    void PlaceActors();
    void SelectionUpdate();

    void OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _program_state);

    void LevelDrawPhase(ufo::Graphics* _graphics);

    void OnSave(Editor* _editor);
};

}
