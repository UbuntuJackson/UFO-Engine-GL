#pragma once
#include <map>
#include <string>
#include <memory>
#include "../utils/console.h"
#include "../file/file.h"
#include "../external/cJSON.h"
#include <stdexcept>
#include "../external/olcPixelGameEngine.h"

class JsonDictionary;
class JsonArray;

class JsonVariant{
public:
    std::string path;
    bool is_null = false;

    static JsonDictionary Read(std::string _path);

    std::string GetJsonAsString(){
        return cJSON_Print(GetObject());
    }

    enum{
        INT,
        STRING,
        FLOAT,
        DICTIONARY,
        ARRAY
    };

    virtual bool IsNull(){
        return true;
    }

    virtual bool IsArray(){
        return false;
    }
    virtual bool IsInt(){
        return false;
    }
    virtual bool IsString(){
        return false;
    }
    virtual bool IsFloat(){
        return false;
    }
    virtual bool IsDictionary(){
        return false;
    }

    virtual JsonArray& AsArray();
    virtual int AsInt();
    virtual std::string AsString();
    virtual float AsFloat();
    virtual JsonDictionary& AsDictionary();
    virtual olc::vf2d AsVector2f(std::string _x, std::string _y);

    virtual void Write(std::string _path){

    }

    virtual cJSON* GetObject(){return nullptr;}

};

class JsonInt : public JsonVariant{
public:
    int i = 0;

    JsonInt(int _i):i{_i}{}

    int AsInt(){
        return i;
    }

    bool IsNull(){
        return false;
    }
    
    cJSON* GetObject(){
        cJSON* self = cJSON_CreateNumber(i);
        return self;
    }
};

class JsonString : public JsonVariant{
public:
    std::string s = 0;
    JsonString(std::string _s):s{_s}{}

    std::string AsString(){
        return s;
    }

    bool IsString(){
        return true;
    }

    bool IsNull(){
        return false;
    }

    cJSON* GetObject(){
        cJSON* self = cJSON_CreateString(s.c_str());
        return self;
    }
};

class JsonFloat : public JsonVariant{
public:
    float f = 0;
    JsonFloat(float _f) : f{_f}{}

    float AsFloat(){
        return f;
    }

    int AsInt(){
        return (int)f;
    }

    bool IsNull(){
        return false;
    }

    bool IsFloat(){
        return true;
    }

    cJSON* GetObject(){
        cJSON* self = cJSON_CreateNumber(f);
        return self;
    }
};

class JsonArray : public JsonVariant{
public:
    JsonArray() = default;
    JsonArray(JsonArray& _other){
        for(auto&& _other : _other.objects) objects.push_back(std::move(_other));
    }
    JsonArray(JsonArray&& _other){
        for(auto&& _other : _other.objects) objects.push_back(std::move(_other));
    }

    std::vector<std::unique_ptr<JsonVariant>> objects;

    void Push(int _value);
    void Push(float _value);
    void Push(std::string _value);
    void Push(JsonDictionary _value);
    void Push(JsonArray _value);
    void Push(JsonVariant _value);

    static JsonArray cJSON_ToArray(cJSON* member);

    JsonArray& AsArray(){
        return *this;
    }

    std::vector<std::unique_ptr<JsonVariant>>& Iterable(){
        return objects;
    }

    bool IsArray(){
        return true;
    }

    bool IsNull(){
        return false;
    }

    bool IsEmpty(){
        return objects.size() == 0;
    }

    cJSON* GetObject(){
        cJSON* self = cJSON_CreateArray();

        for(const auto& v : objects){
            cJSON_AddItemToArray(self, v->GetObject());
        }
        Console::Out("Object:",self);
        return self;
    }
};

class JsonDictionary : public JsonVariant{
public:
    JsonDictionary() = default;

    JsonDictionary& operator=(JsonDictionary&& _other){
        for(auto&& [k,_other] : _other.dictionary) dictionary[k] = (std::move(_other));
        return *this;
    }

    JsonDictionary(JsonDictionary&& _other){
        for(auto&& [k,_other] : _other.dictionary) dictionary[k] = (std::move(_other));
    }

    JsonDictionary(JsonDictionary& _other){
        for(auto&& [k,_other] : _other.dictionary) dictionary[k] = (std::move(_other));
    }

    std::map<std::string, std::unique_ptr<JsonVariant>> dictionary;
    JsonDictionary& AsDictionary(){
        return *this;
    }

    olc::vf2d AsVector2f(std::string _x, std::string _y){
        float x = Get(_x).AsFloat();
        float y = Get(_y).AsFloat();
        return olc::vf2d(x,y);
    }

