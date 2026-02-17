#include <stdexcept>
#include <vector>
#include <memory>
#include "tile_map.h"
#include "../src/camera.h"
#include "../ufo_maths/ufo_maths.h"
#include "../src/actor.h"
#include "../src/level.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "../src/engine.h"
#include "../src/graphics.h"
#include "../src/engine.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "tileset_manager.h"
#include "../ufo_engine_studio/editor.h"
#include "../src/actor_undo_and_redo.h"

void TileMap::OnSpawn(){
    Actor::OnSpawn();

}

TileMap::TileMap(Vector2f _) : Actor(_){
    base_class_name = "TileMap";
    class_name = base_class_name;
    tilemap_data = std::vector<int>(number_of_rows*number_of_columns, 0);
}

void TileMap::Do(){
    //change the course of changes
    while((int)level->level_changes.size()-1 > level->current_level_change){
        Console::PrintLine("loop change stack",level->current_level_change, level->level_changes.size());
        level->level_changes.pop_back();
    }

}

void TileMap::DoPaint(){
    Do();

    Console::PrintLine("Popped change stack",level->current_level_change, level->level_changes.size());

    level->level_changes.push_back(std::make_unique<TileMapChange_Paint>(
        this,
        left_bound_tile,
        lower_bound_tile,
        right_bound_tile,
        upper_bound_tile)
    );

    level->level_changes.back()->Do();

    level->current_level_change++;

    //Output: accurate
    Console::PrintLine("Doing:",level->current_level_change,right_bound_tile-left_bound_tile);
}

void TileMap::DoResize(int _left, int _right, int _bottom, int _top){
    Do();

    level->level_changes.push_back(
        std::make_unique<TileMapChange_TileMapSize>(this, _left, _right, _bottom, _top)
    );
    level->current_level_change++;
}

std::unique_ptr<TileMap>
TileMap::Load(ufo::gc::JsonMap* _layer){
    std::unique_ptr<TileMap> u_tilemap = std::make_unique<TileMap>(Vector2f(0.0f,0.0f));

    if(_layer->map.at("type")->AsString() == "tilelayer"){
        std::vector<int> data;
        auto layer_data = _layer->map.at("data")->AsArray();

        for(const auto& _json : layer_data){
            int id = (float)_json->AsFloat();
            u_tilemap->tilemap_data.push_back(id);
        }

    }

    return std::move(u_tilemap);
}

int TileMap::GetTileID_AtLevelPosition(Vector2f _position){
    int tile_id = tilemap_data[int(_position.y/tile_height) * number_of_columns + (_position.x/tile_width)];
    return tile_id;
}

ufo::Rectangle
TileMap::GetRectangle(int _x, int _y, Vector2f _frame_size){
    ufo::Rectangle rect = ufo::Rectangle({(float)(_x * _frame_size.x), (float)(_y * _frame_size.y)}, _frame_size);
    return rect;
}

ufo::Rectangle
TileMap::GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size){
    return GetRectangle(
        (int)_frame % (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x), //1 can only give me x = 0
        (int)_frame / (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x),
        _frame_size); //1 can only give y = 1
}

void TileMap::ResizeRight(int _number_of_tiles_to_insert){
    if(_number_of_tiles_to_insert+number_of_columns < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize right");

    if(_number_of_tiles_to_insert > 0){
        int start_index = number_of_columns;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < _number_of_tiles_to_insert; j++) tilemap_data.insert(tilemap_data.begin()+start_index, 0);
            start_index+=(_number_of_tiles_to_insert+number_of_columns);
        }
    }
    else{
        int start_index = number_of_columns+_number_of_tiles_to_insert;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < _number_of_tiles_to_insert*-1; j++) tilemap_data.erase(tilemap_data.begin()+start_index);
            start_index+=(_number_of_tiles_to_insert+number_of_columns);
        }
    }

    number_of_columns += _number_of_tiles_to_insert;
}

void TileMap::ResizeLeft(int _number_of_tiles_to_insert){
    if(_number_of_tiles_to_insert+number_of_columns < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize left");

    if(_number_of_tiles_to_insert > 0){
        int start_index = 0;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < _number_of_tiles_to_insert; j++) tilemap_data.insert(tilemap_data.begin()+start_index, 0);
            start_index+=(_number_of_tiles_to_insert+number_of_columns);
        }
    }
    else{
        int start_index = _number_of_tiles_to_insert;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < _number_of_tiles_to_insert*-1; j++) tilemap_data.erase(tilemap_data.begin()+start_index);
            start_index+=(_number_of_tiles_to_insert+number_of_columns);
        }
    }

    number_of_columns += _number_of_tiles_to_insert;
}

