#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_events.h>
#include "../utils/console.h"
#include "../glad/include/glad/glad.h"
#include <SDL3/SDL_opengl.h>
#include "../utils/file.h"
#include "../src/openglv4_5_asset_manager.h"
#include "../src/engine.h"
#include "../src/input.h"
#include "../src/opengl_debug_output.h"
#include "../external/olcPixelGameEngine.h"
#include "../src/sprite_renderer.h"
#include "Main.h"

#ifndef USE_PGE

namespace ufo{

Main::Main(){

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

    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    window = SDL_CreateWindow("Hello GL", 800,600, SDL_WINDOW_OPENGL);

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

    /*int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if(flags & GL_CONTEXT_FLAG_DEBUG_BIT){
        Console::PrintLine("initialise debug output...");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }*/

    //vsync
    SDL_GL_SetSwapInterval(int(vsync_on));

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    int w = 0;
    int h = 0;
    SDL_GetWindowSize(window, &w,&h);

    glViewport(0,0,w,h);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //This is where the main loop was before with the engine
}

void Main::Start(std::unique_ptr<Engine> _custom_engine){
    engine = std::move(_custom_engine);

    Console::PrintLine("level memory address",engine->level.get());

    engine->graphics = std::make_unique<ufo::OpenGLv4_5_Graphics>(engine.get());
    engine->Init();
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

        SDL_GL_SwapWindow(window);
    
        //

        unsigned char data[20*20*4]= {0};

    }

    engine->asset_manager.Clear();

    SDL_GL_DestroyContext(open_gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}

#endif