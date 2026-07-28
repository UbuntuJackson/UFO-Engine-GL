#pragma once
#include <string>
#include <SDL3/SDL_dialog.h>

namespace UFOEngineStudio{

inline static const SDL_DialogFileFilter global_texture_filters[] = {{"PNG images", "png"}, {"JPEG images", "jpeg;jpg"}};

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

}
