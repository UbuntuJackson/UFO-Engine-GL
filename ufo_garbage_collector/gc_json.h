#pragma once
#include "engine_memory.h"
#include <map>
#include <string>
#include <vector>
#include "../external/cJSON.h"
#include "../file/file.h"
#include "../json/json.h"

namespace ufo{

namespace gc{

class Json : public gc::Object{
public:

    virtual float AsFloat(){throw;}
    virtual std::string AsString(){throw;}
    virtual std::map<std::string,gc::Json*> AsMap(){throw;}
    virtual std::vector<gc::Json*> AsArray(){throw;}
    virtual bool IsNull(){return true;}

    ~Json(){
        Console::PrintLine("Json Object destroyed", this);
    }

    virtual cJSON* GetObject(){
        return nullptr;
    }

};

class JsonNumber : public gc::Json{
public:
    float value = 0;
    JsonNumber(float _value) : value{_value}{}

    float AsFloat(){return value;}
    bool IsNull(){return false;}

    cJSON* GetObject(){
        cJSON* self = cJSON_CreateNumber(value);
        return self;
    }

};

class JsonString : public gc::Json{
public:
    std::string value = "";
    JsonString(std::string _value) : value{_value}{}

    std::string AsString(){return value;}
    bool IsNull(){return false;}

    cJSON* GetObject(){
        cJSON* self = cJSON_CreateString(value.c_str());
        return self;
    }

};

class JsonArray : public gc::Json{
public:
    std::vector<gc::Json*> array;

    std::vector<gc::Json*> AsArray(){return array;}
    bool IsNull(){return false;}

    void OnMark(){
        alive = true;
        for(const auto& v : array){
            v->Mark();
        }
    }

    cJSON* GetObject(){
        cJSON* self = cJSON_CreateArray();

        for(const auto& v : array){
            cJSON_AddItemToArray(self, v->GetObject());
        }
        //Console::PrintLine("Object:",self);
        return self;
    }
};

class JsonMap : public gc::Json{
public:
    std::map<std::string,gc::Json*> map;

    std::map<std::string,gc::Json*> AsMap(){return map;}
    bool IsNull(){return false;}

    void OnMark(){
        alive = true;
        for(const auto& [k,v] : map){
            v->Mark();
            
        }
    }

    cJSON* GetObject(){
        cJSON* self = cJSON_CreateObject();

        for(const auto& [k, v] : map){
            cJSON_AddItemToObject(self, k.c_str(), v->GetObject());
        }
        //Console::PrintLine("Object:",self);
        return self;
    }

    std::string GetJsonAsString(){
        cJSON* json_obj = GetObject();

        char* json_as_string = cJSON_Print(json_obj);

        delete json_as_string;

        cJSON_Delete(json_obj);
        
        return std::string(json_as_string);
    }

    void Write(const std::string& _path){
        auto file = File::New(_path);
        file.Insert(GetJsonAsString());
        file.Save();
    }

};

gc::JsonMap* GetDictionaryAsTree(ufo::GarbageCollector* _gc, cJSON* _obj);

gc::JsonArray* cJSON_ToArray(ufo::GarbageCollector* _gc, cJSON* member);

gc::JsonMap* JsonRead(GarbageCollector* _gc,std::string _path);

}

}