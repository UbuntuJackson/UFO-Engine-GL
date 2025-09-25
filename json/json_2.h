#ifndef JSON_2_H
#define JSON_2_H

#include <string>
#include "../external/cJSON.h"
#include "../console/console.h"

class Json2{
public:

    int reference_count = 0;

    std::string path_and_name;
    bool is_top_node = false;

    cJSON* member;

    cJSON* Get(){
        return member;
    }

    static Json2 Read(std::string _path_and_name){
        Json2 j; 
        
        j.path_and_name = _path_and_name;
        j.member = cJSON_Parse(_path_and_name.c_str());

        return j;
    }

    ~Json2(){
        if(is_top_node) cJSON_Delete(member);
        Console::Out("Json2 with filename", path_and_name, "was garbage collected");
    }

};

#endif