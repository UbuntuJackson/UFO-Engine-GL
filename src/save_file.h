#pragma once
#include "../ufo_garbage_collector/gc_json.h"
#include "../utils/file_utils.h"
#include "level.h"

namespace ufo{

class SaveFile : ufo::gc::Root{
public:
    std::string save_file_path;

    SaveFile(std::string _save_file_path) : save_file_path{_save_file_path}{}

    virtual void OnLoad(ufo::gc::JsonMap* _save_file){

    }

    virtual ufo::gc::JsonMap* OnSave(){
        return gc.New<ufo::gc::JsonMap>();
    }

    void Load(){
        if(ufo::FileSystem::FileExists(save_file_path)) OnLoad(ufo::gc::JsonRead(&gc,save_file_path));
    }

    void Save(){
        ufo::gc::JsonMap* save_file = OnSave();
        save_file->Write(save_file_path);
    }

};

}
