#include <filesystem>
#include <vector>
#include <unordered_map>
#include <string>
#include <cmath>
#include "tileset_manager.h"
#include "tileset_data.h"
#include "../external/olcPixelGameEngine.h"
#include "../src/camera.h"
#include "../src/engine.h"
#include "../utils/console.h"
#include "../utils/file_utils.h"
#include "../src/ufo_macros.h"

#ifdef UFO_ENGINE_STUDIO
#include "../imgui/imgui.h"
#include "../ufo_engine_studio/file_dialogue.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/editor.h"
#endif

namespace ufo{

TilesetManager::TilesetManager(){
}

//Todo: This function much like the OpenGLv4_5_AssetManager one could be two different functions called in Start and StartWithImGui
void TilesetManager::InitialiseTextures(){
    if(!engine->in_editor){
        for(const auto& tileset : tileset_data){
            if(tileset.is_loaded_from_path) engine->asset_manager.LoadTexture("../"+tileset.name,tileset.name,true);
        }
    }
#ifdef UFO_ENGINE_STUDIO
    else{
        //This might break at some point.
        UFOEngineStudio::Editor* editor = engine->level_handle->DynamicCast<UFOEngineStudio::Editor>();
        for(const auto& tileset : tileset_data){
            std::string path = editor->opened_directory_path + "/" + tileset.name;
            Console::PrintLine("Full Tileset Path",path);
            if(tileset.is_loaded_from_path) engine->asset_manager.LoadTexture(path,tileset.name,true);

            if(!engine->asset_manager.textures.count(tileset.name)){
                //...
            }
        }
    }
#endif

}

TilesetData TilesetManager::GetTilesetData(std::string _name){
    for(auto&& data : tileset_data){
        if(data.name == _name) return data;
    }
    Console::PrintLine("Tilemap::GetTilesetData Invalid tileset name:", _name);
    //This error handling might not be good.
    return {
        "",
        0,
        0,
        0,
        0,
        0,
        0,
        0
    };
}

#ifdef UFO_ENGINE_STUDIO

void TilesetManager::UpdateSelectedTilesetTile(const TilesetData& _tileset){
    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 item_rect_pos = ImGui::GetItemRectMin();

    int clicked_pos_x = (mouse_pos.x-item_rect_pos.x)/_tileset.tile_width;
    int clicked_pos_y = (mouse_pos.y-item_rect_pos.y)/_tileset.tile_height;

    if(clicked_pos_x < 0) clicked_pos_x = 0;
    if(clicked_pos_x > _tileset.columns-1) clicked_pos_x = _tileset.columns-1;
    if(clicked_pos_y < 0) clicked_pos_y = 0;

    int tileset_rows = _tileset.tile_count/_tileset.columns;

    if(clicked_pos_y > tileset_rows-1) clicked_pos_y = tileset_rows-1;

    currently_selected_tile = clicked_pos_y*_tileset.columns + clicked_pos_x + _tileset.tileset_start_id;

}

void TilesetManager::InitialiseTexturesEditor(UFOEngineStudio::Editor* _editor){
    for(const auto& tileset : tileset_data){
        std::string path = _editor->opened_directory_path + "/" + tileset.name;
        Console::PrintLine("Full Tileset Path",path);
        if(tileset.is_loaded_from_path) engine->asset_manager.LoadTexture(path,tileset.name,true);

        if(!engine->asset_manager.textures.count(tileset.name)){
            //...
        }
    }
}

void TilesetManager::AddTileset(const std::string& _path, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    try{

        std::string relative_path = ufo::FileSystem::GetRelativePath(_path, _level_editor_tab->editor->opened_directory_path);

        engine->asset_manager.LoadTexture(_path, relative_path, true);
        int width = engine->asset_manager.textures.at(relative_path).width;
        int height = engine->asset_manager.textures.at(relative_path).height;

        int columns = (int)width/input_tileset_width;

        int rows = (int)height/input_tileset_height;

        int tileset_start_id = 1;

        if(!tileset_data.empty()) tileset_start_id = tileset_data.back().tileset_start_id+tileset_data.back().tile_count;

        tileset_data.push_back(
            TilesetData{
                relative_path,
                columns,
                tileset_start_id,
                (float)width, (float)height,
                (float)input_tileset_width, (float)input_tileset_height,
                columns*rows
            }
        );
    } catch (const std::runtime_error& _error){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, _error.what());
    }
}

void TilesetManager::RecoverTileset(int _index,const std::string& _path, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    try{
        std::string relative_path = ufo::FileSystem::GetRelativePath(_path, _level_editor_tab->editor->opened_directory_path);

        engine->asset_manager.LoadTexture(_path, relative_path, true);
        int width = engine->asset_manager.textures.at(relative_path).width;
        int height = engine->asset_manager.textures.at(relative_path).height;

        int columns = (int)width/16;

        int rows = (int)height/16;

        int tileset_start_id = 1;

        tileset_start_id = tileset_data[_index].tileset_start_id;

        tileset_data[_index] = TilesetData{
                relative_path,
                columns,
                tileset_start_id,
                (float)width, (float)height,
                16.0f, 16.0f,
                columns*rows
            };
    } catch (const std::runtime_error& _error){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, _error.what());
    }
}

