#include <cstddef>
#include <stdexcept>
#include <string>
#include "console.h"
#include "editor.h"
#include "file_dialogue.h"
#include "file_utils.h"
#include "level_editor_tab.h"
#include "level_loader.h"
#include "openglv4_5_graphics.h"
#include "text_editor_tab.h"
#include "../tilemap/tileset_manager.h"
#include "../src/openglv4_5_asset_manager.h"
#include "make_release_build.h"
#include "../tilemap/tile_map.h"
#include "../tilemap/tileset_data.h"
#include "../src/ufo_macros.h"
#include "../utils/file_utils.h"

namespace UFOEngineStudio{

void OnSelectStartLevel(void *_userdata, const char * const *_filelist, [[maybe_unused]] int _filter){
    if(_filelist == nullptr){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error occurred trying to select start level");
        return; //Should file not have been selected
    }
    else if(*_filelist == nullptr) return; //Should file not have been selected

    Editor* editor = (Editor*)_userdata;
    editor->project_settings.start_level = ufo::FileSystem::GetRelativePath(std::string(*_filelist), editor->opened_directory_path);

}

void OnOpenFolder(void *_userdata, const char * const *_filelist, [[maybe_unused]] int _filter){
    if(_filelist == nullptr){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error occurred trying to open file");
        return; //Should file not have been selected
    }
    else if(*_filelist == nullptr) return; //Should file not have been selected

    Editor* program = (Editor*)_userdata;

    std::string my_path = std::string(*_filelist);
    my_path = std::filesystem::path(my_path).generic_string();

    program->OpenFolder(my_path);

}

void OnOpenAutoTileFile(void *_userdata, const char * const *_filelist, [[maybe_unused]] int _filter){
    if(_filelist == nullptr){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error occurred trying to open file");
        return; //Should file not have been selected
    }
    else if(*_filelist == nullptr) return; //Should file not have been selected

    ufo::TileMap* tile_map = (ufo::TileMap*)_userdata;

    std::string my_path = std::string(*_filelist);
    my_path = std::filesystem::path(my_path).generic_string();

    auto autotile_level_json = ufo::LevelLoader().LoadLevel(tile_map->engine, my_path);

    autotile_level_json->AddNewActors();
    try{
        auto auto_tile_tile_map = autotile_level_json->GetActor("Tiles");
        tile_map->auto_tiling_tilemap = auto_tile_tile_map->DynamicCast<ufo::TileMap>();
        tile_map->auto_tiling_tilemap_owner = std::move(autotile_level_json);
        tile_map->autotiling_file = my_path;
    }
    catch(const std::runtime_error& _error){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__,"Should push an error dialog here, but for now, no layer named Tiles inside of "+my_path);
    }

}

void OnNewActorFile(void *_tab, const char * const *_filelist,[[maybe_unused]] int _filter){
    if(_filelist == nullptr){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error occurred trying to open file");
        return; //Should file not have been selected
    }
    else if(*_filelist == nullptr) return; //Should file not have been selected

    LevelEditorTab* tab = (LevelEditorTab*)_tab;

    std::string name = std::string(*_filelist).substr(std::string(*_filelist).find_last_of("/")+1);

    tab->path = *_filelist;

    auto level_json = tab->this_level->GetAsJson(&(tab->gc));
    level_json->Write(tab->path);

    tab->Refresh();

}

void OnOpenTileset(void *_userdata, const char * const *_filelist, [[maybe_unused]] int _filter){
    if(_filelist == nullptr){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error occurred trying to open file");
        return; //Should file not have been selected
    }
    else if(*_filelist == nullptr) return; //Should file not have been selected

    UFOEngineStudio::LevelEditorTab* level_editor_tab = (UFOEngineStudio::LevelEditorTab*)_userdata;

    level_editor_tab->editor->queued_tilesets.push_back(ufo::FileSystem::GetGenericString(*_filelist));
    level_editor_tab->editor->finished_importing_assets = false;

}

void OnRecoverTileset(void *_userdata, const char * const *_filelist, [[maybe_unused]] int _filter){
    if(_filelist == nullptr){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error occurred trying to open file");
        return; //Should file not have been selected
    }
    else if(*_filelist == nullptr) return; //Should file not have been selected

    UFOEngineStudio::LevelEditorTab* level_editor_tab = (UFOEngineStudio::LevelEditorTab*)_userdata;

    std::string tile_set = *_filelist;

    int index = 0;
    for(auto&& tileset : level_editor_tab->this_level->tileset_manager.tileset_data){
        if(tileset.name == level_editor_tab->this_level->tileset_manager.tileset_being_recovered){
            level_editor_tab->this_level->tileset_manager.RecoverTileset(index, tile_set, level_editor_tab);
        }
        index++;
    }

}

void OnOpenTexture(void *_userdata, const char * const *_filelist, [[maybe_unused]] int _filter){
    if(_filelist == nullptr){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error occurred trying to open file");
        return; //Should file not have been selected
    }
    else if(*_filelist == nullptr) return; //Should file not have been selected

    UFOEngineStudio::LevelEditorTab* level_editor_tab = (UFOEngineStudio::LevelEditorTab*) _userdata;

    while(*_filelist != nullptr){
        Console::PrintLine(*_filelist);

        level_editor_tab->editor->queued_textures.push_back(ufo::FileSystem::GetGenericString(*_filelist));

        _filelist++;
    }

    level_editor_tab->editor->finished_importing_assets = false;

}

void OnOpenShader(void *_userdata, const char * const *_filelist, [[maybe_unused]] int _filter){
    if(_filelist == nullptr){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error occurred trying to open file");
        return; //Should file not have been selected
    }
    else if(*_filelist == nullptr) return; //Should file not have been selected

    UFOEngineStudio::LevelEditorTab* level_editor_tab = (UFOEngineStudio::LevelEditorTab*) _userdata;

    while(*_filelist != nullptr){
        Console::PrintLine(*_filelist);

        level_editor_tab->editor->queued_shaders.push_back(ufo::FileSystem::GetGenericString(*_filelist));

        _filelist++;
    }

    level_editor_tab->editor->finished_importing_assets = false;

}

void OnNewTextFile(void *_tab, const char * const *_filelist, [[maybe_unused]] int _filter){
    if(_filelist == nullptr){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error occurred trying to open file");
        return; //Should file not have been selected
    }
    else if(*_filelist == nullptr) return; //Should file not have been selected

    TextEditorTab* tab = (TextEditorTab*)_tab;

    std::string name = std::string(*_filelist).substr(std::string(*_filelist).find_last_of("/")+1);

    tab->path = *_filelist;

    ufo::FileSystem::Write(tab->path, tab->text);

    tab->Refresh();

}

void OnSelectDirectoryForDebugBuild(void *_editor, const char * const *_filelist, [[maybe_unused]] int _filter){

    if(_filelist == nullptr){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error occurred trying to open file");
        return; //Should file not have been selected
    }
    else if(*_filelist == nullptr) return; //Should file not have been selected

    UFOEngineStudio::Editor* editor = (UFOEngineStudio::Editor*)_editor;

    MakeReleaseBuild(editor->opened_directory_path, std::string(*_filelist));

}

}
