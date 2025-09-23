#include <fstream>
#include <sstream>
#include <string>
#include "console.h"
#include "file.h"

File::File() : contents{""}{}

File::File(std::string _path){
    Read(_path);
    path = _path;
}

File
File::New(){
    return File();
}

File
File::New(std::string _path){
    File file;
    file.Write(_path);
    file.path = _path;
    return file;
}

void
File::Save(){
    if(path == "") Console::Out("Error: faulty path.");
    else Write(path);
}

bool File::Exists(std::string _path){
    std::ifstream ifs(_path.c_str());
    return ifs.good();
}

std::string File::Read(std::string _path){
    path = _path;
    std::ifstream ifs{_path};
    std::stringstream contents_stream;
    contents_stream << ifs.rdbuf();
    contents = contents_stream.str();
    ifs.close();
    return contents;
}

std::string File::GetAsString(){return contents;}

void File::Write(std::string _filename){
    std::ofstream outfile(_filename);
    outfile << contents << std::endl;
    outfile.close();
}

void File::Insert(std::string _text){
    contents += _text;
}   