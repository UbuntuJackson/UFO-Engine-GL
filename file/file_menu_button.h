#pragma once

#include "../widgets/widget.h"
#include "../widgets/button.h"
#include "../external/olcPixelGameEngine.h"
#include "../keyboard/single_keyboard.h"
#include "../widgets/wrap_menu.h"

class FileMenuButton : public Button{
public:
    std::string path;

    FileMenuButton(olc::vf2d _local_position, olc::vf2d _size, std::string _text, std::string _path) : Button(_local_position, _size, _text),
    path{_path}
    {

    }

    void OnUpdate(){
        Button::OnUpdate();

        if(SingleKeyboard::Get().GetKey(olc::ESCAPE).is_pressed && text == ".."){
            on_pressed(dynamic_cast<WrapMenu*>(parent), this);
        }
    }

};