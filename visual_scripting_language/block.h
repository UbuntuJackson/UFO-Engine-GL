#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../utils/console.h"

namespace ufo::visual{

class VirtualMachine;

class Block{
public:

    static inline int id_counter = 0;

    int id = -1;

    std::vector<std::unique_ptr<Block>> blocks;

    Block(){
        id_counter++;
        id = id_counter;
    }

    virtual std::unique_ptr<Block> Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args);
    virtual std::unique_ptr<Block> Clone();

    virtual void Draw(){

    }

    virtual void Print(){}
};

//Definitionblocks

class FunctionDefinitionBlock : public Block{
public:
    std::map<std::string, std::unique_ptr<Block>> parameters;

    std::vector<std::unique_ptr<Block>> contents;

    std::unique_ptr<Block> Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) override;

    virtual void Draw(){
        bool open = ImGui::TreeNode("<Block>");
        if(open){
            for(const auto& content : contents){
                content->Draw();
            }

            ImGui::TreePop();
        }

    }
};

class VariableDefinitionBlock : public Block{
public:
    std::string name;

    std::unique_ptr<Block> value;

    std::unique_ptr<Block> Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) override;

    void Draw(){
        ImGui::InputText(std::string("###"+std::to_string(id)).c_str(),&name);
        ImGui::SameLine();
        value->Draw();

    }

};

//Call blocks

class FunctionCallBlock : public Block{
public:
    std::map<std::string,std::unique_ptr<Block>> arguments;
    std::string name;

    std::unique_ptr<Block> Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) override;
};

class FloatBlock : public Block{
public:
    float value;
    FloatBlock(float _value) : value{_value}{}
    virtual std::unique_ptr<Block> Clone(){
        return std::make_unique<FloatBlock>(value);
    }
    void Draw(){
        ImGui::InputFloat(std::string("###"+std::to_string(id)).c_str(),&value);

    }

    void Print(){
        Console::PrintLine("ufo::visual",value);
    }
};

class PrintBlock : public Block{
public:
    std::unique_ptr<Block> value;

    std::unique_ptr<Block> Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args);

    void Draw(){
        ImGui::Text(std::string("print###id"+std::to_string(id)).c_str()); ImGui::SameLine();
        value->Draw();

    }
};

class AddFloatBlock : public Block{
public:

    std::unique_ptr<Block> Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) override;
};

class NullBlock : public Block{
public:

    std::unique_ptr<Block> Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) override;
};

class ReturnBlock : public Block{
public:

    std::unique_ptr<Block> Operation(VirtualMachine* _vm, std::map<std::string, std::unique_ptr<Block>>& _args) override;
};

//

}
