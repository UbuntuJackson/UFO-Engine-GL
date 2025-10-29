#pragma once
#include <string>
#include <exception>
#include <thread>
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../imgui/imgui.h"
#include "program_state.h"
#include "../console/console.h"
#include "program_state.h"
#include "file_node.h"
#include "level_editor_tab.h"
#include "asset_browser_tab.h"
#include "../ufo_project_manager/ufo_project_manager.h"

namespace UFOEngineStudio{

inline void OnWriteProjectFile(void *_userdata, const char * const *_filelist, int _filter){
    if(*_filelist == nullptr) return; //Should file not have been selected
    ((ProgramState*)_userdata)->WriteProjectFile(*_filelist);
}

inline void OnOpenProjectFile(void *_userdata, const char * const *_filelist, int _filter){
    if(*_filelist == nullptr) return; //Should file not have been selected
    ((ProgramState*)_userdata)->OpenProjectFile(*_filelist);
}

inline void OnOpenFolder(void *_userdata, const char * const *_filelist, int _filter){
    if(*_filelist == nullptr) return; //Should file not have been selected

    ProgramState* program = (ProgramState*)_userdata;

    program->OpenFolder(*_filelist);

}

inline void OnNewActorFile(void *_tab, const char * const *_filelist, int _filter){
    if(*_filelist == nullptr) return; //Should file not have been selected
    LevelEditorTab* tab = (LevelEditorTab*)_tab;

    std::string name = std::string(*_filelist).substr(std::string(*_filelist).find_last_of("/")+1);

    tab->path = *_filelist;
    tab->actor->WriteToJson().Write(*_filelist);

}

inline void MainMenuBar(ProgramState* _program){
    //ImGui::Begin("MenuBarWindow", nullptr, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            //ShowExampleMenuFile();

            if(ImGui::MenuItem("Open Folder")){
                const char* file_location = _program->working_directory_path.c_str();
                Console::PrintLine("Debug file_location:",file_location);

                SDL_ShowOpenFolderDialog(&OnOpenFolder , _program, _program->window, file_location, false);
            }

            if(ImGui::MenuItem("New Level/Actor File")){
                _program->tabs.push_back(std::make_unique<LevelEditorTab>(_program, ""));
                
                
            }

            if(ImGui::MenuItem("Save File", "CTRL+S")){ //Shortcut does not work and I have no idea why
                if(_program->active_tab) _program->active_tab->OnSave(_program);
                
            }

            if(ImGui::MenuItem("Refresh")){ //Shortcut does not work and I have no idea why
                _program->SearchWorkingDirectoryForExposedActorClasses();
                
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("New Project")) {
                _program->show_new_project_dialogue = true;

                //const char file_location[] = "/home";

                //SDL_ShowSaveFileDialog(&OnWriteProjectFile, _program, _program->window, nullptr, 0, file_location);
            }
            if (ImGui::MenuItem("Open Project")) {
                const char file_location[] = "/home";

                //try{
                    SDL_ShowOpenFileDialog(&OnOpenProjectFile, _program, _program->window, nullptr, 0, file_location, false);
                /*}
                catch(std::logic_error()){

                }*/
            }  // Disabled item
            if(ImGui::MenuItem("Build")){

                std::thread t(UFOProjectManager::Build, _program);

                t.detach();
                
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Assets"))
        {
            if (ImGui::MenuItem("Browse assets")) {
                _program->tabs.push_back(std::make_unique<AssetBrowserTab>("Asset Browser"));

            }
            
            ImGui::EndMenu();
        }

        if(_program->asset_browser_open){
            ImGui::Begin("Assets");

            ImGui::End();
        }

        if(_program->show_new_project_dialogue){
            ImGui::Begin("Create New Project...");

            ImGui::Text("Project Name:");
            ImGui::InputText("##Project Name", &(_program->pending_project_name));

            if(ImGui::Button("Cancel")){
                _program->show_new_project_dialogue = false;

                _program->pending_project_name = "";

            }
            if(ImGui::Button("Save")){
                _program->project.name = _program->pending_project_name;

                const char file_location[] = "/home";

                SDL_ShowSaveFileDialog(&OnWriteProjectFile, _program, _program->window, nullptr, 0, file_location);
                std::string new_window_title = "UFO-Engine Studio - "+ _program->project.name;
                SDL_SetWindowTitle(_program->window, new_window_title.c_str());

                _program->pending_project_name = "";

                _program->show_new_project_dialogue = false;
            }

            ImGui::End();
        }
        ImGui::EndMainMenuBar();
    }
    //ImGui::End();
    
}

};