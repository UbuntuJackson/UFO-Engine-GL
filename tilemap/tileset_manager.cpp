#include <filesystem>
#include <vector>
#include <unordered_map>
#include <string>
#include <cmath>
#include "../ufo_maths/ufo_maths.h"
#include "tileset_manager.h"
#include "../shapes/ray2.h"
#include "tileset_data.h"
#include "../external/olcPixelGameEngine.h"
#include "../src/camera.h"
#include "../src/engine.h"
#include "../utils/console.h"
#include "../json/json.h"
#include "../shapes/rectangle.h"
#include "../ufo_engine_studio/file_dialogue.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "../file/file_utils.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/editor.h"

namespace ufo{

TilesetManager::TilesetManager(){
}

//Todo: This function much like the OpenGLv4_5_AssetManager one could be two different functions called in Start and StartWithImGui
void TilesetManager::InitialiseTextures(){
    if(!engine->in_editor){
        for(const auto& tileset : tileset_data){
            if(tileset.is_loaded_from_path) engine->asset_manager.LoadTexture(tileset.name,tileset.name,true);
        }
    }
    else{
        //This might break at some point.
        UFOEngineStudio::Editor* editor = engine->level_handle->DynamicCast<UFOEngineStudio::Editor>();
        for(const auto& tileset : tileset_data){
            std::string path = editor->opened_directory_path + "/" + tileset.name.substr(2,tileset.name.size());
            Console::PrintLine("Full Tileset Path",path);
            if(tileset.is_loaded_from_path) engine->asset_manager.LoadTexture(path,tileset.name,true);

            if(!engine->asset_manager.textures.count(tileset.name)){
                //...
            }
        }
    }

}

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
        std::string path = _editor->opened_directory_path + "/" + tileset.name.substr(2,tileset.name.size());
        Console::PrintLine("Full Tileset Path",path);
        if(tileset.is_loaded_from_path) engine->asset_manager.LoadTexture(path,tileset.name,true);
    }
}

void TilesetManager::Load(ufo::GarbageCollector* _gc, const ufo::gc::JsonMap* _json){

    Console::Out("[!]", "Currently loading tilesets");

    for(auto&& tileset_json : _json->map.at("tilesets")->AsArray()){
        auto tileset_json_dictionary = tileset_json->AsMap();

        std::string image = tileset_json_dictionary.at("image")->AsString();
        std::string name = tileset_json_dictionary.at("name")->AsString();

        std::string relative_path;

        std::string image_path = relative_path;

        engine->asset_manager.LoadTexture(image_path+"/"+image, name, true);
        keys_to_temporary_assets.push_back(name);

        int columns = tileset_json_dictionary.at("columns")->AsFloat();
        int first_gid = tileset_json_dictionary.at("firstgid")->AsFloat();
        int image_height = tileset_json_dictionary.at("imageheight")->AsFloat();
        int image_width = tileset_json_dictionary.at("imagewidth")->AsFloat();
        int tile_count = tileset_json_dictionary.at("tilecount")->AsFloat();
        int tileset_tile_size_x = tileset_json_dictionary.at("tilewidth")->AsFloat();
        int tileset_tile_size_y = tileset_json_dictionary.at("tileheight")->AsFloat();
        TilesetData local_tilset_data = TilesetData{
            name,
            columns,
            first_gid,
            (float)image_width,
            (float)image_height,
            (float)tileset_tile_size_x,
            (float)tileset_tile_size_y,
            tile_count
        };

        if(name == "actors"){

            Console::Out(image_path+"/"+image);

            Console::Out(
                "[!] Tileset data:\n",
                "name:", name, "\n",
                "columns:",columns,"\n",
                "image width:",image_width,"\n",
                "image height:",image_height,"\n",
                "tile width:",tileset_tile_size_x,"\n",
                "tile height:",tileset_tile_size_y,"\n",
                "tile count:",tile_count
            );
        }

        tileset_data.push_back(
            local_tilset_data
        );
    }

}

TilesetData TilesetManager::GetTilesetData(std::string _name){
    for(auto&& data : tileset_data){
        if(data.name == _name) return data;
    }
    Console::PrintLine("Tilemap::GetTilesetData Invalid tileset name:", _name);
    return {
        ""
    };
}

void TilesetManager::AddTileset(const std::string& _path, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    Console::PrintLine(_path, _level_editor_tab->editor->opened_directory_path);
    Console::PrintLine(ufo::FileSystem::GetRelativePath(_path, _level_editor_tab->editor->opened_directory_path));

    std::string relative_path = ufo::FileSystem::GetRelativePath(_path, _level_editor_tab->editor->opened_directory_path);

    engine->asset_manager.LoadTexture(_path, ".."+relative_path, true);
    int width = engine->asset_manager.textures.at(".."+relative_path).width;
    int height = engine->asset_manager.textures.at(".."+relative_path).height;

    int columns = (int)width/16;

    int rows = (int)height/16;

    int tileset_start_id = 1;

    if(!tileset_data.empty()) tileset_start_id = tileset_data.back().tileset_start_id+tileset_data.back().tile_count;

    tileset_data.push_back(
        TilesetData{
            ".."+relative_path,
            columns,
            tileset_start_id,
            (float)width, (float)height,
            16.0f, 16.0f,
            columns*rows
        }
    );
}

