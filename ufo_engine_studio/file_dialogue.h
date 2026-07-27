#pragma once
#include <string>

namespace UFOEngineStudio{

void OnSelectStartLevel(void *_userdata, const char * const *_filelist, int _filter);
void OnWriteProjectFile(void *_userdata, const char * const *_filelist, int _filter);
void OnOpenProjectFile(void *_userdata, const char * const *_filelist, int _filter);
void OnOpenFolder(void *_userdata, const char * const *_filelist, int _filter);
void OnOpenAutoTileFile(void *_userdata, const char * const *_filelist, int _filter);
void OnNewActorFile(void *_tab, const char * const *_filelist, int _filter);
void OnOpenTileset(void *_userdata, const char * const *_filelist, int _filter);
void OnRecoverTileset(void *_userdata, const char * const *_filelist, int _filter);
void OnOpenTexture(void *_userdata, const char * const *_filelist, int _filter);
void OnOpenShader(void *_userdata, const char * const *_filelist, int _filter);
void OnNewTextFile(void *_tab, const char * const *_filelist, int _filter);
void OnSelectDirectoryForDebugBuild(void *_editor, const char * const *_filelist, int _filter);
bool IsExtension(const std::string& _path, const std::string& _ext);

}
