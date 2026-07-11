#pragma once
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <exception>
#include "../utils/console.h"
#include "../external/cJSON.h"
#include "../utils/file_utils.h"
#include "../utils/file_utils.h"

//Work in progress: Json holder class supporting shared_ptr

namespace ufo::SharedMemory{

    class Json{
    public:

        virtual float AsFloat(){throw std::runtime_error("ufo::gc::Json Invalid conversion, Object is not float");}
        virtual std::string AsString(){throw std::runtime_error("ufo::gc::Json Invalid conversion, Object is not String");}
        virtual std::map<std::string,std::shared_ptr<Json>> AsMap(){throw std::runtime_error("ufo::gc::Json Invalid conversion, Object is not Map");}
        virtual std::vector<std::shared_ptr<Json>> AsArray(){throw std::runtime_error("ufo::gc::Json Invalid conversion, Object is not Array");}
        virtual bool IsNull(){return true;}

        ~Json(){

        }

        virtual cJSON* GetObject(){
            return nullptr;
        }

    };

    class JsonNumber : public Json{
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

    class JsonString : public Json{
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

    class JsonArray : public Json{
    public:
        std::vector<std::shared_ptr<Json>> array;

        std::vector<std::shared_ptr<Json>> AsArray(){return array;}
        bool IsNull(){return false;}

        cJSON* GetObject(){
            cJSON* self = cJSON_CreateArray();

            for(const auto& v : array){
                cJSON_AddItemToArray(self, v->GetObject());
            }
            //Console::PrintLine("Object:",self);
            return self;
        }
    };

    class JsonMap : public Json{
    public:
        std::map<std::string,std::shared_ptr<Json>> map;

        std::map<std::string,std::shared_ptr<Json>> AsMap(){return map;}
        bool IsNull(){return false;}

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
        bool IsNull() override{return true;}
    };

    std::shared_ptr<JsonMap> GetDictionaryAsTree(cJSON* _obj);

    std::shared_ptr<JsonArray> cJSON_ToArray(cJSON* member);

    std::shared_ptr<JsonMap> JsonRead(std::string _path);

}
