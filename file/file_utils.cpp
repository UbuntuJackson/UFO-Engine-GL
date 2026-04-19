#include "file_utils.h"
#include <stdexcept>

namespace ufo::FileSystem{

std::string GetFilenameFromPath(const std::string& _path){
    size_t last_slash_index = _path.find_last_of("/")+1;

    std::string res = _path.substr(last_slash_index);

    if (last_slash_index == _path.npos){
        return _path;
    }

    return res;
}

bool FileExists(const std::string& _path){
    return std::filesystem::exists(_path);
}

bool HasExtension(const std::string& _path, const std::string& _extension){

    return _path.substr(_path.find_last_of(".")) == "."+_extension;

}

void Write(const std::string& _path, const std::string& _contents){
    std::ofstream outfile(_path);
    if(!outfile.good()){
        throw std::runtime_error("ufo::FileSystem::Write: Could not write to path "+_path);
    }
    outfile << _contents << std::endl;
    outfile.close();
}

std::string Read(const std::string& _path){
    std::ifstream ifs{_path};
    if(!ifs.good()){
        throw std::runtime_error("ufo::FileSystem::Read: Path "+_path+" does not exist");
    }
    std::stringstream contents_stream;
    contents_stream << ifs.rdbuf();
    std::string contents = contents_stream.str();
    ifs.close();
    return contents;
}

void Rename(const std::string& _path, const std::string& _new_path){

    std::filesystem::rename(_path, _new_path);

}

std::string GetRelativePath(const std::string& _full_path, const std::string& _current_working_directory){

    size_t cut_index = _full_path.find(_current_working_directory)+1;

    if(cut_index == std::string::npos) throw std::runtime_error("ufo::FileSystem::GetRelativePath: Found no matching subpath");

    return _full_path.substr(cut_index+_current_working_directory.size(),_full_path.size()-1);
}

}