void TileMap::ResizeBottom(int _number_of_tiles_to_insert){
    if(_number_of_tiles_to_insert+number_of_rows < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize bottom");

    if(_number_of_tiles_to_insert > 0){
        int add_at_index = number_of_columns*number_of_rows;
        for(int i = 0; i < number_of_columns*_number_of_tiles_to_insert; i++){
            tilemap_data.insert(tilemap_data.begin()+add_at_index, 0);
        }
    }
    else{
        int remove_at_index = number_of_columns*number_of_rows - number_of_columns*_number_of_tiles_to_insert;
        for(int i = 0; i < number_of_columns*_number_of_tiles_to_insert*-1; i++) tilemap_data.erase(tilemap_data.begin()+remove_at_index);
    }

    number_of_rows += _number_of_tiles_to_insert;
}

void TileMap::ResizeTop(int _number_of_tiles_to_insert){
    if(_number_of_tiles_to_insert+number_of_rows < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize top");

    if(_number_of_tiles_to_insert > 0){
        int add_at_index = 0;
        for(int i = 0; i < number_of_columns*_number_of_tiles_to_insert; i++){
            tilemap_data.insert(tilemap_data.begin()+add_at_index, 0);
        }
    }
    else{
        int remove_at_index = 0;
        for(int i = 0; i < number_of_columns*_number_of_tiles_to_insert*-1; i++) tilemap_data.erase(tilemap_data.begin()+remove_at_index);
    }

    number_of_rows += _number_of_tiles_to_insert;
}

void TileMap::CancelAllResizeDialogues(){
    resize_right = false;
    resize_left = false;
    resize_top = false;
    resize_bottom = false;
}

void TileMap::OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    if(!visible || !properties_open) return;

    float scale = _camera->scale;

    Vector2f world_mouse = level->active_camera_handles.back()->TransformScreenToWorld(_level_editor_tab->mouse_position_over_screenspace);

    current_world_mouse_x = world_mouse.x;
    current_world_mouse_y = world_mouse.y;

    //Console::PrintLine("World position",_level_editor_tab->mouse_position_over_screenspace);

    int hovered_tile_x = int(world_mouse.x)/tile_width;
    int hovered_tile_y = int(world_mouse.y)/tile_height;

    if(hovered_tile_x < 0) hovered_tile_x = 0;
    if(hovered_tile_x > number_of_columns-1) hovered_tile_x = number_of_columns-1;
    if(hovered_tile_y < 0) hovered_tile_y = 0;
    if(hovered_tile_y > number_of_rows-1) hovered_tile_y = number_of_rows-1;

    currently_hovered_tile_x = hovered_tile_x;
    currently_hovered_tile_y = hovered_tile_y;

    {

        int xx = 0;
        int yy = 0;
        for(const int i : level->tileset_manager.currently_selected_tiles.tiles){

            for(auto&& tileset : level->tileset_manager.tileset_data){

                int tile_id = i;

                olc::vd2d tile_position = {(hovered_tile_x+ xx)*tileset.tile_width, (hovered_tile_y+yy)*tileset.tile_height};

                if(tileset.tileset_start_id <= tile_id && tile_id < tileset.tileset_start_id+tileset.tile_count){
                    ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(tileset.name,tile_id-tileset.tileset_start_id,{tileset.tile_width, tileset.tile_height});
                    //Console::Out("sample rectangle:", sample_rectangle.position, sample_rectangle.size);
                    _graphics->DrawPartialSprite(
                        tileset.name,
                        _camera->Transform(tile_position),
                        {0.0f, 0.0f},
                        {scale, scale},
                        sample_rectangle.position,
                        sample_rectangle.size,
                        0.0f,
                        ufo::Colour(255,255,255,255)
                    );
                }

            }

            //int tile_to_be_set = (hovered_tile_y+yy)*number_of_columns + (hovered_tile_x+xx);

            //if(tile_to_be_set > -1 && tile_to_be_set < tilemap_data.size()) tilemap_data[tile_to_be_set] = i;

            xx++;
            if(xx >= level->tileset_manager.currently_selected_tiles.number_of_columns){
                xx = 0;
                yy++;
            }

        }
    }
}

void TileMap::OnDraw(ufo::Graphics* _graphics, Camera* _camera){
    if(!visible) return;

    float scale = _camera->scale;
    Bounds world_bounds = _camera->world;

    //Haven't made a TilesetManager yet.
    for(auto&& tileset : level->tileset_manager.tileset_data){
        ufo::Rectangle screen_rectangle = _camera->GetOnScreenRectangleInWorld({tileset.tile_width, tileset.tile_height});

        int tile_start_x = int(std::floor(screen_rectangle.position.x/tileset.tile_width));
        int tile_end_x = int(std::floor((screen_rectangle.position.x + screen_rectangle.size.x)/tileset.tile_width));
        int tile_start_y = int(std::floor(screen_rectangle.position.y/tileset.tile_height));
        int tile_end_y = int(std::floor((screen_rectangle.position.y + screen_rectangle.size.y)/tileset.tile_height));

        tile_start_x = std::max(tile_start_x, 0);
        tile_end_x = std::min(tile_end_x, number_of_columns);
        tile_start_y = std::max(tile_start_y, 0);
        tile_end_y = std::min(tile_end_y, number_of_rows);

        for(int index_y = tile_start_y; index_y < tile_end_y; index_y++){
            for(int index_x = tile_start_x; index_x < tile_end_x; index_x++){
                int tile_id = tilemap_data[index_y*number_of_columns + index_x];

                olc::vd2d tile_position = {index_x*tileset.tile_width, index_y*tileset.tile_height};

                if(tileset.tileset_start_id <= tile_id && tile_id < tileset.tileset_start_id+tileset.tile_count){
                    ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(tileset.name,tile_id-tileset.tileset_start_id,{tileset.tile_width, tileset.tile_height});
                    //Console::Out("sample rectangle:", sample_rectangle.position, sample_rectangle.size);
                    _graphics->DrawPartialSprite(
                        tileset.name,
                        _camera->Transform(tile_position),
                        {0.0f, 0.0f},
                        {scale, scale},
                        sample_rectangle.position,
                        sample_rectangle.size,
                        0.0f,
                        ufo::Colour(255,255,255,255)
                    );
                }

            }
        }

    }

}

void TileMap::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    Actor::OnViewProperties(_level_editor_tab, _index);

    ImGui::Separator();

    ImGui::Checkbox(std::string("Visible###"+std::to_string(editor_id)).c_str(), &visible);

    ImGui::Text("Tilemap Width: %i tiles", number_of_columns);
    ImGui::Text("Tilemap Height: %i tiles", number_of_rows);

    //Currently hovered tiles

    ImGui::Text("Tile x: %i tiles", currently_hovered_tile_x);
    ImGui::Text("Tile y: %i tiles", currently_hovered_tile_y);

    ImGui::Text("Current world mouse x: %i", current_world_mouse_x);
    ImGui::Text("Current world mouse y: %i", current_world_mouse_y);

    try{

    if(!resize_right){
        if(ImGui::Button("Resize Right")){
            resize_right = true;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){
            ResizeRight(number_of_tiles_to_insert);

            DoResize(0,number_of_tiles_to_insert,0,0);


            resize_right = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_right = false;
        }
    }

    if(!resize_left){
        if(ImGui::Button("Resize Left")){
            resize_left = true;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){
            ResizeLeft(number_of_tiles_to_insert);

            DoResize(number_of_tiles_to_insert,0,0,0);

            resize_left = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_left = false;
        }
    }

    if(!resize_bottom){
        if(ImGui::Button("Resize Bottom")){
            resize_bottom = true;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){
            ResizeBottom(number_of_tiles_to_insert);

            DoResize(0,0,number_of_tiles_to_insert,0);

            resize_bottom = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_bottom = false;
        }
    }

    if(!resize_top){
        if(ImGui::Button("Resize Top")){
            resize_top = true;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){
            ResizeTop(number_of_tiles_to_insert);

            DoResize(0,0,0,number_of_tiles_to_insert);

            resize_top = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_top = false;
        }
    }

    } catch(const std::runtime_error& _error){
        Console::PrintLine("[UFO-Engine] TileMap::OnViewProperties", _error.what());
        CancelAllResizeDialogues();
    }

    ImGui::Separator();

    level->tileset_manager.EditorTilesetWidget(_level_editor_tab);
}

