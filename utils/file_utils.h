#pragma once
#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace ufo::FileSystem{

std::string GetFilenameFromPath(const std::string& _path);

bool FileExists(const std::string& _path);

bool HasExtension(const std::string& _path, const std::string& _extension);

void Write(const std::string& _path, const std::string& _contents);

std::string Read(const std::string& _path);

void Rename(const std::string& _path, const std::string& _new_path);

std::string GetRelativePath(const std::string& _full_path, const std::string& _current_working_directory);

}
