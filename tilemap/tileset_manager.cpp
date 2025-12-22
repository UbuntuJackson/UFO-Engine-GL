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
        0,
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
                for(int i = 0; i < tileset.tile_count; i++){
                    int x = i%number_of_columns;
                    int y = i/number_of_columns;

                }

                Console::PrintLine("Mouse Pos",ImGui::GetWindowPos().x-ImGui::GetMousePos().x,ImGui::GetWindowPos().y-ImGui::GetMousePos().y);



                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }

}
