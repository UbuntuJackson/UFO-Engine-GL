#include <map>
#include <string>
#include <vector>
#include <memory>
#include "../external/cJSON.h"
#include "../utils/file_utils.h"
#include "../utils/json.h"
#include "shared_json.h"

namespace ufo::SharedMemory{

std::shared_ptr<JsonMap> GetDictionaryAsTree(cJSON* _obj){

    std::shared_ptr<JsonMap> json_map = std::make_shared<JsonMap>();
    cJSON* iterator = nullptr;

    cJSON_ArrayForEach(iterator,_obj){
        if(cJSON_IsNumber(iterator)){


            json_map->map.emplace(iterator->string ,std::make_shared<JsonNumber>(iterator->valuedouble));


        }

        if(cJSON_IsBool(iterator)){


            json_map->map.emplace(iterator->string ,std::make_shared<JsonNumber>(iterator->valueint));
            //Console::Out("Json::GetAsTree found double");


        }

        if(cJSON_IsString(iterator)){
            json_map->map.emplace(iterator->string,std::make_shared<JsonString>(iterator->valuestring));
            //Console::Out("Json::GetAsTree found String", iterator->string);
        }

        if(cJSON_IsObject(iterator)){
            json_map->map.emplace(iterator->string,GetDictionaryAsTree(iterator));
            //Console::Out("Json::GetAsTree found object", iterator->string);
        }

        if(cJSON_IsArray(iterator)){
            json_map->map.emplace(iterator->string,cJSON_ToArray(iterator));

            //Console::Out("Json::GetAsTree found object", iterator->string);
        }
    }
    return json_map;
}

std::shared_ptr<JsonArray> cJSON_ToArray(cJSON* member){
    std::shared_ptr<JsonArray> arr = std::make_shared<JsonArray>();

    cJSON* iterator = nullptr;

    cJSON_ArrayForEach(iterator,member){
        if(cJSON_IsNumber(iterator)){


            arr->array.push_back(std::make_shared<JsonNumber>(iterator->valuedouble));


        }

        if(cJSON_IsBool(iterator)){

            arr->array.push_back(std::make_shared<JsonNumber>(iterator->valueint));
            //Console::Out("Json::GetAsTree found double");

        }

        if(cJSON_IsString(iterator)){
            arr->array.push_back(std::make_shared<JsonString>(iterator->valuestring));
            //Console::Out("JsonArray::cJSON_ToArray found number");
        }

        if(cJSON_IsArray(iterator)){
            arr->array.push_back(cJSON_ToArray(iterator));

            //Console::Out("Json::GetAsTree found object", iterator->string);
        }

        if(cJSON_IsObject(iterator)){
            arr->array.push_back(GetDictionaryAsTree(iterator));

            //Console::Out("JsonArray::cJSON_ToArray found object");
        }

        if(cJSON_IsNull(iterator)){
            //j.Push(JsonVariant());
        }
    }

    return arr;
}

std::shared_ptr<JsonMap> JsonRead(std::string _path){
    std::string s = FileSystem::Read(_path);
    cJSON* member = ujson::JsonParse(s);
    bool invalid_cjson = false;
    if(!member){
        invalid_cjson = true;
        Console::PrintLine("[!]", "[Json::JsonRead()]" ,"Could not load json from path:", _path);
        return std::make_shared<FaultyJsonMap>();
    }
    else Console::PrintLine("[!]", "[Json::JsonRead()]","Json loaded successfully", _path);
    std::shared_ptr<JsonMap> j = GetDictionaryAsTree(member);
    cJSON_Delete(member);
    return j;
}

}
