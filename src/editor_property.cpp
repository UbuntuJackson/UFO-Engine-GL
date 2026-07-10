#include <memory>
#include <string>
#include "editor_property.h"
#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "actor_undo_and_redo.h"
#include "level.h"
#include "level_editor_tab.h"

namespace ufo{

// EditorPropertyInt

EditorPropertyInt::EditorPropertyInt(const std::string& _name,const std::string& _alias, int _value) : EditorProperty(_name,_alias),
    value{_value}{

    }

void EditorPropertyInt::Update(UFOEngineStudio::LevelEditorTab* _level_editor_tab, Actor* _actor, const std::string& _editor_name, int _index) {
    int former_value = value;

    if(ImGui::InputInt(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value)){
        _actor->level->RemoveFutureChanges();
        _actor->level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableInt>(_level_editor_tab, _actor->editor_id, variable_name, former_value, value));
    }
}

ufo::gc::JsonMap* EditorPropertyInt::GetJson(ufo::GarbageCollector* _gc){
    auto m = _gc->New<ufo::gc::JsonMap>();
    m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
    m->map.emplace("type", _gc->New<ufo::gc::JsonString>("float"));
    m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(value));
    m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyInt"));
    return m;
}

std::unique_ptr<EditorProperty> EditorPropertyInt::Copy(){
    return std::make_unique<EditorPropertyInt>(variable_name,alias,value);
}

// EditorPropertyFloat

EditorPropertyFloat::EditorPropertyFloat(const std::string& _name,const std::string& _alias, float _value) : EditorProperty(_name,_alias),
    value{_value}{

    }

void EditorPropertyFloat::Update(UFOEngineStudio::LevelEditorTab* _level_editor_tab, Actor* _actor, const std::string& _editor_name, int _index){
    float former_value = value;
    if(ImGui::InputFloat(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value)){
        _actor->level->RemoveFutureChanges();
        _actor->level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableFloat>(_level_editor_tab, _actor->editor_id, variable_name, former_value, value));
    }
}

ufo::gc::JsonMap* EditorPropertyFloat::GetJson(ufo::GarbageCollector* _gc){
    auto m = _gc->New<ufo::gc::JsonMap>();
    m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
    m->map.emplace("type", _gc->New<ufo::gc::JsonString>("float"));
    m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(value));
    m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyFloat"));
    return m;
}

std::unique_ptr<EditorProperty> EditorPropertyFloat::Copy(){
    return std::make_unique<EditorPropertyFloat>(variable_name,alias,value);
}

// EditorPropertyString

EditorPropertyString::EditorPropertyString(const std::string& _name,const std::string& _alias, const std::string& _value) : EditorProperty(_name,_alias),
    value{_value}{

    }

void EditorPropertyString::Update(UFOEngineStudio::LevelEditorTab* _level_editor_tab, Actor* _actor, const std::string& _editor_name, int _index){
    ImGui::InputText(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value);
}

ufo::gc::JsonMap* EditorPropertyString::GetJson(ufo::GarbageCollector* _gc){
    auto m = _gc->New<ufo::gc::JsonMap>();
    m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
    m->map.emplace("type", _gc->New<ufo::gc::JsonString>("std::string"));
    m->map.emplace("value", _gc->New<ufo::gc::JsonString>(value));
    m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyString"));
    return m;
}

std::unique_ptr<EditorProperty> EditorPropertyString::Copy(){
    return std::make_unique<EditorPropertyString>(variable_name,alias,value);
}

// EditorPropertyIntSlider

EditorPropertyIntSlider::EditorPropertyIntSlider(const std::string& _name,const std::string& _alias, int _value, int _min, int _max) : EditorProperty(_name,_alias),
    value{_value},
    max{_max},
    min{_min}{

    }

void EditorPropertyIntSlider::Update(UFOEngineStudio::LevelEditorTab* _level_editor_tab, Actor* _actor, const std::string& _editor_name, int _index) {
    ImGui::SliderInt(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value, min, max);
}

ufo::gc::JsonMap* EditorPropertyIntSlider::GetJson(ufo::GarbageCollector* _gc){
    auto m = _gc->New<ufo::gc::JsonMap>();
    m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
    m->map.emplace("type", _gc->New<ufo::gc::JsonString>("float"));
    m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(value));
    m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyIntSlider"));
    return m;
}

std::unique_ptr<EditorProperty> EditorPropertyIntSlider::Copy(){
    return std::make_unique<EditorPropertyIntSlider>(variable_name,alias,value,min,max);
}

// EditorPropertyFloatSlider

EditorPropertyFloatSlider::EditorPropertyFloatSlider(const std::string& _name,const std::string& _alias, float _value, float _min, float _max, float _step) : EditorProperty(_name,_alias),
    value{_value},
    min{_min},
    max{_max},
    step{_step}{

    }

void EditorPropertyFloatSlider::Update(UFOEngineStudio::LevelEditorTab* _level_editor_tab, Actor* _actor, const std::string& _editor_name, int _index) {
    ImGui::SliderFloat(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value, min, max);
}

ufo::gc::JsonMap* EditorPropertyFloatSlider::GetJson(ufo::GarbageCollector* _gc){
    auto m = _gc->New<ufo::gc::JsonMap>();
    m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
    m->map.emplace("type", _gc->New<ufo::gc::JsonString>("float"));
    m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(value));
    m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyFloatSlider"));
    return m;
}

std::unique_ptr<EditorProperty> EditorPropertyFloatSlider::Copy(){
    return std::make_unique<EditorPropertyFloatSlider>(variable_name,alias,value,min,max, step);
}

// EditorPropertyCheckBox

EditorPropertyCheckBox::EditorPropertyCheckBox(const std::string& _name,const std::string& _alias, bool _value) : EditorProperty(_name,_alias),
value{_value}{

}

void EditorPropertyCheckBox::Update(UFOEngineStudio::LevelEditorTab* _level_editor_tab, Actor* _actor, const std::string& _editor_name, int _index) {
    ImGui::Checkbox(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value);
}

ufo::gc::JsonMap* EditorPropertyCheckBox::GetJson(ufo::GarbageCollector* _gc){
    auto m = _gc->New<ufo::gc::JsonMap>();
    m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
    m->map.emplace("type", _gc->New<ufo::gc::JsonString>("bool"));
    m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(value));
    m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyCheckBox"));
    return m;
}

std::unique_ptr<EditorProperty> EditorPropertyCheckBox::Copy(){
    return std::make_unique<EditorPropertyCheckBox>(variable_name,alias,value);
}

}
