#include <memory>
#include <filesystem>
#include <functional>
#include "../widgets/button.h"
#include "../widgets/wrap_menu.h"
#include "../ufo_maths/ufo_maths.h"
#include "../ufo_engine/ufo_engine.h"
#include "../level/level.h"
#include "../pingus/pingus_level.h"
#include "../widgets/label.h"
#include "../file/file.h"
#include "file_menu_button.h"
#include "../ufo_maths/ufo_maths.h"
#include "file_menu.h"

FileMenu::FileMenu(Vector2f _local_position, Vector2f _size, std::string _path) :
    WrapMenu(_local_position, _size),
    path{_path}
{
    
}

void FileMenu::SetDirectory(std::string _path){
    path = _path;
    selected_index = 0;

    for(auto&& child : children){
        child->QueueForPurge();
    }
    buttons.clear();

    for(const auto& directory_entry : std::filesystem::directory_iterator{_path}){
        Console::Out("Directory entry found");

        std::error_code ec;
        std::string s_path = std::string(directory_entry.path().string());
        if(std::filesystem::is_directory(directory_entry,ec)){

            std::string directory_name = s_path.substr(s_path.find_last_of("/")+1);

            auto b = std::make_unique<FileMenuButton>(Vector2f(0.0f, 0.0f),Vector2f(250.0f, 20.0f), directory_name, directory_entry.path().string());
            b->background_colour = olc::Pixel(150,150,80);

            b->on_pressed = [&](Widget* _w, Button* _button){

                auto fm = dynamic_cast<FileMenu*>(_w);
                fm->path_stack.push_back(fm->path);

                fm->SetDirectory(dynamic_cast<FileMenuButton*>(_button)->path);
            };

            AddChild(std::move(b));
        }
        else{
            Console::Out("is file");

            std::string file_name = s_path.substr(s_path.find_last_of("/")+1);
            auto b = std::make_unique<FileMenuButton>(Vector2f(0.0f, 0.0f),Vector2f(250.0f, 20.0f),file_name, directory_entry.path().string());
            b->background_colour = olc::Pixel(100,100,100);
            
            b->on_pressed = [&](Widget* _w, Button* _button){

                auto fm = dynamic_cast<FileMenu*>(_w);

                std::string file_path = dynamic_cast<FileMenuButton*>(_button)->path;
                fm->func_on_file_open(_w, dynamic_cast<FileMenuButton*>(_button), file_path.substr(file_path.find_last_of(".")+1));
                fm->OnFileOpen(dynamic_cast<FileMenuButton*>(_button), file_path.substr(file_path.find_last_of(".")+1));
            };
            AddChild(std::move(b));
        }

    }

    if(path_stack.size() > 0){
        auto bback = std::make_unique<FileMenuButton>(Vector2f(0.0f, 0.0f),Vector2f(250.0f, 150.0f), "..", path);

        bback->background_colour = olc::Pixel(200,90,0);

        bback->on_pressed = [](Widget* _w, Button* _button){
            auto fm = dynamic_cast<FileMenu*>(_w);
            Console::Out(fm->path_stack.back());
            std::string popped_path = fm->path_stack.back();
            fm->path_stack.pop_back();      
            fm->SetDirectory(popped_path);
            
        };
        AddChild(std::move(bback));
    }
    func_on_file_menu_created(this);
    OnFileMenuCreated();

    

    for(auto button : buttons){
        button->theme = OnSetButtonTheme();
        button->hovered_theme = OnSetButtonHoveredTheme();
        button->held_theme = OnSetButtonHeldTheme();
    }

    OnSetDirectory(_path);
}

void FileMenu::OnSetDirectory(std::string _path){
    
}

std::unique_ptr<Theme> FileMenu::OnSetButtonTheme(){
    return std::make_unique<ColourRectangleTheme>(Graphics::DARK_CYAN);
}

std::unique_ptr<Theme> FileMenu::OnSetButtonHoveredTheme(){
    return std::make_unique<ColourRectangleTheme>(Graphics::CYAN);
}

std::unique_ptr<Theme> FileMenu::OnSetButtonHeldTheme(){
    return std::make_unique<ColourRectangleTheme>(Graphics::VERY_DARK_CYAN);
}

void FileMenu::OnSetup(Level* _level){
    
    level = _level;
    
    OnFileMenuCreated();

    SetDirectory(path);

    //WrapMenu::OnLevelEnter(_level);

}

void FileMenu::OnFileMenuCreated(){
    
}

void FileMenu::OnFileOpen(FileMenuButton* _button,std::string _file_extension){

}