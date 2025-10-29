#pragma once
#include "../imgui/imgui.h"

namespace UFOEngineStudio{

inline ImVec2 ImAdd(ImVec2 _a, ImVec2 _b){
    return ImVec2(_a.x + _b.x, _a.y + _b.y);
}

inline ImVec2 ImSub(ImVec2 _a, ImVec2 _b){
    return ImVec2(_a.x - _b.x, _a.y - _b.y);
}

}