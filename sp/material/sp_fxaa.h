#pragma once

#include "../video/sp_texture.h"
#include "../video/sp_shader.h"
#include "../entity/sp_scene.h"
#include "sp_material.h"

struct sp_fxaa_params
{
    bool fxaa_enabled;
    glm::vec2 rcp_frame;
    float padding;
};

struct sp_fxaa
{
    sp_shader fxaa_shader;
    sp_material fxaa_material;
    sp_sampler fxaa_sampler;
    sp_buffer fxaa_parameters;
    sp_texture fxaa_texture;
    sp_fxaa_params params;
};

void sp_fxaa_init(sp_fxaa* fxaa);
void sp_fxaa_free(sp_fxaa* fxaa);
void sp_fxaa_update(sp_fxaa* fxaa, sp_texture* in_texture);
void sp_fxaa_resize(sp_fxaa* fxaa);