void TilesetManager::EditorTilesetWidget(UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    if(ImGui::Button("Add Tileset")){
        adding_new_tileset = true;
    }

    if(adding_new_tileset){
        ImGui::InputInt("Tileset Width",&input_tileset_width);
        ImGui::InputInt("Tileset Height",&input_tileset_height);
        if(ImGui::Button("Ok")){
            SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenTileset, _level_editor_tab, _level_editor_tab->engine->window, UFOEngineStudio::global_texture_filters, 2, _level_editor_tab->editor->opened_directory_path.c_str(), true);
            _level_editor_tab->spawn_cursor->actors.clear();
            adding_new_tileset = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            adding_new_tileset = false;
        }
    }

    ImGui::SameLine();

    if(ImGui::Button("Brush")){
        _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_BRUSH;
        for(const auto& tileset : tileset_data){
             if(tileset.name == current_tileset) currently_selected_tiles = ManyTiles{{tileset.tileset_start_id},0,0,1,1};
        }
        _level_editor_tab->spawn_cursor->actors.clear();
    }

    ImGui::SameLine();

    if(ImGui::Button("Eraser")){
        currently_selected_tiles = ManyTiles{{0},0,0,1,1};
        _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_BRUSH;
        _level_editor_tab->spawn_cursor->actors.clear();
    }

    ImGui::SameLine();

    if(ImGui::Button("Fill Bucket")){
        _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_FILL_BUCKET;
        _level_editor_tab->spawn_cursor->actors.clear();
    }

    ImGui::SameLine();

    if(ImGui::Button("Rectangle Selection")){
        _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RECTANGLE_SELECTION;
        _level_editor_tab->spawn_cursor->actors.clear();
    }

    if(ImGui::BeginTabBar("TilesetManager")){
        for(auto& tileset : tileset_data){
            if(!engine->asset_manager.textures.count(tileset.name)){
                if(ImGui::Button(("Recover Missing Tileset:"+ tileset.name).c_str())){
                    SDL_ShowOpenFileDialog(
                        &UFOEngineStudio::OnRecoverTileset,
                        _level_editor_tab,
                        _level_editor_tab->engine->window,
                        nullptr,
                        0,
                        _level_editor_tab->editor->opened_directory_path.c_str(),
                        false
                    );
                    tileset_being_recovered = tileset.name;
                }
                continue;
            }

            if(ImGui::BeginTabItem(tileset.name.c_str(), &tileset.to_not_be_removed, ImGuiTabItemFlags_None)){
                if(current_tileset != tileset.name){
                    UpdateSelectedTilesetTile(tileset);
                    currently_selected_tile = tileset.tileset_start_id;
                }
                current_tileset = tileset.name;
                ImGui::Image(
                    (void*)(intptr_t)(engine->asset_manager.textures.at(tileset.name).id),
                    ImVec2(engine->asset_manager.textures.at(tileset.name).width, engine->asset_manager.textures.at(tileset.name).height),
                    ImVec2(0,0),
                    ImVec2(1,1)
                );

                ImVec2 mouse_pos = ImGui::GetMousePos();
                ImVec2 item_rect_pos = ImGui::GetItemRectMin();

                if(ImGui::IsItemHovered() && (ImGui::IsMouseDragging(0) || ImGui::IsMouseReleased(0))){
                    UpdateSelectedTilesetTile(tileset);
                }


                int tile_on_tile_selector = currently_selected_tile  - tileset.tileset_start_id;
                int new_tile_x = tile_on_tile_selector%tileset.columns;
                int new_tile_y = tile_on_tile_selector/tileset.columns;

                int columns_in_buffer = std::abs(new_tile_x-currently_selected_tiles.column)+1;
                int rows_in_buffer =    std::abs(new_tile_y-currently_selected_tiles.row)+1;

                int x0 = currently_selected_tiles.column;
                int y0 = currently_selected_tiles.row;
                int x1 = new_tile_x+1;
                int y1 = new_tile_y+1;

                if(x0 >= x1){
                    std::swap(x0,x1);
                    x1+=1;
                    x0-=1;

                }
                if(y0 >= y1){
                    std::swap(y0,y1);
                    y1+=1;
                    y0-=1;

                }

                if(ImGui::IsItemHovered()){
                    if(ImGui::IsMouseClicked(0)){
                        _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_BRUSH;

                        UpdateSelectedTilesetTile(tileset);

                        int tile_on_tile_selector = currently_selected_tile  - tileset.tileset_start_id;
                        int x = tile_on_tile_selector%tileset.columns;
                        int y = tile_on_tile_selector/tileset.columns;

                        Console::PrintLine(x,y);

                        currently_selected_tiles = ManyTiles{
                            {},
                            x,y,
                            1,1,
                            currently_selected_tile
                        };
                        _level_editor_tab->spawn_cursor->actors.clear();
                    }
                    if(ImGui::IsMouseDragging(0) || ImGui::IsMouseReleased(0)){
                        currently_selected_tiles.tiles.clear();

                        UpdateSelectedTilesetTile(tileset);

                        for(int r = y0; r < y1; r++){
                            for(int c = x0; c < x1; c++){
                                int t = (r*tileset.columns)+ c + tileset.tileset_start_id;
                                //Console::PrintLine("Added tile",t);
                                currently_selected_tiles.tiles.push_back(t);
                            }
                        }

                        currently_selected_tiles.number_of_columns = columns_in_buffer;
                        currently_selected_tiles.number_of_rows = rows_in_buffer;

                    }
                }

                ImGui::GetWindowDrawList()->AddRect(
                    ImVec2(item_rect_pos.x + x0*tileset.tile_width , item_rect_pos.y + y0*tileset.tile_height),
                    ImVec2(item_rect_pos.x +
                        x0*tileset.tile_width+tileset.tile_width*(x1-x0) ,
                        item_rect_pos.y +
                        y0*tileset.tile_height+tileset.tile_height*(y1-y0)),
                    0xFFFF00FF, 0.0f, 0, 1.0f);

                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }

    for(int ts = tileset_data.size()-1; ts != -1; ts--){
        if(!tileset_data[ts].to_not_be_removed) tileset_data.erase(tileset_data.begin()+ts);
    }

}

#endif //UFO_ENGINE_STUDIO

}
