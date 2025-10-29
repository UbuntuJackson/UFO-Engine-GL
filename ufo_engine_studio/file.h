#pragma once
#include <string>
#include "file_node.h"

namespace UFOEngineStudio{

class Editor;

class TreeFile : public FileNode{
public:
    bool is_new_file = false;

    TreeFile(bool _is_new_file = false) : is_new_file{_is_new_file}{
        
    }

    void Update(int _file_index, Directory* _parent,std::string path , Editor* _editor);

    TreeFile(const TreeFile&) = delete;

};

}