void TilesetManager::RecoverTileset(int _index,const std::string& _path, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    Console::PrintLine(_path, _level_editor_tab->editor->opened_directory_path);
    Console::PrintLine(ufo::FileSystem::GetRelativePath(_path, _level_editor_tab->editor->opened_directory_path));

    std::string relative_path = ufo::FileSystem::GetRelativePath(_path, _level_editor_tab->editor->opened_directory_path);

    engine->asset_manager.LoadTexture(_path, ".."+relative_path, true);
    int width = engine->asset_manager.textures.at(".."+relative_path).width;
    int height = engine->asset_manager.textures.at(".."+relative_path).height;

    int columns = (int)width/16;

    int rows = (int)height/16;

    int tileset_start_id = 1;

    tileset_start_id = tileset_data[_index].tileset_start_id;

    tileset_data[_index] = TilesetData{
            ".."+relative_path,
            columns,
            tileset_start_id,
            (float)width, (float)height,
            16.0f, 16.0f,
            columns*rows
        };
}

void TilesetManager::EditorTilesetWidget(UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    if(ImGui::Button("Add Tileset")){
        SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenTileset, _level_editor_tab, _level_editor_tab->engine->window, nullptr, 0, _level_editor_tab->editor->opened_directory_path.c_str(), false);
    }

    ImGui::SameLine();

    if(ImGui::Button("Brush")){
        tool = Tools::BRUSH;

        _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::EDIT_TILEMAP;
    }

    if(ImGui::Button("Eraser")){
        currently_selected_tiles = ManyTiles{{0},0,0,1,1};
        tool = Tools::BRUSH;
        _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::EDIT_TILEMAP;
    }

    if(ImGui::Button("Fill Bucket")){
        tool = Tools::FILL_BUCKET;
        _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::EDIT_TILEMAP;
    }

    if(ImGui::BeginTabBar("TilesetManager")){
        for(auto& tileset : tileset_data){
            if(!engine->asset_manager.textures.count(tileset.name)){
                if(ImGui::Button(("Recover Missing Tileset:"+ tileset.name).c_str())){
                    SDL_ShowOpenFileDialog(&UFOEngineStudio::OnRecoverTileset, _level_editor_tab, _level_editor_tab->engine->window, nullptr, 0, _level_editor_tab->editor->opened_directory_path.c_str(), false);
                    tileset_being_recovered = tileset.name;
                }

                Console::PrintLine("[UFO-Engine] TilesetManager::EditorTilesetWidget: Error, missing asset", tileset.name);
                continue;
            }

            if(ImGui::BeginTabItem(tileset.name.c_str(), &tileset.to_not_be_removed, ImGuiTabItemFlags_None)){
                if(current_tileset != tileset.name){
                    UpdateSelectedTilesetTile(tileset);
                    currently_selected_tile = tileset.tileset_start_id;
                }
                current_tileset = tileset.name;

                ImGui::Text("Contents");
                ImGui::Image(
                    (void*)(intptr_t)(engine->asset_manager.textures.at(tileset.name).id),
                    ImVec2(engine->asset_manager.textures.at(tileset.name).width, engine->asset_manager.textures.at(tileset.name).height),
                    ImVec2(0,0),
                    ImVec2(1,1)
                );

                ImVec2 mouse_pos = ImGui::GetMousePos();
                ImVec2 item_rect_pos = ImGui::GetItemRectMin();

                if(ImGui::IsItemHovered()){
                    if(ImGui::IsMouseClicked(0)){
                        _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::EDIT_TILEMAP;

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
                    }
                    if(ImGui::IsMouseDragging(0) || ImGui::IsMouseReleased(0)){
                        currently_selected_tiles.tiles.clear();

                        UpdateSelectedTilesetTile(tileset);

                        int tile_on_tile_selector = currently_selected_tile  - tileset.tileset_start_id;
                        int x = tile_on_tile_selector%tileset.columns;
                        int y = tile_on_tile_selector/tileset.columns;

                        int columns_in_buffer = x-currently_selected_tiles.column+1;
                        int rows_in_buffer = y-currently_selected_tiles.row+1;
                        //Console::PrintLine("first tile id",tileset.tileset_start_id, "rect",x, columns_in_buffer,y,rows_in_buffer);

                        int x0 = currently_selected_tiles.column;
                        int y0 = currently_selected_tiles.row;
                        int x1 = x+1;
                        int y1 = y+1;

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
                    ImVec2(item_rect_pos.x + currently_selected_tiles.column*tileset.tile_width , item_rect_pos.y + currently_selected_tiles.row*tileset.tile_height),
                    ImVec2(item_rect_pos.x +
                        currently_selected_tiles.column*tileset.tile_width+tileset.tile_width*currently_selected_tiles.number_of_columns ,
                        item_rect_pos.y +
                        currently_selected_tiles.row*tileset.tile_height+tileset.tile_height*currently_selected_tiles.number_of_rows),
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

}
