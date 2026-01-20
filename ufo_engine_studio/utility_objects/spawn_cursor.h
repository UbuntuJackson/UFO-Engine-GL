#pragma once
#include "../../src/actor.h"
#include "../../ufo_maths/ufo_maths.h"

namespace UFOEngineStudio{

class Editor;
class LevelEditorTab;

class SpawnCursor : public Actor{
public:

    SpawnCursor(Vector2f _);

    void OnUpdate(float _dt);

    bool OnUpdateEditorViewportFocus(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

};

}
