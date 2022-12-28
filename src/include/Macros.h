#pragma once
#define EXPO export
#ifdef AUDIO
//namespace miniaudio

#undef var
#define STB_VORBIS_HEADER_ONLY
#include "miniaudio/extras/stb_vorbis.c"  // Enables Vorbis decoding.
#define MINIAUDIO_IMPLEMENTATION
#include"miniaudio/miniaudio.h"
#undef  STB_VORBIS_HEADER_ONLY
#include "miniaudio/extras/stb_vorbis.c"  // Enables Vorbis decoding.
#define var auto

#undef AUDIO
#endif // AUDIO
constexpr int rendernum = 3;