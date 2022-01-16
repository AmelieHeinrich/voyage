#pragma once

#include "../video/sp_shader.h"
#include "../video/sp_texture.h"
#include "../entity/sp_scene.h"

#include "sp_env_map.h"

struct sp_forward
{
    sp_shader forward_shader;
    sp_texture rtv;
    sp_sampler texture_sampler;
};

void sp_forward_init(sp_forward* forward);
void sp_forward_free(sp_forward* forward);
void sp_forward_update(sp_forward* forward, sp_scene* scene, sp_env_map* map);
void sp_forward_resize(sp_forward* forward);