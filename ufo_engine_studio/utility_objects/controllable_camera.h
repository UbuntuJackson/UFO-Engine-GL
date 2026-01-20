#pragma once
#include "../../src/camera.h"
#include "../../ufo_maths/ufo_maths.h"

namespace ufo{
    class Controls;
    class Graphics;
}

namespace UFOEngineStudio{

class LevelEditorTab;
class Editor;

class ControllableCamera : public Camera{
public:

    ufo::Controls* controls = nullptr;

    ControllableCamera(Vector2f _local_position);

    void OnSpawn();

    void OnUpdate(float _delta_time);

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

};

}
