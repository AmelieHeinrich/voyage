#include "sp_audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>

#define DR_WAV_IMPLEMENTATION
#include <dr_libs/dr_wav.h>

#include <assert.h>
#include <vector>
#include "../sp_log.h"

#define SP_SAMPLE_FORMAT ma_format_s16
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

void sp_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    /* Assuming format is always s16 for now. */
    for (i32 i = 0; i < ctx.clip_count; i++) {
		if (pDevice->playback.format == ma_format_s16) {
       		drwav_read_pcm_frames_s16(&ctx.clips[i]->wav, frameCount, (drwav_int16*)pOutput);
    	} else if (pDevice->playback.format == ma_format_f32) {
    	    drwav_read_pcm_frames_f32(&ctx.clips[i]->wav, frameCount, (float*)pOutput);
    	} else {
    	    /* Unsupported format. */
    	}
	}

    (void)pInput;
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

void sp_audio_clip_load_wav(sp_audio_clip* clip, const char* path)
{
	drwav_init_file(&clip->wav, path, NULL);
}

void sp_audio_clip_free(sp_audio_clip* clip)
{
    if (clip->playing) {
		sp_audio_clip_stop(clip);
	}

	ma_decoder_uninit(&clip->decoder);

	drwav_uninit(&clip->wav);
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