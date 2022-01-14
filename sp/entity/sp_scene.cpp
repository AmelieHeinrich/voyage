#include "sp_scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include "../script/sp_script_engine.h"

void sp_scene_init(sp_scene* scene)
{
    scene->scene_camera.view = glm::mat4(1.0f);
    scene->scene_camera.projection = glm::mat4(1.0f);
    sp_buffer_create(&scene->camera_buffer, sizeof(scene->scene_camera), 0, sp_buffer_usage::uniform);
}

void sp_scene_free(sp_scene* scene)
{
    sp_buffer_free(&scene->camera_buffer);
    for (auto material : scene->materials)
        sp_material_free(&material.second);
    for (auto entity : scene->entities)
        sp_entity_free(&entity.second);
}

void sp_scene_push_entity(sp_scene* scene, sp_entity entity)
{
    scene->entities[entity.id] = entity;
}

void sp_scene_pop_entity(sp_scene* scene, sp_entity entity)
{
    sp_entity_free(&scene->entities[entity.id]);
}

void sp_scene_push_material(sp_scene* scene, sp_material_info mat)
{
    sp_material material;
    sp_material_create(&material, mat);
    scene->materials[material.id] = material;
}

void sp_scene_pop_material(sp_scene* scene, i32 mat_index)
{
    sp_material_free(&scene->materials[mat_index]);
}

void sp_scene_on_init(sp_scene* scene)
{
    for (auto entity : scene->entities)
    {
        if (entity.second.scripted)
            sp_script_engine_init_entity(&entity.second);
    }
}

void sp_scene_on_update(sp_scene* scene)
{
    sp_buffer_set_data(&scene->camera_buffer, &scene->scene_camera);
    for (auto entity : scene->entities)
    {
        entity.second.transform = glm::translate(glm::mat4(1.0f), entity.second.position) 
                                * glm::scale(glm::mat4(1.0f), entity.second.scale)
                                * glm::rotate(glm::mat4(1.0f), glm::radians(entity.second.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))
                                * glm::rotate(glm::mat4(1.0f), glm::radians(entity.second.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
                                * glm::rotate(glm::mat4(1.0f), glm::radians(entity.second.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        sp_buffer_set_data(&entity.second.gpu_transform, &entity.second.transform);

        if (entity.second.scripted)
            sp_script_engine_update_entity(&entity.second);
    }
}

void sp_scene_on_free(sp_scene* scene)
{
    for (auto entity : scene->entities)
    {
        if (entity.second.scripted)
            sp_script_engine_free_entity(&entity.second);
    }
}