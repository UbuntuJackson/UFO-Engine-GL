#pragma once
#include "../../src/actor.h"
#include "../../ufo_maths/ufo_maths.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/garbage_collector.h"

namespace UFOEngineStudio{

class Editor;
class LevelEditorTab;

class NewActorPlaceHolder : public ufo::Actor{
public:

    NewActorPlaceHolder(Vector2f _);

    void OnSpawn() override;

    void OnUpdate(float _dt) override;

    ufo::Actor* OnGetFocusedActor(Vector2f _mouse_position_over_screenspace) override;

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

};

}
