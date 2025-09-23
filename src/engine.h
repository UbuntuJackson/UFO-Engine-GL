#pragma once
#include <memory>
#include <chrono>
#include "actor.h"
#include "graphics.h"
#include "input.h"
#include "openglv4_5_asset_manager.h"
#include "level.h"
#include "control_settings.h"
#include "text_rendering.h"

namespace ufo{

enum PaintingMethods{
    RECTANGLE_BRUSH_glTexSubImage2D,
    RECTANGLE_BRUSH_PixelBufferObject
};

class Engine{
public:
    bool quit = false;
    const std::string engine_path = "../UFO-Engine";

    //Width and height of window
    int width;
    int height;

    float elapsedTime = 0.0f;
    float fLastElapsed = 0.0f;
    std::chrono::time_point<std::chrono::system_clock> m_tp1, m_tp2;

    std::unique_ptr<Level> level = nullptr;

    ufo::TextRenderer text_renderer;

    //For opengl 4.5
    OpenGLv4_5_AssetManager asset_manager;
    // watch out, nullptr
    std::unique_ptr<Graphics> graphics = nullptr;
    Keyboard keyboard;
    ControlSettings control_settings;
    Mouse mouse;

    Engine(int _width, int _height);
    void Init();

    virtual ~Engine();
    
    void Update();
    void UpdatePixelGameEngine(float _delta_time);

    void BrushTest();

    void Render();
};

}