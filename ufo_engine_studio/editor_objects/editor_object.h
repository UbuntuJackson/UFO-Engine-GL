#pragma once
#include "../vector2f.h"
#include "../property.h"
#include "../../imgui/imgui.h"
#include "../im_vec.h"

namespace UFOEngineStudio{


class ProgramState;

class ActorEditorObject{
public:
    
    std::string class_name = "";

    PropertyVector2f* local_position_handle = nullptr;
    
    //These are the variables that you can configure via the browser
    std::vector<std::unique_ptr<Property>> properties;

    //These are the variables that you can configure via the browser, except these properties are specifically to generate the constructor
    std::vector<std::unique_ptr<Property>> constructor_properties;

    //
    std::vector<std::unique_ptr<Property>> exported_properties;

    template<typename tType, typename ...tArgs>
    tType* AddConstructorProperty(tArgs... _args){
        auto u_property = std::make_unique<tType>(_args...);
        tType* property_ptr = u_property.get();
        constructor_properties.push_back(std::move(u_property));

        return property_ptr;
    }

    template<typename tType, typename ...tArgs>
    tType* AddProperty(tArgs... _args){
        auto u_property = std::make_unique<tType>(_args...);
        tType* property_ptr = u_property.get();
        properties.push_back(std::move(u_property));

        return property_ptr;
    }

    template<typename tType, typename ...tArgs>
    tType* AddExportedProperty(tArgs... _args){
        auto u_property = std::make_unique<tType>(_args...);
        tType* property_ptr = u_property.get();
        exported_properties.push_back(std::move(u_property));

        return property_ptr;
    }

    ActorEditorObject();

    virtual void Update(ProgramState* _program_state, ActorEditorObject* _parent_editor_object, Vector2f _parent_position);

    virtual void PropertyUpdate(ProgramState* _program);

    virtual void NullHandles(){
        local_position_handle = nullptr;
    }

    virtual void OnSetupProperty(PropertyInt* _p){

    }

    virtual void OnSetupProperty(PropertyString* _p){
        
    }

    virtual void OnSetupProperty(PropertyAssetString* _p){

    }

    virtual void OnSetupProperty(PropertyFloat* _p){
        
    }

    virtual void OnSetupProperty(PropertyVector2f* _p){
        if(_p->name == "local position"){
            local_position_handle = _p;
        }
    }

    void UpdateExportedAttributes(ProgramState* _program);

    void SetPropertiesFromJson(JsonDictionary* _json);

};

}