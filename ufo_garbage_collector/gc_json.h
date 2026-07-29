#pragma once
#include "garbage_collector.h"
#include "object.h"
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <exception>
#include "../external/cJSON.h"
#include "../utils/json.h"
#include "../utils/file_utils.h"

namespace ufo{

namespace gc{

class JsonMap;

class Json : public gc::Object{
public:

    virtual float AsFloat(){throw std::runtime_error("ufo::gc::Json Invalid conversion, Object is not float");}
    virtual std::string AsString(){throw std::runtime_error("ufo::gc::Json Invalid conversion, Object is not String");}
    virtual std::map<std::string,gc::Json*> AsMap(){throw std::runtime_error("ufo::gc::Json Invalid conversion, Object is not Map");}
    virtual gc::JsonMap* AsJsonMap(){throw std::runtime_error("ufo::gc::Json Invalid conversion, Object is not Map");}
    virtual std::vector<gc::Json*> AsArray(){throw std::runtime_error("ufo::gc::Json Invalid conversion, Object is not Array");}
    virtual bool IsNull(){return true;}

    ~Json(){

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
    gc::JsonMap* AsJsonMap(){return this;}
    bool IsNull(){return false;}

    void OnMark(){
        alive = true;
        for(const auto& [k,v] : map){
            v->Mark();

        }
    }

    void TryToGetValueAsString(std::string _key, std::string& _string, const std::string& _location){
        try{
            _string = map.at(_key)->AsString();
        } catch(const std::exception& _error){
            Console::PrintLine(_location,"[UFO-Engine] JsonMap: Could not get value for key",_key);
        }
    }
    void TryToGetValueAsFloat(std::string _key, float& _i, const std::string& _location){
        try{
            _i = map.at(_key)->AsFloat();
        } catch(const std::exception& _error){
            Console::PrintLine(_location,"[UFO-Engine] JsonMap: Could not get value for key",_key);
        }
    }
    void TryToGetValueAsInt(std::string _key, int& _i, const std::string& _location){
        try{
            _i = (int)map.at(_key)->AsFloat();
        } catch(const std::exception& _error){
            Console::PrintLine(_location,"[UFO-Engine] JsonMap: Could not get value for key",_key);
        }
    }
    void TryToGetValueAsBool(std::string _key, bool& _i, const std::string& _location){
        try{
            _i = (bool)map.at(_key)->AsFloat();
        } catch(const std::exception& _error){
            Console::PrintLine(_location,"[UFO-Engine] JsonMap: Could not get value for key",_key);
        }
    }
    void TryToGetValueAsArray(std::string _key, std::vector<gc::Json*>& _array, const std::string& _location){
        try{
            _array = map.at(_key)->AsArray();
        } catch(const std::exception& _error){
            Console::PrintLine(_location,"[UFO-Engine] JsonMap: Could not get value for key",_key);
        }
    }
    void TryToGetValueAsMap(std::string _key, std::map<std::string,gc::Json*>& _map, const std::string& _location){
        try{
            _map = map.at(_key)->AsMap();
        } catch(const std::exception& _error){
            Console::PrintLine(_location,"[UFO-Engine] JsonMap: Could not get value for key",_key);
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

        std::string string_to_return = json_as_string;

        free(json_as_string);

        cJSON_Delete(json_obj);

        return std::string(string_to_return);
    }

    std::string GetJsonAsUnformattedString(){
        cJSON* json_obj = GetObject();

        char* json_as_string = cJSON_PrintUnformatted(json_obj);

        std::string string_to_return = json_as_string;

        free(json_as_string);

        cJSON_Delete(json_obj);

        return std::string(string_to_return);
    }

    void WriteUnformatted(const std::string& _path){
        std::string json_as_string = GetJsonAsUnformattedString();
        try{
            FileSystem::Write(_path, json_as_string);
        }
        catch(const std::exception& _error){
            Console::PrintLine("ufo::gc::JsonMap: Something went wrong writing json to path", _path, _error.what());
        }
    }

    void Write(const std::string& _path){
        std::string json_as_string = GetJsonAsString();
        try{
            FileSystem::Write(_path, json_as_string);
        }
        catch(const std::exception& _error){
            Console::PrintLine("ufo::gc::JsonMap: Something went wrong writing json to path", _path, _error.what());
        }
    }

};

class FaultyJsonMap : public JsonMap{
public:
     bool IsNull() override {return true;}
};

gc::JsonMap* GetDictionaryAsTree(ufo::GarbageCollector* _gc, cJSON* _obj);

gc::JsonArray* cJSON_ToArray(ufo::GarbageCollector* _gc, cJSON* member);

gc::JsonMap* JsonRead(GarbageCollector* _gc,std::string _path);

}

}
