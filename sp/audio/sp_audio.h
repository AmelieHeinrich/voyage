#pragma once

#include "../sp_common.h"

#include <miniaudio/miniaudio.h>

struct sp_audio_clip
{
    ma_decoder decoder;
	ma_decoder_config decoder_config;

	u8* data;
	u64 data_size;

	bool playing;
	bool loop;

    i32 id;
};

void sp_audio_init();
void sp_audio_free();
void sp_audio_update();

void sp_audio_clip_init(sp_audio_clip* clip, u8* data, u64 size);
void sp_audio_clip_free(sp_audio_clip* clip);
void sp_audio_clip_play(sp_audio_clip* clip);
void sp_audio_clip_stop(sp_audio_clip* clip);
void sp_audio_clip_loop(sp_audio_clip* clip, bool loop);