    std::map<std::string,std::unique_ptr<JsonVariant>>& Iterable(){
        return dictionary;
    }

    bool IsEmpty(){
        Console::Out("Json dictionary size",dictionary.size());
        return dictionary.size() == 0;
    }

    bool IsDictionary(){
        return true;
    }

    bool IsNull(){
        //Is not null by default but can be set to null if not read from JsonVariant::Read()
        return is_null;
    }

    void Set(std::string _key, int _value){
        dictionary[_key] = std::make_unique<JsonInt>(_value);
    }
    void Set(std::string _key, float _value){
        dictionary[_key] = std::make_unique<JsonFloat>(_value);
    }
    void Set(std::string _key, std::string _value){
        dictionary[_key] = std::make_unique<JsonString>(_value);
    }
    void Set(std::string _key, JsonDictionary _value){
        dictionary[_key] = std::make_unique<JsonDictionary>(_value);
    }
    void Set(std::string _key, JsonArray _value){
        dictionary[_key] = std::make_unique<JsonArray>(_value);
    }
    void Set(std::string _key, JsonVariant _value){
        dictionary[_key] = std::make_unique<JsonVariant>(_value);
    }

    JsonVariant invalid_json_variant;

    JsonVariant& Get(std::string _key){
        if(!dictionary.count(_key)) return invalid_json_variant;
        return *dictionary[_key];
    }

    bool KeyExists(std::string _key){
        return dictionary.count(_key);
    }

    cJSON* GetObject(){
        cJSON* self = cJSON_CreateObject();

        for(const auto& [k, v] : dictionary){
            cJSON_AddItemToObject(self, k.c_str(), v->GetObject());
        }
        Console::Out("Object:",self);
        return self;
    }

    void Write(std::string _path){
        auto file = File::New(_path);
        file.Insert(GetJsonAsString());
        file.Save();
    }

    JsonDictionary GetAsTree(cJSON* member){
        cJSON* iterator = nullptr;

        JsonDictionary j;

        cJSON_ArrayForEach(iterator,member){
            if(cJSON_IsNumber(iterator)){

                if(/*(double)(iterator->valueint) == iterator->valuedouble*/ false){
                    j.Set(iterator->string ,iterator->valueint);
                    //Console::Out("Json::GetAsTree found number");
                }
                else{
                    j.Set(iterator->string ,float(iterator->valuedouble));
                    //Console::Out("Json::GetAsTree found double");
                }

            }

            if(cJSON_IsBool(iterator)){

                
                j.Set(iterator->string ,float(iterator->valueint));
                //Console::Out("Json::GetAsTree found double");
                

            }

            if(cJSON_IsString(iterator)){
                j.Set(iterator->string,iterator->valuestring);
                //Console::Out("Json::GetAsTree found number", iterator->string);
            }
            
            if(cJSON_IsObject(iterator)){
                j.Set(iterator->string,GetAsTree(iterator));
                
                //Console::Out("Json::GetAsTree found object", iterator->string);
            }

            if(cJSON_IsArray(iterator)){
                j.Set(iterator->string,JsonArray::cJSON_ToArray(iterator));
                
                //Console::Out("Json::GetAsTree found object", iterator->string);
            }
        }
        return j;
    }

    JsonDictionary GetDictionaryAsTree(cJSON* _obj){

        JsonDictionary j;
        cJSON* iterator = nullptr;

        cJSON_ArrayForEach(iterator,_obj){
            if(cJSON_IsNumber(iterator)){

                if(/*(double)(iterator->valueint) == iterator->valuedouble*/ false){
                    j.Set(iterator->string ,iterator->valueint);
                    //Console::Out("Json::GetAsTree found number");
                }
                else{
                    j.Set(iterator->string ,float(iterator->valuedouble));
                    //Console::Out("Json::GetAsTree found double");
                }

            }

            if(cJSON_IsBool(iterator)){

                
                j.Set(iterator->string ,float(iterator->valueint));
                //Console::Out("Json::GetAsTree found double");
                

            }
            
            if(cJSON_IsString(iterator)){
                j.Set(iterator->string,iterator->valuestring);
                //Console::Out("Json::GetAsTree found String", iterator->string);
            }

            if(cJSON_IsObject(iterator)){
                j.Set(iterator->string,GetDictionaryAsTree(iterator));
                //Console::Out("Json::GetAsTree found object", iterator->string);
            }

            if(cJSON_IsArray(iterator)){
                j.Set(iterator->string,JsonArray::cJSON_ToArray(iterator));
                
                //Console::Out("Json::GetAsTree found object", iterator->string);
            }
        }
        return j;
    }

};