#pragma once

#include "../video/sp_texture.h"
#include "../video/sp_shader.h"
#include "../entity/sp_scene.h"
#include "sp_material.h"

struct sp_bloom_params
{
    bool bloom_enabled;
    float bloom_threshold;
    glm::vec2 padding;
};

struct sp_bloom
{
    sp_shader bloom_shader;
    sp_shader vertical_blur_shader;
    sp_shader horizontal_blur_shader;
    sp_shader combine_shader;
    sp_texture bright_texture;
    sp_texture blur_texture;
    sp_texture combine_texture;
    sp_sampler bloom_sampler;
    sp_buffer bloom_parameters;
    sp_buffer blur_buffer;
    sp_bloom_params params;
};

void sp_bloom_init(sp_bloom* bloom);
void sp_bloom_free(sp_bloom* bloom);
void sp_bloom_update(sp_bloom* bloom, sp_texture* in_texture);
void sp_bloom_resize(sp_bloom* bloom);