#include <string>
#include "../console/console.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "../json/json_variant.h"
#include "level_editor_tab.h"
#include <memory>
#include "program_state.h"
#include "text_editor_tab.h"
#include "actor_class.h"
#include "property.h"
#include "../ufo_project_manager/ufo_project_manager_header_tool.h"
#include "asset_manager.h"
#include "../file/file.h"

namespace UFOEngineStudio{

ProgramState::ProgramState(SDL_Renderer* _renderer) : asset_manager(_renderer){
    SDL_Surface* example_surface = IMG_Load("../res/placeholder_icon.png");
    assert(example_surface != nullptr);
    example_texture = SDL_CreateTextureFromSurface(_renderer ,example_surface);
    assert(example_texture != nullptr);
    SDL_DestroySurface(example_surface);

    project.actor_classes.push_back(ActorClass{"Actor", "<actor.h>"});
    project.actor_classes.push_back(ActorClass{"Animation", "<animation.h>"});
    project.actor_classes.push_back(ActorClass{"Sprite", "<sprite.h>"});
    project.actor_classes.push_back(ActorClass{"Camera", "<camera.h>"});
    project.actor_classes.push_back(ActorClass{"TileMap", "<tile_map.h>"});

}

void ProgramState::WriteProjectFile(std::string _path){
    Console::PrintLine("Write project file:", _path);

    JsonDictionary project_json;
    project_json.Set("project_name", project.name);
    project_json.Set("actor_classes", JsonArray());
    project_json.Set("actor_categories", JsonArray());

    project_json.Write(_path);

    //Making sure the project is loaded.
    //OpenProjectFile(_path);
}

void ProgramState::OpenProjectFile(std::string _path){
    Console::PrintLine("Open project file:", _path);

    JsonDictionary project_json = JsonVariant::Read(_path);

    project = Project{true, project_json.Get("project_name").AsString()};

    auto& actor_classes = project_json.Get("actor_classes").AsArray().Iterable();

    for(auto&& actor_class : actor_classes){
        std::string actor_class_name = actor_class->AsDictionary().Get("name").AsString();
        Console::PrintLine("Loading actor class:", actor_class_name);
        //project.actor_categories.push_back(ActorClass{actor_class_name});
    }

}

void ProgramState::OpenFolder(std::string _path){
    opened_directory = FileNode::ParseFolder(_path);
    opened_directory->file_name = "";
    //opened_directory->id = -1;
    working_directory_path = _path;
}

void ProgramState::SearchWorkingDirectoryForExposedActorClasses(){
    log_text = "";
    Console::PrintLine("Refreshing working directory:", working_directory_path);
    opened_directory->SearchForHeaderFiles(this, "");
}

void ProgramState::ImportHeaderFileToProject(std::string _path){

    //int result = system((std::string("cd .. && cd header_tool && python3 project.py ")+_path).c_str());
    //if(result) Console::PrintLine("ProgramState::ImportHeaderFileToProject() Result returned true");
    
    std::string file_name = _path.substr(_path.find_last_of("/")+1);

    log_text += "Looking in header file: "+_path+"\n";

    auto messages = UFOProjectManagerHeaderTool::Run(_path);

    for(const auto& item : messages){
        log_text += item + "\n";
    }

    std::string exported_variables_supposed_location = _path + "_exported_variables.json";

    if(!File::Exists(exported_variables_supposed_location)) return;

    JsonDictionary d = JsonVariant::Read(exported_variables_supposed_location);

    std::string class_name = "";
    std::vector<std::unique_ptr<Property>> exported_properties;
    for(const auto& variable : d.Get("exported_variables").AsArray().Iterable()){
        
        std::string local_name = variable->AsDictionary().Get("name").AsString();
        std::string data_type = variable->AsDictionary().Get("data_type").AsString();
        std::string default_value = variable->AsDictionary().Get("default_value").AsString();

        if(data_type == "class"){
            if(class_name.empty()) class_name = local_name;
            else Console::PrintLine("[x] UFO-Engine Studio: Error, two classes found in", _path);
        }
        else{
            try{
                if(data_type == "std::string"){
                    std::string text = default_value;
                    auto p = std::make_unique<PropertyString>(local_name, text);
                    exported_properties.push_back(std::move(p));
                }
                if(data_type == "bool"){
                    bool checked = (default_value == "true");
                    auto p = std::make_unique<PropertyBool>(local_name, checked);
                    exported_properties.push_back(std::move(p));
                }
                if(data_type == "int"){
                    int value = std::stoi(default_value);
                    auto p = std::make_unique<PropertyInt>(local_name, value);
                    exported_properties.push_back(std::move(p));
                }
                if(data_type == "float"){
                    float value = std::stof(default_value);
                    auto p = std::make_unique<PropertyFloat>(local_name, value);
                    exported_properties.push_back(std::move(p));
                }
            }
            catch(const std::invalid_argument& err){
                std::string error_message = std::string("[x] UFO-Engine Studio: Conversion error '")+default_value+"' is not of type '"+data_type+"'"+
                    "\n    in file: "+ _path;
                
                Console::PrintLine(error_message);
                log_text+= error_message + "\n";
            }
        }

    }

    //Produce the generated.h and sh&t

    //Erase actor class if it already exists in favour of the new version
    for(int actor_class_index = project.actor_classes.size()-1; actor_class_index >= 0; actor_class_index--){
        if(project.actor_classes[actor_class_index].name == class_name){
            project.actor_classes.erase(project.actor_classes.begin()+actor_class_index);
        }
    }

    project.actor_classes.push_back(ActorClass{class_name, "\""+file_name+"\"", true});

    for(auto&& p : exported_properties){
        
        project.actor_classes.back().exported_properties.push_back(std::move(p));
    }

    //Need to reload the working directory.
    should_refresh_working_directory = true;
    should_refresh_properties_on_all_nodes = true;
}

//This runs after the directory tree is updated
void ProgramState::Update(){

    if(File::Exists(working_directory_path+"/build/engine_log.txt")){
        std::string log_contents = File().Read(working_directory_path+"/build/engine_log.txt");

        //Console::PrintLine();

        log_text += log_contents;
        
        //File f;

        //f.Write(working_directory_path+"/build/engine_log.txt");
    }
    //This list will be the new drag_drop_stack.
    //All items that begin a dragdrop but aren't moved to a new folder end up here.
    std::vector<DragDrop> preserved;

    //This might seem unnecessary but it's to preserve indexes to avoid removing elements while iterating
    struct DragDropAndEraseIndex{
        DragDrop drag_drop;
        int erase_index;
        std::unique_ptr<FileNode> u_file_node_to_move;
    };

    std::vector<DragDropAndEraseIndex> to_be_moved;

    for(auto&& drag_drop : drag_drop_stack){
        int index = 0;

        //If it's the highest folder which does not have an owner.
        if(drag_drop.owner == nullptr) continue;

        for(auto&& u_ptr : drag_drop.owner->file_nodes){

            if(u_ptr.get() == drag_drop.file_to_be_moved){
                if(drag_drop.move_to_folder != nullptr){

                    //Here the unique_ptr is moved to establish it in the directory tree
                    to_be_moved.push_back(DragDropAndEraseIndex{drag_drop, index, std::move(u_ptr)});
                }
                else{
                    preserved.push_back(drag_drop);
                }
            }
            
            index++;
        }
    }

    for(auto&& item : to_be_moved){

        Console::PrintLine(item.drag_drop.owner_path, item.drag_drop.move_to_path+"/"+item.u_file_node_to_move->file_name);

        Console::PrintLine(item.drag_drop.owner_path+"/"+item.drag_drop.file_to_be_moved->file_name,
            item.drag_drop.move_to_path+"/"+item.drag_drop.file_to_be_moved->file_name);

        std::filesystem::rename(item.drag_drop.owner_path+"/"+item.drag_drop.file_to_be_moved->file_name,
            item.drag_drop.move_to_path+"/"+item.drag_drop.file_to_be_moved->file_name);

        item.drag_drop.owner->file_nodes.erase(item.drag_drop.owner->file_nodes.begin() +item.erase_index);
        item.drag_drop.move_to_folder->file_nodes.insert(item.drag_drop.move_to_folder->file_nodes.begin() + item.drag_drop.index_in_move_to_folder, std::move(item.u_file_node_to_move));
    }

    drag_drop_stack.clear();
    to_be_moved.clear();

    drag_drop_stack = preserved;

    //Delete file nodes marked for deletion
    if(opened_directory.get() != nullptr){

        //opened_directory->Traverse();

        opened_directory->DeleteFileNodesMarkedForDeletion();
    
        opened_directory->AddFileNodesRecursive();

        opened_directory->Sort();
    }

    if(should_refresh_working_directory){
        OpenFolder(working_directory_path);
    }
    should_refresh_working_directory = false;

    should_refresh_properties_on_all_nodes = false;

    if(should_search_working_directory_for_exposed_actor_classes) SearchWorkingDirectoryForExposedActorClasses();

    should_search_working_directory_for_exposed_actor_classes = false;

    /*std::sort(project.actor_categories.begin(), project.actor_categories.begin(), [](const ActorVariant& _a, const ActorVariant& _b){
        return _a.category < _b.category;
    });*/
}

void
ProgramState::CleanUp(){
    SDL_DestroyTexture(example_texture);
}

}