#include "garbage_collector.h"
#include <map>
#include <string>
#include <vector>
#include "../external/cJSON.h"
#include "../file/file.h"
#include "../json/json.h"
#include "gc_json.h"

namespace ufo{

namespace gc{

gc::JsonMap* GetDictionaryAsTree(ufo::GarbageCollector* _gc, cJSON* _obj){

    gc::JsonMap* json_map = _gc->New<gc::JsonMap>();
    cJSON* iterator = nullptr;

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

gc::JsonMap* JsonRead(GarbageCollector* _gc,std::string _path){
    std::string s = File(_path).GetAsString();
    cJSON* member = ujson::JsonParse(s);
    bool invalid_cjson = false;
    if(!member){
        invalid_cjson = true;
        Console::PrintLine("[!]", "[Json::JsonRead()]" ,"Could not load json from path:", _path);
    }
    else Console::PrintLine("[!]", "[Json::JsonRead()]","Json loaded successfully", _path);
    gc::JsonMap* j = GetDictionaryAsTree(_gc,member);
    cJSON_Delete(member);
    return j;
}

}

}
