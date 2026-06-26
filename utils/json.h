#ifndef JSON_INTERFACE_H
#define JSON_INTERFACE_H
#include "../external/cJSON.h"
#include <string>
#include <vector>
#include <functional>
#include "../utils/console.h"
#include "../utils/file_utils.h"
#include <variant>
#include <map>

namespace ujson{
    cJSON* JsonParse(std::string _path);
    cJSON* GetObject(cJSON* _obj, std::string _name);
    int ArrayLen(cJSON* _obj);
    cJSON* GetElement(cJSON* _obj, int _index);
    cJSON* CreateObject();
    cJSON* CreateString(std::string _s);
    cJSON* CreateNumber(double _num);
    cJSON* CreateNumber(int _num);
    cJSON* CreateArray();
    void AddItemToArray(cJSON* _obj_a, cJSON* _obj_b);
    void AddItemToObject(cJSON* _obj_a, std::string _name, cJSON* _obj_b);

}

#endif
