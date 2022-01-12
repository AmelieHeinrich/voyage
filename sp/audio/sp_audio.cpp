#include "sp_audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>

#include <assert.h>
#include <vector>
#include "../sp_log.h"

#define SP_SAMPLE_FORMAT ma_format_f32
#define SP_CHANNEL_COUNT 2
#define SP_SAMPLE_RATE 48000
#define SP_MAX_AUDIO_CLIPS 1024

struct sp_audio_ctx
{
    ma_device device;
    ma_device_config device_config;

    sp_audio_clip* clips[SP_MAX_AUDIO_CLIPS];
    i32 clip_count;
};

sp_audio_ctx ctx;

u32 sp_read_and_mix(ma_decoder* decoder, float* out, u32 frame_count) {
	float temp[4096];
	u32 temp_cap = (sizeof(temp) / sizeof(float)) / SP_CHANNEL_COUNT;
	u32 read = 0;

	while (read < frame_count) {
		u32 remaining = frame_count - read;
		u32 to_read = temp_cap;

		if (to_read > remaining) {
			to_read = remaining;
		}

		u32 frames_read = (u32)ma_decoder_read_pcm_frames(decoder, temp, to_read);
		if (frames_read == 0) {
			break;
		}

		for (u32 i = 0; i < frames_read * SP_CHANNEL_COUNT; i++) {
			out[read * SP_CHANNEL_COUNT + i] += temp[i];
		}

		read += frames_read;

		if (frames_read < to_read) {
			break;
		}
	}

	return read;
}

void sp_data_callback(ma_device* device, void* out, const void* in, u32 frame_count)
{
    assert(device->playback.format == SP_SAMPLE_FORMAT);

    for (u32 i = 0; i < ctx.clip_count; i++) {
		sp_audio_clip* clip = ctx.clips[i];

		if (clip->playing) {
			u32 read = sp_read_and_mix(&clip->decoder, (float*)out, frame_count);
			if (read < frame_count) {
				clip->playing = false;
			}
		}
	}

	(void)in;
}

void sp_audio_init()
{
    ctx.device_config = ma_device_config_init(ma_device_type_playback);
    ctx.device_config.playback.format = SP_SAMPLE_FORMAT;
    ctx.device_config.playback.channels = SP_CHANNEL_COUNT;
    ctx.device_config.sampleRate = SP_SAMPLE_RATE;
    ctx.device_config.dataCallback = sp_data_callback;
    ctx.device_config.pUserData = NULL;

    ma_device_init(NULL, &ctx.device_config, &ctx.device);
    ma_device_start(&ctx.device);
}

void sp_audio_free()
{
    ma_device_stop(&ctx.device);
    ma_device_uninit(&ctx.device);
}

void sp_audio_update()
{
    for (u32 i = 0; i < ctx.clip_count; i++) {
		sp_audio_clip* clip = ctx.clips[i];

		if (!clip->playing) {
			sp_audio_clip_stop(clip);
			if (clip->loop) {
				sp_audio_clip_play(clip);
			}
		}
	}
}

void sp_audio_clip_init(sp_audio_clip* clip, u8* data, u64 size)
{
	clip->data = data;
	clip->data_size = size;

	clip->decoder_config = ma_decoder_config_init(SP_SAMPLE_FORMAT, SP_CHANNEL_COUNT, SP_SAMPLE_RATE);
	ma_result r = ma_decoder_init_memory(clip->data, clip->data_size, &clip->decoder_config, &clip->decoder);
	if (r != MA_SUCCESS) {
		sp_log_crit("Failed to create audio clip.");
		ma_decoder_uninit(&clip->decoder);
	}
}

void sp_audio_clip_free(sp_audio_clip* clip)
{
    if (clip->playing) {
		sp_audio_clip_stop(clip);
	}

	ma_decoder_uninit(&clip->decoder);

	free(clip->data);
    free(clip);
}

void sp_audio_clip_play(sp_audio_clip* clip)
{
    for (u32 i = 0; i < ctx.clip_count; i++) {
		if (ctx.clips[i] == clip) {
			sp_audio_clip_stop(ctx.clips[i]);
			break;
		}
	}

	ma_decoder_seek_to_pcm_frame(&clip->decoder, 0);

	clip->playing = true;
	ctx.clips[ctx.clip_count] = clip;
	clip->id = ctx.clip_count++;
}

void sp_audio_clip_stop(sp_audio_clip* clip)
{   
    clip->playing = false;

	ctx.clips[clip->id] = ctx.clips[ctx.clip_count - 1];
	ctx.clips[clip->id]->id = clip->id;
    ctx.clip_count--;
}

void sp_audio_clip_loop(sp_audio_clip* clip, bool loop)
{
    clip->loop = loop;
}