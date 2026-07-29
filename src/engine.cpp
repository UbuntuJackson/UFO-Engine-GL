#define SDL_MAIN_HANDLED
#include <exception>
#include <memory>
#include <unordered_map>
#include "../glad/include/glad/glad.h"
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_events.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "opengl_debug_output.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "../utils/console.h"
#include "openglv4_5_asset_manager.h"
#include "openglv4_5_graphics.h"
#include "engine.h"
#include "input.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/object.h"
#include "level_loader.h"
#include "../utils/ufo_benchmarker.h"

#ifdef UFO_ENGINE_STUDIO
//Imgui
#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl3.h"
#endif

namespace ufo{

Engine::Engine()
{

}

Engine::~Engine(){
    asset_manager.Clear();

    //Null some resources to make sure for example SDL font resources aren't freed later than the SDL ttf libraries themseleves.
    loaded_levels.clear();
    level_handle = nullptr;

    TTF_CloseFont(font);
    TTF_Quit();

    MIX_Quit();

    SDL_GL_DestroyContext(open_gl_context);
    SDL_GL_UnloadLibrary();
    SDL_DestroyWindow(window);
    SDL_Quit();

    Console::PrintLine("UFO-Engine exited",__UFO_PRETTY_FUNCTION__);
}

void Engine::InitIndependant(){
    bool vsync_on = true;
    unsigned int game_width = 1600;
    unsigned int game_height = 800;
    std::string window_title = "";

    if(ufo::FileSystem::FileExists("../settings.json")){

        class SettingsReader : public ufo::gc::Root{
            public:
            void Read(const std::string& _path, std::string& _window_title, bool& _v_sync, bool& _multi_player, unsigned int& _game_width, unsigned int& _game_height){
                auto j_settings = gc::JsonRead(&gc, _path);

                if(!j_settings->map.count("vsync")) j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(0.0f);
                if(!j_settings->map.count("multi_player")) j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(0.0f);
                if(!j_settings->map.count("game_width")) j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(1600.0f);
                if(!j_settings->map.count("game_height")) j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(900.0f);
                if(!j_settings->map.count("game_window_title")) j_settings->map["game_window_title"] = gc.New<ufo::gc::JsonString>("");

                _v_sync = (bool)j_settings->map["vsync"]->AsFloat();
                _multi_player = (bool)j_settings->map["multi_player"]->AsFloat();
                _game_width = (int)j_settings->map["game_width"]->AsFloat();
                _game_height = (int)j_settings->map["game_height"]->AsFloat();
                _window_title = j_settings->map["game_window_title"]->AsString();
            }
        };

        SettingsReader r;
        r.Read("../settings.json", window_title, vsync_on, multi_player, game_width, game_height);

    }

    width = game_width;
    height = game_height;

    window = nullptr;
    //SDL_GL_Context is unassigned here

    if(!SDL_Init(SDL_INIT_VIDEO)){
        Console::PrintLine("Couldn't initialise SDL", SDL_GetError());
        exit(2);
    }

    //Tutorial says "Default OpenGL is fine."
    //That makes NO sense to me as they JUST said they were going to
    // show how to use OpenGL 4.5
    SDL_GL_LoadLibrary(nullptr);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    //Request a depth buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    window = SDL_CreateWindow(window_title.c_str(), game_width, game_height, SDL_WINDOW_OPENGL);

    if(window == nullptr){
        Console::PrintLine("Window is null");
        exit(2);
    }

    if(!MIX_Init()){
        Console::PrintLine("Engine::InitIndependant: Failed to initialise SDL_mixer. MIX_Init failed.");
    }

    SDL_SetWindowResizable(window, true);

    open_gl_context = SDL_GL_CreateContext(window);
    if(open_gl_context == nullptr){
        Console::PrintLine("Failed to create context");
        exit(2);
    }

    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    Console::PrintLine("Vendor", glGetString(GL_VENDOR));
    Console::PrintLine("Renderer", glGetString(GL_RENDERER));
    Console::PrintLine("Version", glGetString(GL_VERSION));

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if(flags & GL_CONTEXT_FLAG_DEBUG_BIT){
        Console::PrintLine("initialise debug output...");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    //vsync
    SDL_GL_SetSwapInterval(int(vsync_on));

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0,0,game_width,game_height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Everything to do with SDL_ttf

    if(!TTF_Init()){
        Console::PrintLine("Failed to initialise SDL_ttf", SDL_GetError());
    }

    font = TTF_OpenFont("../UFO-Engine/res/fonts-japanese-gothic.ttf",30);
    if(!font){
        Console::PrintLine("Failed to load font", SDL_GetError());
    }

    loaded_levels.push_back(std::make_unique<Level>());
}

void Engine::Start(){

    level_handle = loaded_levels[0]->DynamicCast<Level>();

    //text_renderer.Init(this);
    //Reserve space for a few dozens of actors or so
    level_handle->actors.reserve(50);
    level_handle->engine = this;

    actor_generator->Initialise();

    m_tp1 = std::chrono::system_clock::now();

    graphics = std::make_unique<ufo::OpenGLv4_5_Graphics>(this);
    //if(_custom_engine.get() != nullptr) engine = std::move(_custom_engine);

    asset_manager.Initialise(this);

    level_handle->Load();
    level_handle->OnSpawn();


    while(!quit){
        SDL_Event event;

        mouse.ResetTemporaryStates();

        while(SDL_PollEvent(&event)){

            if(event.type == SDL_EVENT_QUIT){
                quit = true;
            }

            keyboard.CheckEvents(event);

            mouse.CheckEvents(event);

        }

        Update();
        keyboard.ClearPressedAndReleased();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Render();

        GarbageCollect();

        SDL_GL_SwapWindow(window);

    }

}

#ifdef UFO_ENGINE_STUDIO

void Engine::StartWithImGui(){

    SDL_SetWindowTitle(window, "UFO-Engine Studio (version 0.x.x)");

    level_handle = loaded_levels[0]->DynamicCast<Level>();

    //text_renderer.Init(this);
    //Reserve space for a few dozens of actors or so
    level_handle->actors.reserve(50);
    level_handle->engine = this;

    actor_generator->Initialise();

    m_tp1 = std::chrono::system_clock::now();

    in_editor = true;
    graphics = std::make_unique<ufo::OpenGLv4_5_Graphics>(this);

    graphics->CreateFrameBuffer();

    //Should this be uncommented?
    //engine->asset_manager.Initialise(engine.get());

    level_handle->Load();

    /*ForImGUI*/
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //Imgui stuff

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, open_gl_context);
    const char* glsl_version = "#version 450";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details. If you like the default font but want it to scale better, consider using the 'ProggyVector' from the same author!
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.

    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("/etc/alternatives/fonts-japanese-mincho.ttf");

    io.Fonts->AddFontFromFileTTF("../UFO-Engine/res/fonts-japanese-gothic.ttf", 12);

    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // Our state
    [[maybe_unused]] bool show_demo_window = false;
    [[maybe_unused]] bool show_another_window = false;

    /*ForIMGUI END*/

    while(!quit){
        SDL_Event event;

        mouse.ResetTemporaryStates();

        while(SDL_PollEvent(&event)){

            /*ForImGUi*/

            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                quit = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                quit = true;

            /*ForImGUI END*/

            if(event.type == SDL_EVENT_QUIT){
                quit = true;
            }

            keyboard.CheckEvents(event);

            mouse.CheckEvents(event);

        }

        /*ForImGUI*/

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        Update();
        keyboard.ClearPressedAndReleased();

        // Rendering
        ImGui::Render();

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        SDL_GL_SwapWindow(window);

    }

    asset_manager.SaveAssets();

}

#endif //UFO_ENGINE_STUDIO

bool Engine::GoToLevel(const std::string& _path){

    try{
        ufo::BenchMarker level_loading_time;

        auto loaded_level = ufo::LevelLoader().LoadLevel(this, _path);

        //Pushes the loaded level to loaded_levels just to make it not go out of memory.
        loaded_levels.push_back(std::move(loaded_level));
        Console::PrintLine("Engine::GoToLevel",loaded_levels.back()->editor_name, "loaded in", level_loading_time.Stop()/1000000000.0f, "seconds");

        pending_levels.push_back(loaded_levels.back().get());
    }
    catch(const std::exception& _error){
        Console::PrintLine(_error.what());
        return false;

    }

    return true;

}

struct BrushRectangle{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    bool is_on_canvas = true;
};

BrushRectangle CutOutRectangle(BrushRectangle _borders, BrushRectangle _brush){

    //check if all  points in rectangle

    int new_x = _brush.x;
    int new_y = _brush.y;
    int new_w = _brush.w;
    int new_h = _brush.h;

    if(_brush.x < 0){
        new_w = _brush.w + _brush.x;
        new_x = 0;
    }
    if(_brush.y < 0){
        new_h = _brush.h + _brush.y;
        new_y = 0;
    }
    if(_brush.x + _brush.w > _borders.w){
        new_w = _borders.w - _brush.x;
    }
    if(_brush.y + _brush.h > _borders.h){
        new_h = _borders.h - _brush.y;
    }

    if(new_w < 0 || new_h < 0){
        return BrushRectangle{new_x, new_y, new_w, new_h, false};
    }

    return BrushRectangle{new_x, new_y, new_w, new_h};

}

void Engine::EditorUpdate(){

    // Handle Timing
    m_tp2 = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedTime = m_tp2 - m_tp1;
    m_tp1 = m_tp2;

    // Our time per frame coefficient
    float fElapsedTime = elapsedTime.count();
    fLastElapsed = fElapsedTime;

    level_handle->EditorUpdatePhase(fLastElapsed);
}

void Engine::Update(){
    //if(keyboard.GetKey(SDLK_ESCAPE).is_pressed) quit = true;

    // Handle Timing
    m_tp2 = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedTime = m_tp2 - m_tp1;
    m_tp1 = m_tp2;

    // Our time per frame coefficient
    float fElapsedTime = elapsedTime.count();
    fLastElapsed = fElapsedTime;

    if(fLastElapsed > 0.05f) fLastElapsed = 0.05f;

    level_handle->UpdatePhase(fLastElapsed);

    if(pending_levels.size() > 0){

        for(int l = (int)loaded_levels.size()-1; l != -1; l--){
            ufo::Level* loaded_level = loaded_levels[l]->DynamicCast<ufo::Level>();

            bool level_in_pending_levels = false;
            for(ufo::Actor* level : pending_levels){
                if(loaded_level == level){
                    level_in_pending_levels = true;
                }
            }

            if(!level_in_pending_levels && !loaded_level->persistent_on_level_transition){
                Console::PrintLine("[UFO-Engine] Engine::Update: Freeing level",loaded_level->editor_name);
                loaded_levels.erase(loaded_levels.begin()+l);
            }
        }

        //Do everything needed to initialise a level.
        level_handle = pending_levels.back()->DynamicCast<Level>();
        level_handle->actors.reserve(50);
        level_handle->engine = this;
        level_handle->Load();
        level_handle->OnSpawn();
        pending_levels.pop_back();
    }
    //Console::PrintLine(1.0f/fLastElapsed);
}

void Engine::UpdatePixelGameEngine(float _delta_time){

    level_handle->UpdatePhase(_delta_time);

}

void Engine::BrushTest(){
    if(mouse.is_left_button_held){

        auto& texture = asset_manager.textures.at("face");

        int brush_width = 64;
        int brush_height = 64;

        BrushRectangle br = CutOutRectangle(BrushRectangle{0,0,int(texture.width), int(texture.height)}, BrushRectangle{int(mouse.position.x), int(mouse.position.y), brush_width, brush_height});

        if(br.is_on_canvas){

            unsigned int new_data[br.h][br.w];

            for(int y = 0; y < br.h; y++){
                for(int x = 0; x < br.w; x++){
                    new_data[y][x] = 0xFF0000FF;
                }
            }

            glTexSubImage2D(GL_TEXTURE_2D, 0, br.x, br.y, br.w, br.h, GL_RGBA, GL_UNSIGNED_BYTE, (void*)new_data);
        }

    }

    unsigned int pixel_data[4*1] = {0,0,0,0};

    glReadPixels(0,0, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, (void*)(&pixel_data));

    //for(int i = 0; i < 4*1; i++) Console::Print(GLuint(pixel_data[i]), " ");
    //Console::PrintLine("");

}

void Engine::Render(){

    level_handle->DrawPhase(graphics.get(),width, height);

}

void Engine::EditorRender(){
    level_handle->EditorDrawPhase();
}

void Engine::GarbageCollect(){
    level_handle->InvokeGarbageCollector();
}

}
