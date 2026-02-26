#include <SDL3/SDL.h>
#include "console.h"
#include "editor.h"
#include <cstddef>
#include <string>
#include "file_dialogue.h"
#include "level_editor_tab.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "text_editor_tab.h"
#include "../tilemap/tileset_manager.h"
#include "../src/openglv4_5_asset_manager.h"

namespace UFOEngineStudio{

void OnOpenFolder(void *_userdata, const char * const *_filelist, int _filter){
    if(*_filelist == nullptr) return; //Should file not have been selected

    Editor* program = (Editor*)_userdata;

    program->OpenFolder(*_filelist);

}

void OnNewActorFile(void *_tab, const char * const *_filelist, int _filter){
    if(*_filelist == nullptr) return; //Should file not have been selected
    LevelEditorTab* tab = (LevelEditorTab*)_tab;

    std::string name = std::string(*_filelist).substr(std::string(*_filelist).find_last_of("/")+1);

    tab->path = *_filelist;

    auto level_json = tab->this_level->GetAsJson(&(tab->gc));
    level_json->Write(tab->path);

    tab->Refresh();

}

void OnOpenTileset(void *_userdata, const char * const *_filelist, int _filter){
    if(*_filelist == nullptr) return; //Should file not have been selected

    UFOEngineStudio::LevelEditorTab* level_editor_tab = (UFOEngineStudio::LevelEditorTab*)_userdata;

    level_editor_tab->this_level->tileset_manager.AddTileset(*_filelist, level_editor_tab);

}

void OnRecoverTileset(void *_userdata, const char * const *_filelist, int _filter){
    if(*_filelist == nullptr) return; //Should file not have been selected

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

void OnOpenTexture(void *_userdata, const char * const *_filelist, int _filter){
    if(*_filelist == nullptr) return; //Should file not have been selected

    while(*_filelist != nullptr){
        Console::PrintLine(*_filelist);
        UFOEngineStudio::LevelEditorTab* level_editor_tab = (UFOEngineStudio::LevelEditorTab*) _userdata;

        level_editor_tab->engine->asset_manager.OnAddTexture(*_filelist, level_editor_tab);

        _filelist++;
    }

}

void OnNewTextFile(void *_tab, const char * const *_filelist, int _filter){
    if(*_filelist == nullptr) return; //Should file not have been selected
    TextEditorTab* tab = (TextEditorTab*)_tab;

    std::string name = std::string(*_filelist).substr(std::string(*_filelist).find_last_of("/")+1);

    tab->path = *_filelist;

    File f;
    f.Insert(tab->text);
    f.Write(tab->path);

    tab->Refresh();

}

std::string GetFilenameFromPath(const std::string& _path){
    int last_slash_index = _path.find_last_of("/")+1;

    std::string res = _path.substr(last_slash_index);

    if (last_slash_index == _path.npos){
        return _path;
    }

    return res;
}

bool IsExtension(const std::string& _path, const std::string& _ext){
    return _path.substr(_path.find_last_of(".")) == "."+_ext;
}
}
