#pragma once
#include <memory>
#include <chrono>
#include "actor.h"
#include "graphics.h"
#include "input.h"
#include "openglv4_5_asset_manager.h"
#include "level.h"
#include "control_settings.h"
//#include "text_rendering.h"
#include "generic_generator.h"
#include <SDL3/SDL.h>

namespace ufo{

enum PaintingMethods{
    RECTANGLE_BRUSH_glTexSubImage2D,
    RECTANGLE_BRUSH_PixelBufferObject
};

class Main;

class Engine{
public:
    std::string language = "English";
    std::vector<std::string> languages = {
        "English",
        "Japanese",
        "Swedish"
    };

    ufo::Texture2D sdl_text_texture;

    bool in_editor = false;
    bool quit = false;

    std::unique_ptr<GenericGenerator> actor_generator = std::make_unique<GenericGenerator>();

    //This is for when loading default assets.
    //The engine utilises the relative path to the executable location
    std::string engine_path = "../UFO-Engine";

    bool multi_player = false;

    //Width and height of window
    int width;
    int height;

    float elapsedTime = 0.0f;
    float fLastElapsed = 0.0f;
    std::chrono::time_point<std::chrono::system_clock> m_tp1, m_tp2;

    Level* level_handle = nullptr;
    std::unique_ptr<Actor> level = nullptr;
    std::vector<std::unique_ptr<Actor>> pending_levels;

    //ufo::TextRenderer text_renderer;

    //For opengl 4.5
    OpenGLv4_5_AssetManager asset_manager;
    // watch out, nullptr
    std::unique_ptr<Graphics> graphics = nullptr;
    Keyboard keyboard;
    ControlSettings control_settings;
    Mouse mouse;

    SDL_Window* window = nullptr;

    Engine();
    void Init(Main* _main);

    void Start();
    void StartWithImGui();

    bool GoToLevel(const std::string& _path);

    void ResetUFOEngineStudio();

    virtual ~Engine();

    void Update();
    void EditorUpdate();
    void UpdatePixelGameEngine(float _delta_time);

    void BrushTest();

    void Render();
    void EditorRender();

    void GarbageCollect();
};

}
