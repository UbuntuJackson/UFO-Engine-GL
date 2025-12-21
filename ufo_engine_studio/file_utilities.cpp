#include <SDL3/SDL.h>
#include "editor.h"
#include <string>
#include "file_utilities.h"
#include "level_editor_tab.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "text_editor_tab.h"

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
