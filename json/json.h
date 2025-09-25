#ifndef JSON_INTERFACE_H
#define JSON_INTERFACE_H
#include "../external/cJSON.h"
#include <string>
#include <vector>
#include <functional>
#include "../utils/console.h"
#include "../file/file.h"
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

/*class Json;

namespace json_get{
    template<typename tType>
    tType Get(cJSON* _json, std::string _name);

    template<>
    int Get<int>(cJSON* _json, std::string _name);
    template<>
    double Get<double>(cJSON* _json, std::string _name);
    template<>
    std::string Get<std::string>(cJSON* _json, std::string _name);
    template<>
    Json Get<Json>(cJSON* _json, std::string _name);
}*/

class Json{
public:
    cJSON *member = nullptr;
    std::string path;
    bool is_top_node = false;
    bool from_read_creation_method = false;

    //new functions

    /*template<typename tType>
    tType Get(std::string _name){
        return json_get::Get<tType>(this, _name);
    }*/

    Json() = default;

    Json(const Json&) = default;

    Json(Json&& _other){
    }

    //This is apparently invoked when something is passed by copy?
    //And it's also invoked whenever the assignment operator is used I think?
    Json& operator=(Json&& _other){
        member = _other.member;
        _other.is_top_node = false;
        if(_other.from_read_creation_method) is_top_node = true;
        path = _other.path;
        return *this;
    }

    void ReplaceObject(std::string _object_name, Json _json){
        cJSON_ReplaceItemInObject(member, _object_name.c_str(), _json.member);
    }

    void Set(std::string _object_name, Json _json){
        if(Json::GetObject(_object_name).IsNull()){
            PushToObject(_object_name, _json);
        }
        else{
            ReplaceObject(_object_name, _json);
        }
    }

    void RemoveObject(std::string _object_name){
        Console::Out("[!] [Json::RemoveObject] Removing and calling delete on removed object");
        cJSON_Delete(cJSON_DetachItemFromObjectCaseSensitive(member, _object_name.c_str()));
    }

    Json(std::string _path){
        std::string s = File(_path).GetAsString();
        member = ujson::JsonParse(s);
        if(!member) Console::Out("[!]", "[Json::JsonRead()]" ,"Could not load json from path:", _path);
        else Console::Out("[!]", "[Json::JsonRead()]","Json loaded successfully", _path);
        is_top_node = true;
        from_read_creation_method = true;
        path = _path;
    }

    static Json Read(std::string _path);

    /*std::map<std::string, std::variant<int,std::string,float>> GetAsTree(std::map<std::string, std::variant<int,std::string,float>>& _j){
        cJSON* iterator = nullptr;

        std::map<std::string, std::variant<int,std::string,float>> j;

        cJSON_ArrayForEach(iterator,member){
            if(cJSON_IsNumber(iterator)){
                j[iterator->string] = iterator->valueint;
                Console::Out("Json::GetAsTree found number");
            }
        }
        return j;
    }*/

    static Json New(std::string _path);

    void Save(){
        Write(path);
    }
    
    void Write(std::string _path){
        auto file = File::New(_path);
        file.Insert(GetJsonAsString());
        file.Save();
    }

    static Json New(){
        Json json;
        json.member = ujson::CreateObject();
        json.is_top_node = true;
        json.from_read_creation_method = true;
        return json;
    }

    bool IsEmpty(){
        return member->child == NULL;
    }

    Json GetObject(std::string _name){
        Json json;
        json.member = ujson::GetObject(member, _name);
        if(!json.member) Console::Out("[!] Could not successfully get json-object named:", _name);
        return json;
    }

    std::string GetKey(){
        return member->string;
    }

    std::string GetKey(std::string _name){
        return Json::GetObject(_name).member->string;
    }

    int GetAsInt(std::string _name){ //should be renamed to GetObjectAs[Type](...)?
        return Json::GetObject(_name).GetAsInt();
    }

    double GetAsDouble(std::string _name){
        return Json::GetObject(_name).GetAsDouble();
    }

    std::string GetAsString(std::string _name){
        return Json::GetObject(_name).GetAsString();
    }

