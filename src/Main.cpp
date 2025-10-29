#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_events.h>
#include "../utils/console.h"
#include "../glad/include/glad/glad.h"
#include <SDL3/SDL_opengl.h>
#include "../file/file.h"
#include "../src/openglv4_5_asset_manager.h"
#include "../src/engine.h"
#include "../src/input.h"
#include "../src/opengl_debug_output.h"
#include "../external/olcPixelGameEngine.h"
#include "../src/sprite_renderer.h"
#include "Main.h"
//Imgui
#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl3.h"

#ifndef USE_PGE

namespace ufo{

Main::Main(unsigned int _width, unsigned int _height){

    bool vsync_on = false;

    window = nullptr;
    //SDL_GL_Context is unassigned here

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
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

    window = SDL_CreateWindow("Hello GL", _width, _height, SDL_WINDOW_OPENGL);

    if(window == nullptr){
        Console::PrintLine("Window is null");
        exit(2);
    }

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

    glViewport(0,0,_width,_height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //This is where the main loop was before with the engine
}

void Main::StartWithImGui(std::unique_ptr<Engine> _custom_engine){
    engine = std::move(_custom_engine);

    Console::PrintLine("level memory address",engine->level.get());

    engine->Init(this);
    engine->graphics = std::make_unique<ufo::OpenGLv4_5_Graphics>(engine.get());

    engine->graphics->CreateFrameBuffer();
    
    //if(_custom_engine.get() != nullptr) engine = std::move(_custom_engine);

    engine->level->Load();

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

    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;

    /*ForIMGUI END*/

    while(!engine->quit){
        SDL_Event event;

        engine->mouse.ResetTemporaryStates();

        while(SDL_PollEvent(&event)){

            /*ForImGUi*/

            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                engine->quit = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                engine->quit = true;

            /*ForImGUI END*/

            if(event.type == SDL_EVENT_QUIT){
                engine->quit = true;
            }

            engine->keyboard.CheckEvents(event);

            engine->mouse.CheckEvents(event);
            
        }

        /*ForImGUI*/

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        engine->Update();
        engine->keyboard.ClearPressedAndReleased();

        // Rendering
        ImGui::Render();

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        engine->Render();

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

    engine->asset_manager.Clear();

    SDL_GL_DestroyContext(open_gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Main::Start(std::unique_ptr<Engine> _custom_engine){
    engine = std::move(_custom_engine);

    Console::PrintLine("level memory address",engine->level.get());

    engine->Init(this);
    engine->graphics = std::make_unique<ufo::OpenGLv4_5_Graphics>(engine.get());
    //if(_custom_engine.get() != nullptr) engine = std::move(_custom_engine);

    engine->level->Load();

    while(!engine->quit){
        SDL_Event event;

        engine->mouse.ResetTemporaryStates();

        while(SDL_PollEvent(&event)){

            if(event.type == SDL_EVENT_QUIT){
                engine->quit = true;
            }

            engine->keyboard.CheckEvents(event);

            engine->mouse.CheckEvents(event);
            
        }

        //Test start
        /*if(app.keyboard.GetKey(SDLK_RIGHT).is_pressed){
            Console::PrintLine("right key pressed");
        }

        if(app.keyboard.GetKey(SDLK_RIGHT).is_released){
            Console::PrintLine("right key released");
        }

        if(app.keyboard.GetKey(SDLK_LEFT).is_held){
            Console::PrintLine("left key held down, not to be confused with pressed");
        }

        if(app.mouse.is_left_button_pressed) Console::PrintLine("left mouse pressed");
        if(app.mouse.is_left_button_released) Console::PrintLine("left mouse released");*/

        //Test end

        engine->Update();
        engine->keyboard.ClearPressedAndReleased();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        engine->Render();

        engine->GarbageCollect();

        SDL_GL_SwapWindow(window);

    }

    engine->asset_manager.Clear();

    SDL_GL_DestroyContext(open_gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}

#endif