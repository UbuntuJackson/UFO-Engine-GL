#include "json_variant.h"
#include "../console/console.h"

void JsonVariantTest(){
    auto j = JsonVariant::Read("../UFO-Engine/res/save/test_file.json");
    int i = j.Get("value").AsInt();
    Console::Out(i);
    Console::Out("dict size:",j.dictionary.size());

    j.Set("hello", "world");

    JsonDictionary dict;
    dict.Set("e", 1);
    dict.Set("f", 2);
    dict.Set("g", 3);
    dict.Set("e", 6.1f);

    JsonArray an_array;
    an_array.Push(3);
    an_array.Push("Man...");
    an_array.Push(false);
    an_array.Push(JsonDictionary());

    dict.Set("an_array", an_array);

    j.Set("a", dict);

    Console::Out("dict size:",j.dictionary.size());

    j.Write("../res/save/test_file.json");
    
    auto read_dictionary = JsonVariant::Read("../UFO-Engine/res/save/file_to_read_from.json");

    auto& arr_a = read_dictionary.Get("array").AsArray();

    Console::Out("Is array empty?", arr_a.IsEmpty());

    for(auto&& v : arr_a.Iterable()){
        if(!v->IsNull()) Console::Out("Integer:",v->AsInt());
        else Console::Out("Null");
    }

    Console::Out("Is array2 empty?",read_dictionary.Get("array2").AsArray().IsEmpty());
    Console::Out("Is obj empty?",read_dictionary.Get("obj").AsDictionary().IsEmpty());
    Console::Out("Is obj2 empty?",read_dictionary.Get("obj2").AsDictionary().IsEmpty(), read_dictionary.Get("obj2").AsDictionary().Get("str").AsString());

    Console::Out(read_dictionary.Get("position").AsVector2f("x","y"));

    //Console::Out("JsonVariantTest:",read_dictionary.GetJsonAsString());
}

void JsonVariantTest2(){
    JsonDictionary j = JsonVariant::Read("../UFO-Engine/res/save/json_test_file.json");

    JsonArray arr = j.Get("stuff").AsArray();

    for(auto&& o : arr.Iterable()){
        Console::Out(o->AsDictionary().Get("x").AsString());
    }

    JsonDictionary to_be_saved = JsonDictionary();
    to_be_saved.Set("one",1);
    to_be_saved.Set("two",2.0f);
    to_be_saved.Set("three","3");
    to_be_saved.Iterable().erase("one");
    to_be_saved.Write("../UFO-Engine/res/save/to_be_saved.json");
}