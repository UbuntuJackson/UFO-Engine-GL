#include <map>
#include <string>
#include <memory>
#include "../external/cJSON.h"
#include "../utils/console.h"
#include "../file/file.h"
#include "json_variant.h"
#include "../json/json.h"
#include "../external/olcPixelGameEngine.h"

void JsonArray::Push(int _value){
    objects.push_back(std::make_unique<JsonInt>(_value));
}

void JsonArray::Push(float _value){
    objects.push_back(std::make_unique<JsonFloat>(_value));
}

void JsonArray::Push(std::string _value){
    objects.push_back(std::make_unique<JsonString>(_value));
}

void JsonArray::Push(JsonDictionary _value){
    objects.push_back(std::make_unique<JsonDictionary>(_value));
}

void JsonArray::Push(JsonArray _value){
    objects.push_back(std::make_unique<JsonArray>(_value));
}

void JsonArray::Push(JsonVariant _value){
    objects.push_back(std::make_unique<JsonVariant>(_value));
}

JsonDictionary JsonVariant::Read(std::string _path){
    std::string s = File(_path).GetAsString();
    cJSON* member = ujson::JsonParse(s);
    bool invalid_cjson = false;
    if(!member){
        invalid_cjson = true;
        Console::Out("[!]", "[Json::JsonRead()]" ,"Could not load json from path:", _path);
    }
    else Console::Out("[!]", "[Json::JsonRead()]","Json loaded successfully", _path);
    JsonDictionary j = JsonDictionary().GetAsTree(member);
    j.is_null = invalid_cjson;
    cJSON_Delete(member);
    return j;
}

olc::vf2d JsonVariant::AsVector2f(std::string _x, std::string _y){
    throw std::runtime_error("Not a Vector2f");
}

JsonArray JsonArray::cJSON_ToArray(cJSON* member){
    JsonArray j;
    cJSON* iterator = nullptr;

    cJSON_ArrayForEach(iterator,member){
        if(cJSON_IsNumber(iterator)){

            if(/*(double)(iterator->valueint) == iterator->valuedouble*/ false){
                j.Push(iterator->valueint);
                //Console::Out("Json::GetAsTree found number");
            }
            else{
                j.Push(float(iterator->valuedouble));
                //Console::Out("Json::GetAsTree found double");
            }

        }

        if(cJSON_IsBool(iterator)){

            j.Push(float(iterator->valueint));
            //Console::Out("Json::GetAsTree found double");

        }

        if(cJSON_IsString(iterator)){
            j.Push(iterator->valuestring);
            //Console::Out("JsonArray::cJSON_ToArray found number");
        }
        
        if(cJSON_IsArray(iterator)){
            j.Push(JsonArray::cJSON_ToArray(iterator));
            
            //Console::Out("Json::GetAsTree found object", iterator->string);
        }

        if(cJSON_IsObject(iterator)){
            j.Push(JsonDictionary().GetDictionaryAsTree(iterator));
            
            //Console::Out("JsonArray::cJSON_ToArray found object");
        }

        if(cJSON_IsNull(iterator)){
            j.Push(JsonVariant());
        }
    }

    return j;
}

JsonArray& JsonVariant::AsArray(){
    throw std::runtime_error("Not an array");
}

int JsonVariant::AsInt(){
    throw std::runtime_error("Not an integer");
}

std::string JsonVariant::AsString(){
    throw std::runtime_error("Not a string");
}

float JsonVariant::AsFloat(){
    throw std::runtime_error("Not a float");
}

JsonDictionary& JsonVariant::AsDictionary(){
    throw std::runtime_error("Not a dictionary");
}