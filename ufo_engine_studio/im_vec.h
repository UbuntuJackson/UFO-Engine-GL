#pragma once
#include "../imgui/imgui.h"
#include "../ufo_maths/ufo_maths.h"

namespace UFOEngineStudio{

inline ImVec2 ImAdd(ImVec2 _a, ImVec2 _b){
    return ImVec2(_a.x + _b.x, _a.y + _b.y);
}

inline ImVec2 ImSub(ImVec2 _a, ImVec2 _b){
    return ImVec2(_a.x - _b.x, _a.y - _b.y);
}

inline ImVec2 FromVector2fToImVec2(Vector2f _v){
    return ImVec2(_v.x, _v.y);
}

inline Vector2f FromImVec2ToVector2f(ImVec2 _v){
    return Vector2f(_v.x, _v.y);
}

}
