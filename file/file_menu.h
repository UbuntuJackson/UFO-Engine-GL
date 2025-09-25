#pragma once
#include <memory>
#include <filesystem>
#include <functional>
#include "../widgets/button.h"
#include "../widgets/wrap_menu.h"
#include "../ufo_maths/ufo_maths.h"
#include "../ufo_engine/ufo_engine.h"
#include "../level/level.h"
#include "../widgets/label.h"
#include "../file/file.h"
#include "file_menu_button.h"
#include "../ufo_maths/ufo_maths.h"

class FileMenu : public WrapMenu{
public:
    std::string path;
    std::vector<std::string> path_stack;
    std::function<void(Widget* _w, FileMenuButton* _button, std::string _file_extension)> func_on_file_open = [](Widget* _w, FileMenuButton* _button, std::string _file_extension){
        
    };

    std::function<void(Widget* _w)> func_on_file_menu_created = [](Widget* _w){
        
    };
    
    FileMenu(Vector2f _local_position, Vector2f _size, std::string _path);

    void SetDirectory(std::string _path);
    virtual void OnSetDirectory(std::string _path);

    virtual std::unique_ptr<Theme> OnSetButtonTheme();
    virtual std::unique_ptr<Theme> OnSetButtonHoveredTheme();
    virtual std::unique_ptr<Theme> OnSetButtonHeldTheme();

    void OnSetup(Level* _level);

    virtual void OnFileMenuCreated();

    virtual void OnFileOpen(FileMenuButton* _button,std::string _file_extension);

private:
    Level* level = nullptr;

};