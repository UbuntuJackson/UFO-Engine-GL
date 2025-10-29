#pragma once
#include <string>
#include <vector>
#include "../console/console.h"
#include <filesystem>
#include "../ufo_engine_studio/program_state.h"
#include "../file/file.h"

namespace UFOProjectManager{

    inline void WriteDefaultCMakeLists_txt(UFOEngineStudio::ProgramState* _program_state){
        if(!File::Exists(_program_state->working_directory_path+"/CMakeLists.txt")){
        
            File f = File();
            f.Insert("cmake_minimum_required(VERSION 3.10)\n"
                    "project(OUT)\n"
                    "add_subdirectory(UFO-Engine) #CMakeLists.txt for engine is in folder called \"engine\"\n"
                    "set(\n"
                    "    SRC\n"
                    "    main.cpp"
                    ")\n"
                    "add_executable(${PROJECT_NAME} ${SRC})\n"
                    "target_link_libraries(${PROJECT_NAME} PUBLIC UFO)\n");
        
            f.Write(_program_state->working_directory_path+"/CMakeLists.txt");
        }
    }

    inline void Build(UFOEngineStudio::ProgramState* _program_state){
        
        UFOProjectManager::WriteDefaultCMakeLists_txt(_program_state);

        if(!File::Exists(_program_state->working_directory_path+"/build")) system(std::string("cd "+_program_state->working_directory_path + " && mkdir build && cd build && cmake .. && make -j6 && (./OUT 2>&1 | tee engine_log.txt)").c_str());
        else system(std::string("cd "+_program_state->working_directory_path + " && cd build && cmake .. && make -j6 && (./OUT 2>&1 | tee engine_log.txt)").c_str());

        Console::PrintLine(_program_state->working_directory_path+"/build");
    }
}

/*class InstanceProperty{
public:
    std::string identifier;
    std::string alias;

};

class InstancePropertyInt : public InstanceProperty{
public:
    int value;
};

class InstancePropertyIntRange : public InstanceProperty{
public:
    int value;
    int lower_boundary = 0;
    int upper_boundary = 100;
};

class InstancePropertyFloat : public InstanceProperty{
public:
    float value;
};

class InstancePropertyFloatRange : public InstanceProperty{
public:
    float value;
    float lower_boundary = 0.0f;
    float upper_boundary = 100.0f;
};

class Variable{
public:
    struct Attribute{
        struct AttributeArgument{
            std::string data_type;
            std::string value;
        };

        std::string name;
        std::vector<AttributeArgument> args;
    };

    std::string data_type;
    std::string name;
    std::string default_value;

    std::vector<Attribute> attributes;

    void Print(){
        for(const auto& attribute : attributes){
            Console::Print("Attribute:",attribute.name, "= ");
            for(const auto& arg : attribute.args){
                Console::Print(arg.data_type, ":", arg.value, "\n");
            }
        }
        Console::PrintLine("Variable:",data_type, name, "=", default_value);
    }
};

class UFOProjectManager{
public:
    
    UFOProjectManager(){
        auto v = Variable();
        v.name = "interval_milliseconds";
        v.data_type = "int";
        v.default_value = "1000";
        v.attributes = {
            Variable::Attribute{
                "alias",
                {Variable::Attribute::AttributeArgument{"String", "spawn interval (milliseconds)"}}
            }
        };

        v.Print();
    }

};*/