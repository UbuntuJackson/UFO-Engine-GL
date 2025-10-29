#pragma once

#include "file_node.h"

namespace UFOEngineStudio{

class Editor;

class Directory : public FileNode{
public:
    Directory(bool _is_new_directory = false) : is_new_directory{_is_new_directory} {}

    bool is_new_directory = false;
    //_file_index : the number of the file in it's local directory
    //_parent : The local directory the file or directory resides in
    //_path : the full path relative to the working directory
    //_program : the program state with it's various variables.
    void Update(int _file_index, Directory*,std::string path , Editor* _editor);
    Directory() = default;

    Directory(const Directory&) = delete;
};

}