    std::vector<Json> GetAsArray(std::string _name){
        return Json::GetObject(_name).GetAsArray();
    }

    int GetAsInt(){
        if(!member) Console::Out("[!] member not valid");
        return member->valueint;
    }

    double GetAsDouble(){
        if(!member) Console::Out("[!] member not valid");
        return member->valuedouble;
    }

    std::string GetAsString(){
        if(!member) Console::Out("[!] member not valid");
        return member->valuestring;
    }

    std::vector<Json> GetAsArray(){ //apparently has lookup time that grows with each incrementation of i. Don't know entirely why.
        std::vector<Json> v;
        for(int i = 0; i < ujson::ArrayLen(member); i++){
            Json json;
            json.member = ujson::GetElement(member, i);
            if(!json.member) Console::Out("[!] Could not successfully get json-array-member");
            v.push_back(json);
        }
        return v;
    }

    Json GetElement(int _i){
        Json json;
        json.member = ujson::GetElement(member, _i);
        if(!json.member) Console::Out("[!] [Json::GetElement()] member is presumably not a json-array.");
        return json;
    }

    int GetArrayLength(){
        return ujson::ArrayLen(member);
    }

    static void ArrayForEach(Json&& _iterator_json, const Json& _array_json, std::function<void(Json&, const Json&)> _lambda){ //should pass by reference. For ducks sake
        for(_iterator_json.member = (_array_json.member != NULL) ? (_array_json.member)->child : NULL; _iterator_json.member != NULL; _iterator_json.member = _iterator_json.member->next){
            _lambda(_iterator_json, _array_json);
        }
    }

    static Json NewObject(int _number){
        Json json;
        json.member = ujson::CreateNumber(_number);
        return json;
    }

    static Json NewObject(double _number){
        Json json;
        json.member = ujson::CreateNumber(_number);
        return json;
    }

    static Json NewObject(std::string _s){
        Json json;
        json.member = ujson::CreateString(_s);
        return json;
    }

    static Json NewObject(){
        Json json;
        json.member = ujson::CreateObject();
        return json;
    }

    template<typename ...tArgs>
    static Json NewArray(tArgs ...args){
        Json json;
        json.member = ujson::CreateArray();
        
        (ujson::AddItemToArray(json.member, NewObject(args).member), ...);
        
        return json;
    }

    static Json NewArray(){
        Json json;
        json.member = ujson::CreateArray();
        return json;
    }
    void PushToObject(std::string _name, Json _json){
        ujson::AddItemToObject(member, _name, _json.member);
    }

    template<typename tType>
    void PushToObject(std::string _name, tType _value){

        ujson::AddItemToObject(member, _name, NewObject(_value).member);
    }

    void PushToArray(Json _json){
        ujson::AddItemToArray(member, _json.member); //this object may or may not even be an array
    }

    template<typename tType>
    void PushToArray(tType _value){
        ujson::AddItemToArray(member, NewObject(_value).member); //this object may or may not even be an array
    }

    std::string GetJsonAsString(){
        return cJSON_Print(member);
    }

    bool IsNull(){
        if(!member) return true;
        return false;
    }

    ~Json(){
        if(is_top_node){
            Console::Out("[!]", "[Json::~Json()   ]","Trying to call cJSON_Delete on json with path:", path);
            cJSON_Delete(member);
        }
    }
    
};

/*namespace json_get{
    template<typename tType>
    tType Get(cJSON* _json, std::string _name){
        
    }

    template<>
    int Get<int>(cJSON* _json, std::string _name){
        return ujson::GetObject(_json, _name)->valueint;
    }
    template<>
    double Get<double>(cJSON* _json, std::string _name){
        return ujson::GetObject(_json, _name)->valuedouble;
    }
    template<>
    std::string Get<std::string>(cJSON* _json, std::string _name){
        return ujson::GetObject(_json, _name)->valuestring;
    }
    template<>
    Json Get<Json>(cJSON* _json, std::string _name){
        Json json;
        json.member = ujson::GetObject(_json, _name);
        return json;
    }
}*/

#endif