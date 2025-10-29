#pragma once
#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include "../utils/console.h"

namespace UFOEngineStudio{

class Editor;
class Directory;
class FileNode{
public:

    static inline int file_id_counter = 0;
    int id = 0;

    bool editing_name = false;
    void TurnOnEditMode(){
        editing_name = true;
        old_file_name = file_name;
    }
    
    bool to_be_deleted = false;
    std::vector<std::unique_ptr<FileNode>> file_nodes_to_be_added_at_end_of_frame;

    std::vector<std::unique_ptr<FileNode>> file_nodes;

    std::string file_name;
    std::string old_file_name;

    std::string path_for_drag_drop_payload_use_only = "";

    FileNode();

    //This parses a folder
    static std::unique_ptr<FileNode> ParseFolder(std::string _path);

    virtual void Update(int _file_index, Directory* _parent, std::string path , Editor* _editor);

    void AddFileNodesRecursive();

    void DeleteFileNodesMarkedForDeletion();

    void Sort();

    void Traverse();

    void SearchForHeaderFiles(Editor* _editor, std::string _path);

    FileNode(const FileNode&) = delete;
    //FileNode(FileNode&) = delete;

};

}