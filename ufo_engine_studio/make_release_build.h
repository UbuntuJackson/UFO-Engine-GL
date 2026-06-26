#pragma once
#include "console.h"
#include <filesystem>
#include <string>
#include <sys/stat.h>
#include "../utils/file_utils.h"
#include "../shared_json/shared_json.h"

namespace UFOEngineStudio{

void MakeReleaseBuild(const std::string& _development_build_directory, const std::string& _release_build_directory){

    int number_of_directory_entries = 0;

    for(const auto& directory_entry : std::filesystem::recursive_directory_iterator(_release_build_directory)){
        number_of_directory_entries++;
    }

    if(number_of_directory_entries != 0) return;

    auto dir_it = std::filesystem::recursive_directory_iterator(_development_build_directory);

    for(const auto& directory_entry : dir_it){

        Console::PrintLine("Found entry", ufo::FileSystem::GetFilenameFromPath(directory_entry.path().c_str()));

        const std::string entry_name = ufo::FileSystem::GetFilenameFromPath(directory_entry.path().c_str());

        if(directory_entry.is_directory()){
            if(entry_name == "UFO-Engine" || entry_name == "UFO-Engine-GL" ||
                entry_name == "__pycache__"
            ){
                dir_it.disable_recursion_pending();
                continue;
            }

            std::string dir = std::string(directory_entry.path().c_str());

            Console::PrintLine("mkdir",
                _release_build_directory+"/"+ufo::FileSystem::GetRelativePath(directory_entry.path().c_str(), _development_build_directory)
            );

            std::filesystem::create_directory(std::string(_release_build_directory+"/"+ufo::FileSystem::GetRelativePath(directory_entry.path().c_str(), _development_build_directory)).c_str());

        }
        //if file
        else{
            std::string release_build_file_path = _release_build_directory+"/"+ufo::FileSystem::GetRelativePath(directory_entry.path().c_str(), _development_build_directory);

            if(ufo::FileSystem::HasExtension(entry_name, "ason")){
                Console::PrintLine("Copying entry", ufo::FileSystem::GetFilenameFromPath(directory_entry.path().c_str()));
                //std::filesystem::copy(directory_entry.path().c_str(),release_build_file_path);
                auto shared_json = ufo::SharedMemory::JsonRead(directory_entry.path().c_str());
                if(!shared_json->IsNull()) shared_json->WriteUnformatted(release_build_file_path);
            }
            if(ufo::FileSystem::HasExtension(entry_name, "json")){
                Console::PrintLine("Copying entry", ufo::FileSystem::GetFilenameFromPath(directory_entry.path().c_str()));
                //std::filesystem::copy(directory_entry.path().c_str(),release_build_file_path);
                auto shared_json = ufo::SharedMemory::JsonRead(directory_entry.path().c_str());
                if(!shared_json->IsNull()) shared_json->WriteUnformatted(release_build_file_path);
            }
            if(ufo::FileSystem::HasExtension(entry_name, "png")){
                Console::PrintLine("Copying entry", ufo::FileSystem::GetFilenameFromPath(directory_entry.path().c_str()));
                std::filesystem::copy(directory_entry.path().c_str(),release_build_file_path);
                //ufo::SharedMemory::JsonRead(directory_entry.path().c_str())->WriteUnformatted(/* ... */);
            }
            if(ufo::FileSystem::HasExtension(entry_name, "ogg")){
                Console::PrintLine("Copying entry", ufo::FileSystem::GetFilenameFromPath(directory_entry.path().c_str()));
                std::filesystem::copy(directory_entry.path().c_str(),release_build_file_path);
                //ufo::SharedMemory::JsonRead(directory_entry.path().c_str())->WriteUnformatted(/* ... */);
            }
            if(ufo::FileSystem::HasExtension(entry_name, "cs")){
                Console::PrintLine("Copying entry", ufo::FileSystem::GetFilenameFromPath(directory_entry.path().c_str()));
                std::filesystem::copy(directory_entry.path().c_str(),release_build_file_path);
                //ufo::SharedMemory::JsonRead(directory_entry.path().c_str())->WriteUnformatted(/* ... */);
            }
            if(ufo::FileSystem::HasExtension(entry_name, "glsl")){
                Console::PrintLine("Copying entry", ufo::FileSystem::GetFilenameFromPath(directory_entry.path().c_str()));
                std::filesystem::copy(directory_entry.path().c_str(),release_build_file_path);
                //ufo::SharedMemory::JsonRead(directory_entry.path().c_str())->WriteUnformatted(/* ... */);
            }
            if(entry_name == "OUT"){
                Console::PrintLine("Copying entry", ufo::FileSystem::GetFilenameFromPath(directory_entry.path().c_str()));
                std::filesystem::copy(directory_entry.path().c_str(),release_build_file_path);
                //ufo::SharedMemory::JsonRead(directory_entry.path().c_str())->WriteUnformatted(/* ... */);
            }


        }
    }

}

}
