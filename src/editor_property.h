#pragma once
#include <string>
#include "../ufo_garbage_collector/garbage_collector.h"
#include "../ufo_garbage_collector/gc_json.h"

namespace ufo{

class Actor;

class EditorProperty{
public:
    std::string variable_name;
    std::string alias;
    bool to_be_removed = false;
    bool viewport_text = false;

    EditorProperty(const std::string& _variable_name, const std::string& _alias) : variable_name{_variable_name}, alias{_alias}{}

    virtual ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
        return nullptr;
    }

    virtual void Update(Actor* _actor, const std::string& _editor_name, int _index) = 0;

    virtual std::unique_ptr<EditorProperty> Copy() = 0;

    virtual ~EditorProperty() = default;
};

class EditorPropertyFloatHandle : public EditorProperty{
public:
    float* value = nullptr;

    EditorPropertyFloatHandle(const std::string& _name,const std::string& _alias, float* _value);

    void Update(Actor* _actor, const std::string& _editor_name, int _index) override;

    ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc);

    std::unique_ptr<EditorProperty> Copy();
};

class EditorPropertyIntHandle : public EditorProperty{
public:
    int* value = nullptr;

    EditorPropertyIntHandle(const std::string& _name,const std::string& _alias, int* _value);

    void Update(Actor* _actor, const std::string& _editor_name, int _index) override;

    ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc);

    std::unique_ptr<EditorProperty> Copy();
};

class EditorPropertyInt : public EditorProperty{
public:
    int value = 0;

    EditorPropertyInt(const std::string& _name,const std::string& _alias, int _value);

    void Update(Actor* _actor, const std::string& _editor_name, int _index) override;

    ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc);

    std::unique_ptr<EditorProperty> Copy();
};

class EditorPropertyFloat : public EditorProperty{
public:
    float value = 0;

    EditorPropertyFloat(const std::string& _name,const std::string& _alias, float _value);

    void Update(Actor* _actor, const std::string& _editor_name, int _index) override;

    ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc);

    std::unique_ptr<EditorProperty> Copy();
};

class EditorPropertyString : public EditorProperty{
public:
    std::string value;

    EditorPropertyString(const std::string& _name,const std::string& _alias, const std::string& _value);

    void Update(Actor* _actor, const std::string& _editor_name, int _index) override;

    ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc);

    std::unique_ptr<EditorProperty> Copy();
};

class EditorPropertyIntSlider : public EditorProperty{
public:
    int value = 0;

    int min;
    int max;

    EditorPropertyIntSlider(const std::string& _name,const std::string& _alias, int _value, int _min, int _max);

    void Update(Actor* _actor, const std::string& _editor_name, int _index) override;

    ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc);

    std::unique_ptr<EditorProperty> Copy();
};

class EditorPropertyFloatSlider : public EditorProperty{
public:
    float value;

    float min;
    float max;
    float step;

    EditorPropertyFloatSlider(const std::string& _name,const std::string& _alias, float _value, float _min, float _max, float _step);

    void Update(Actor* _actor, const std::string& _editor_name, int _index) override;

    ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc);

    std::unique_ptr<EditorProperty> Copy();
};

class EditorPropertyCheckBox : public EditorProperty{
public:
    bool value = false;

    EditorPropertyCheckBox(const std::string& _name,const std::string& _alias, bool _value);

    void Update(Actor* _actor, const std::string& _editor_name, int _index) override;

    ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc);

    std::unique_ptr<EditorProperty> Copy();
};

}
