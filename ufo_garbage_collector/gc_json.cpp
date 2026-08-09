#include "garbage_collector.h"
#include <map>
#include <string>
#include <vector>
#include "../external/cJSON.h"
#include "../utils/file_utils.h"
#include "../utils/json.h"
#include "gc_json.h"

namespace ufo::gc{

gc::Json* GetJson(ufo::GarbageCollector* _gc, cJSON* _obj){

    gc::Json* json_obj = nullptr;

    if(cJSON_IsNumber(_obj)){
        json_obj = _gc->New<JsonNumber>(_obj->valuedouble);
    }

    if(cJSON_IsBool(_obj)){
        json_obj = _gc->New<JsonNumber>(_obj->valueint);
    }

    if(cJSON_IsString(_obj)){
        json_obj = _gc->New<JsonString>(_obj->valuestring);
    }

    if(cJSON_IsObject(_obj)){
        json_obj = GetDictionaryAsTree(_gc,_obj);
    }

    if(cJSON_IsArray(_obj)){
        json_obj = cJSON_ToArray(_gc,_obj);
    }

    return json_obj;

}

gc::JsonMap* GetDictionaryAsTree(ufo::GarbageCollector* _gc, cJSON* _obj){

    gc::JsonMap* json_map = _gc->New<gc::JsonMap>();
    cJSON* iterator = nullptr;

    if(!cJSON_IsObject(_obj)){
        return _gc->New<gc::FaultyJsonMap>();
    }

    cJSON_ArrayForEach(iterator,_obj){
        if(cJSON_IsNumber(iterator)){


            json_map->map.emplace(iterator->string ,_gc->New<JsonNumber>(iterator->valuedouble));


        }

        if(cJSON_IsBool(iterator)){


            json_map->map.emplace(iterator->string ,_gc->New<JsonNumber>(iterator->valueint));
            //Console::Out("Json::GetAsTree found double");


        }

        if(cJSON_IsString(iterator)){
            json_map->map.emplace(iterator->string,_gc->New<JsonString>(iterator->valuestring));
            //Console::Out("Json::GetAsTree found String", iterator->string);
        }

        if(cJSON_IsObject(iterator)){
            json_map->map.emplace(iterator->string,GetDictionaryAsTree(_gc,iterator));
            //Console::Out("Json::GetAsTree found object", iterator->string);
        }

        if(cJSON_IsArray(iterator)){
            json_map->map.emplace(iterator->string,cJSON_ToArray(_gc,iterator));

            //Console::Out("Json::GetAsTree found object", iterator->string);
        }
    }
    return json_map;
}

gc::JsonArray* cJSON_ToArray(ufo::GarbageCollector* _gc, cJSON* member){
    gc::JsonArray* arr = _gc->New<gc::JsonArray>();

    cJSON* iterator = nullptr;

    cJSON_ArrayForEach(iterator,member){
        if(cJSON_IsNumber(iterator)){


            arr->array.push_back(_gc->New<gc::JsonNumber>(iterator->valuedouble));


        }

        if(cJSON_IsBool(iterator)){

            arr->array.push_back(_gc->New<gc::JsonNumber>(iterator->valueint));
            //Console::Out("Json::GetAsTree found double");

        }

        if(cJSON_IsString(iterator)){
            arr->array.push_back(_gc->New<gc::JsonString>(iterator->valuestring));
            //Console::Out("JsonArray::cJSON_ToArray found number");
        }

        if(cJSON_IsArray(iterator)){
            arr->array.push_back(gc::cJSON_ToArray(_gc,iterator));

            //Console::Out("Json::GetAsTree found object", iterator->string);
        }

        if(cJSON_IsObject(iterator)){
            arr->array.push_back(gc::GetDictionaryAsTree(_gc,iterator));

            //Console::Out("JsonArray::cJSON_ToArray found object");
        }

        if(cJSON_IsNull(iterator)){
            //j.Push(JsonVariant());
        }
    }

    return arr;
}

gc::JsonMap* JsonReadMap(GarbageCollector* _gc,std::string _path){
    std::string s = FileSystem::Read(_path);
    cJSON* member = ujson::JsonParse(s);

    if(!member){

        Console::PrintLine("[!]", "[Json::JsonRead()]" ,"Could not load json from path:", _path);
        return _gc->New<gc::FaultyJsonMap>();
    }
    else Console::PrintLine("[!]", "[Json::JsonRead()]","Json loaded successfully", _path);
    gc::JsonMap* j = GetDictionaryAsTree(_gc,member);
    cJSON_Delete(member);
    return j;
}

gc::Json* JsonRead(GarbageCollector* _gc,std::string _path){
    std::string s = FileSystem::Read(_path);
    cJSON* member = ujson::JsonParse(s);

    if(!member){

        Console::PrintLine("[!]", "[Json::JsonRead()]" ,"Could not load json from path:", _path);
        return _gc->New<gc::FaultyJson>();
    }
    else Console::PrintLine("[!]", "[Json::JsonRead()]","Json loaded successfully", _path);
    gc::Json* j = ufo::gc::GetJson(_gc,member);
    cJSON_Delete(member);
    return j;
}

void TryAsString(std::string& _variable, Json*& _json, const std::string& _location){
    if(_json->IsString()) _variable = _json->AsString();
    else Console::PrintLine(_location,"Could not get value for variable",&_variable);
}
void TryAsFloat(float& _variable, Json*& _json, const std::string& _location){
    if(_json->IsFloat()) _variable = _json->AsFloat();
    else Console::PrintLine(_location,"Could not get value for variable",&_variable);
}
void TryAsArray(std::vector<Json*>& _variable, Json*& _json, const std::string& _location){
    if(_json->IsArray()) _variable = _json->AsArray();
    else Console::PrintLine(_location,"Could not get value for variable",&_variable);
}
void TryAsMap(std::map<std::string,Json*>& _variable, Json*& _json, const std::string& _location){
    if(_json->IsMap()) _variable = _json->AsMap();
    else Console::PrintLine(_location,"Could not get value for variable",&_variable);
}

}