void TileMap::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    if(!properties_open) return;

    _level_editor_tab->spawn_cursor->local_position = Vector2f(
        std::floor(_level_editor_tab->spawn_cursor->local_position.x/tile_width)*tile_width,
        std::floor(_level_editor_tab->spawn_cursor->local_position.y/tile_height)*tile_height);

    Vector2f world_mouse = level->active_camera_handles.back()->TransformScreenToWorld(_level_editor_tab->mouse_position_over_screenspace);

    current_world_mouse_x = world_mouse.x;
    current_world_mouse_y = world_mouse.y;

    //Console::PrintLine("World position",_level_editor_tab->mouse_position_over_screenspace);

    int hovered_tile_x = int(world_mouse.x)/tile_width;
    int hovered_tile_y = int(world_mouse.y)/tile_height;

    if(hovered_tile_x < 0) hovered_tile_x = 0;
    if(hovered_tile_x > number_of_columns-1) hovered_tile_x = number_of_columns-1;
    if(hovered_tile_y < 0) hovered_tile_y = 0;
    if(hovered_tile_y > number_of_rows-1) hovered_tile_y = number_of_rows-1;

    currently_hovered_tile_x = hovered_tile_x;
    currently_hovered_tile_y = hovered_tile_y;

    if(ImGui::IsItemHovered(0) && ImGui::IsMouseClicked(0)){
        tilemap_data_before_change = tilemap_data;
        left_bound_tile = hovered_tile_x;
        lower_bound_tile = hovered_tile_y;
        upper_bound_tile = lower_bound_tile+level->tileset_manager.currently_selected_tiles.number_of_rows;
        right_bound_tile = left_bound_tile+level->tileset_manager.currently_selected_tiles.number_of_columns;
    }

    if(ImGui::IsItemHovered(0) && ImGui::IsMouseReleased(0)){

        if(level->tileset_manager.tool == TilesetManager::Tools::BRUSH){
            DoPaint();
        }
        else if(level->tileset_manager.tool == TilesetManager::Tools::FILL_BUCKET){
            bool tiles_are_being_added = true;

            std::vector<Vector2i> all_filled_tiles; //For undo&redo

            std::vector<Vector2i> tiles_to_fill;

            tiles_to_fill.push_back(Vector2i(hovered_tile_x, hovered_tile_y));

            int max_number_of_tiles = 100;

            while(tiles_are_being_added && max_number_of_tiles > 0){
                max_number_of_tiles--;

                std::vector<Vector2i> additional_tiles;

                tiles_are_being_added = false;
                for(Vector2i tile_position : tiles_to_fill){

                    std::vector<Vector2i> directions = {
                        tile_position+Vector2i(1, 0),
                        tile_position+Vector2i(-1, 0),
                        tile_position+Vector2i(0, -1),
                        tile_position+Vector2i(0, 1)

                    };

                    for(Vector2i tile_direction : directions){
                        if(tile_direction.x < 0 || tile_direction.x >= number_of_columns) continue;
                        if(tile_direction.y < 0 || tile_direction.y >= number_of_rows) continue;

                        int tile_index = tile_direction.y*number_of_columns + tile_direction.x;

                        if(tile_index > -1 && tile_index < tilemap_data.size()){
                            if(tilemap_data[tile_index] == 0){

                                tilemap_data[tile_index] = level->tileset_manager.currently_selected_tiles.first_selected_tile;

                                additional_tiles.push_back(tile_direction);

                                all_filled_tiles.push_back(tile_direction);

                                tiles_are_being_added = true;

                            }

                        }
                    }
                }

                tiles_to_fill = additional_tiles;

            }

            if(max_number_of_tiles <= 0) Console::PrintLine("TileMap: Reached max tiles");

            int lower_bound_tile_fill = hovered_tile_y;
            int upper_bound_tile_fill = hovered_tile_y;
            int left_bound_tile_fill = hovered_tile_x;
            int right_bound_tile_fill = hovered_tile_x;

            for(Vector2i tile : all_filled_tiles){
                Console::PrintLine(tile);
                if(lower_bound_tile_fill > tile.y) lower_bound_tile_fill = tile.y;
                if(upper_bound_tile_fill < tile.y) upper_bound_tile_fill = tile.y;
                if(left_bound_tile_fill > tile.x) left_bound_tile_fill = tile.x;
                if(right_bound_tile_fill < tile.x) right_bound_tile_fill = tile.x;
            }

            Console::PrintLine(left_bound_tile_fill, lower_bound_tile_fill, right_bound_tile_fill,upper_bound_tile_fill);

            level->RemoveFutureChanges();

            level->level_changes.push_back(std::make_unique<TileMapChange_Paint>(this,left_bound_tile_fill,lower_bound_tile_fill,right_bound_tile_fill+1,upper_bound_tile_fill+1));

            level->level_changes.back()->Do();

        }

    }

    if(ImGui::IsItemHovered(0) && ImGui::IsMouseDown(0)){
        if(hovered_tile_x < left_bound_tile) left_bound_tile = hovered_tile_x;
        if(hovered_tile_y < lower_bound_tile) lower_bound_tile = hovered_tile_y;
        if((int)hovered_tile_y +level->tileset_manager.currently_selected_tiles.number_of_rows > upper_bound_tile) upper_bound_tile = hovered_tile_y+level->tileset_manager.currently_selected_tiles.number_of_rows;
        if((int)hovered_tile_x+level->tileset_manager.currently_selected_tiles.number_of_columns > right_bound_tile) right_bound_tile = hovered_tile_x+level->tileset_manager.currently_selected_tiles.number_of_columns;

        if(level->tileset_manager.tool == TilesetManager::Tools::BRUSH){
            int xx = 0;
            int yy = 0;
            for(const int i : level->tileset_manager.currently_selected_tiles.tiles){

                int tile_to_be_set = (hovered_tile_y+yy)*number_of_columns + (hovered_tile_x+xx);

                if(tile_to_be_set > -1 && tile_to_be_set < tilemap_data.size()) tilemap_data[tile_to_be_set] = i;

                xx++;
                if(xx >= level->tileset_manager.currently_selected_tiles.number_of_columns){
                    xx = 0;
                    yy++;
                }

            }
        }

    }

    Vector2f bounds_min = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition());
    Vector2f bounds_max = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+Vector2f(number_of_columns*tile_width, number_of_rows*tile_height));

    ImU32 colour = 0x66777755;

    for(int rr = 0; rr < number_of_rows; rr++){
        Vector2f line_start_screen_space = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+Vector2f(0.0f, rr*tile_height));
        Vector2f line_end_screen_space = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+Vector2f(number_of_columns*tile_width, rr*tile_height));

        ImGui::GetWindowDrawList()->AddLine(ImVec2(line_start_screen_space.x, line_start_screen_space.y), ImVec2(line_end_screen_space.x, line_end_screen_space.y), colour);
    }

    for(int cc = 0; cc < number_of_columns; cc++){
        Vector2f line_start_screen_space = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+Vector2f(cc*tile_width, 0.0f));
        Vector2f line_end_screen_space = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+Vector2f(cc*tile_width, number_of_rows*tile_height));

        ImGui::GetWindowDrawList()->AddLine(ImVec2(line_start_screen_space.x, line_start_screen_space.y), ImVec2(line_end_screen_space.x, line_end_screen_space.y), colour);
    }

    ImGui::GetWindowDrawList()->AddRect(ImVec2(bounds_min.x,bounds_min.y), ImVec2(bounds_max.x,bounds_max.y), colour, 1.0f,ImDrawFlags_RoundCornersAll);

    //Placing actors

    if(is_selected){
        if(ImGui::IsItemClicked(0) && _level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::PLACE){
            if(_editor->currently_selected_actor_type != ""){
                if(_editor->spawnable_actor_map.count(_editor->currently_selected_actor_type)){
                    auto inst = _editor->spawnable_actor_map.at(_editor->currently_selected_actor_type)->Spawn(_editor);

                    inst->local_position = _level_editor_tab->spawn_cursor->GetGlobalPosition() - GetGlobalPosition();

                    //Undo&redo

                    while((int)level->level_changes.size()-1 > level->current_level_change){
                        Console::PrintLine("loop change stack",level->current_level_change, level->level_changes.size());
                        level->level_changes.pop_back();
                    }

                    level->level_changes.push_back(std::make_unique<ufo::ActorChange_AddActor>(inst.get()));

                    level->current_level_change++;
                    Console::PrintLine("Actor current change",level->current_level_change);

                    AddActorUniquePtr(std::move(inst));

                }
            }
        }
    }

}

bool TileMap::OnUpdateEditorViewportFocus(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    return false;
}

void TileMap::OnAdditionalButtonsForTreeItem(){
    ImGui::SameLine();
    std::string visible_or_not_string = visible ? "<o>###" : "</>###";
    if(ImGui::Button((visible_or_not_string+std::to_string(editor_id)).c_str())){
        visible = !visible;
    }
}

ufo::gc::JsonMap* TileMap::GetAsJson(ufo::GarbageCollector* _gc){
    Console::PrintLine("Does this even run?");

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);
    ufo::gc::JsonArray* tiles = _gc->New<ufo::gc::JsonArray>();

    for(const auto& i : tilemap_data) tiles->array.push_back(_gc->New<ufo::gc::JsonNumber>(i));
    parent_class_as_json->map.emplace("tiles",tiles);
    parent_class_as_json->map.emplace("number_of_columns", _gc->New<ufo::gc::JsonNumber>(number_of_columns));
    parent_class_as_json->map.emplace("number_of_rows", _gc->New<ufo::gc::JsonNumber>(number_of_rows));
    parent_class_as_json->map.emplace("visible", _gc->New<ufo::gc::JsonNumber>(visible));
    return parent_class_as_json;
}
