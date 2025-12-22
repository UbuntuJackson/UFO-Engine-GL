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
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/garbage_collector.h"

TilesetManager::TilesetManager(){
    tileset_data.push_back(TilesetData{
        "tileset",
        6,
        1,
        96,
        80,
        16,
        16,
        6*5
    });
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

void TilesetManager::EditorTilesetWidget(){
    if(ImGui::Button("Add Tileset")){
        //Open file dialogue here
    }

    if(ImGui::BeginTabBar("TilesetManager")){
        for(const auto& tileset : tileset_data){
            if(ImGui::BeginTabItem(tileset.name.c_str())){
                ImGui::Text("Contents");
                ImGui::Image(
                    (void*)(intptr_t)(engine->asset_manager.textures.at(tileset.name).id),
                    ImVec2(engine->asset_manager.textures.at(tileset.name).width, engine->asset_manager.textures.at(tileset.name).height),
                    ImVec2(0,0),
                    ImVec2(1,1)
                );

                ImVec2 mouse_pos = ImGui::GetMousePos();
                ImVec2 item_rect_pos = ImGui::GetItemRectMin();

                if(ImGui::IsMouseClicked(0)){


                    int clicked_pos_x = (mouse_pos.x-item_rect_pos.x)/tileset.tile_width;
                    int clicked_pos_y = (mouse_pos.y-item_rect_pos.y)/tileset.tile_height;

                    currently_selected_tile = clicked_pos_y*tileset.columns + clicked_pos_x + tileset.tileset_start_id;

                    Console::PrintLine(currently_selected_tile);
                }

                int tile_on_tile_selector = currently_selected_tile  - tileset.tileset_start_id;
                int x = tile_on_tile_selector%tileset.columns;
                int y = tile_on_tile_selector/tileset.columns;

                ImGui::GetWindowDrawList()->AddRect(
                    ImVec2(item_rect_pos.x + x*tileset.tile_width , item_rect_pos.y + y*tileset.tile_height),
                    ImVec2(item_rect_pos.x + x*tileset.tile_width+tileset.tile_width , item_rect_pos.y + y*tileset.tile_height+tileset.tile_height),
                    0xFFFF00FF, 0.0f, 0, 1.0f);

                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }

}
