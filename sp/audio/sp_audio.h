#pragma once

#include "../sp_common.h"

#include <miniaudio/miniaudio.h>
#include <dr_libs/dr_wav.h>

struct sp_audio_clip
{
    ma_decoder decoder;
	ma_decoder_config decoder_config;

	bool playing;
	bool loop;

    i32 id;
	drwav wav;
};

void sp_audio_init();
void sp_audio_free();
void sp_audio_update();

void sp_audio_clip_load_wav(sp_audio_clip* clip, const char* path);
void sp_audio_clip_free(sp_audio_clip* clip);
void sp_audio_clip_play(sp_audio_clip* clip);
void sp_audio_clip_stop(sp_audio_clip* clip);
void sp_audio_clip_loop(sp_audio_clip* clip, bool loop);