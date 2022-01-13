#include "sp_entity.h"

#include <time.h>

void sp_entity_init(sp_entity* out, std::string name)
{
    out->name = name;
    out->id = rand();
    out->transform = glm::mat4(1.0f);
    out->position = glm::vec3(0.0f);
    out->scale = glm::vec3(1.0f);
    out->rotation = glm::vec3(0.0f);

    sp_buffer_create(&out->gpu_transform, sizeof(glm::mat4), 0, sp_buffer_usage::uniform);
}

void sp_entity_free(sp_entity* entity)
{
    if (entity->audio_clip.id != -1) sp_audio_clip_free(&entity->audio_clip);
    if (!entity->render_model.meshes.empty()) sp_model_free(&entity->render_model);
    sp_buffer_free(&entity->gpu_transform);
    entity->id = -1;
}

void sp_entity_load_mesh(sp_entity* entity, std::string path)
{
    sp_model_load(&entity->render_model, path);
}

void sp_entity_load_audio(sp_entity* entity, std::string path)
{
    sp_audio_clip_load_wav(&entity->audio_clip, path.c_str());
}

void sp_entity_set_material_index(sp_entity* entity, i32 material_index)
{
    entity->material_index = material_index;
}

void sp_entity_set_position(sp_entity* entity, f32 x, f32 y, f32 z)
{
    entity->position = glm::vec3(x, y, z);
}

void sp_entity_set_scale(sp_entity* entity, f32 x, f32 y, f32 z)
{
    entity->scale = glm::vec3(x, y, z);
}

void sp_entity_set_rotation(sp_entity* entity, f32 x, f32 y, f32 z)
{
    entity->rotation = glm::vec3(x, y, z);
}