#pragma once
#include <string>

namespace UFOEngineStudio{
void OnWriteProjectFile(void *_userdata, const char * const *_filelist, int _filter);
void OnOpenProjectFile(void *_userdata, const char * const *_filelist, int _filter);
void OnOpenFolder(void *_userdata, const char * const *_filelist, int _filter);
void OnNewActorFile(void *_tab, const char * const *_filelist, int _filter);
void OnOpenTileset(void *_userdata, const char * const *_filelist, int _filter);
void OnOpenTexture(void *_userdata, const char * const *_filelist, int _filter);
void OnNewTextFile(void *_tab, const char * const *_filelist, int _filter);
std::string GetFilenameFromPath(const std::string& _path);
bool IsExtension(const std::string& _path, const std::string& _ext);
}
