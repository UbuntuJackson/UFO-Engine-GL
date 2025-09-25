#ifndef FILE_H
#define FILE_H

#include <string>
#include <sstream>

class File{
public:
    std::string contents;
    std::string path;
    File();
    File(std::string _path);
    static bool Exists(std::string _path);
    std::string Read(std::string _path);
    std::string GetAsString();
    void Write(std::string _filename);
    static File New();
    static File New(std::string _path);
    void Save();
    void Insert(std::string _text);
};

#endif