#include "sp_mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

#include "../sp_log.h"

sp_mesh sp_process_mesh(sp_model* mod, aiMesh* mesh, const aiScene* scene)
{
    sp_mesh out;

    std::vector<sp_vertex> vertices;
    std::vector<u32> indices;

    for (u32 i = 0; i < mesh->mNumVertices; i++)
    {
        sp_vertex vertex;

        vertex.px = mesh->mVertices[i].x;
        vertex.py = mesh->mVertices[i].y;
        vertex.pz = mesh->mVertices[i].z;

        if (mesh->mTextureCoords[0])
        {
            vertex.tx = mesh->mTextureCoords[0][i].x;
            vertex.ty = mesh->mTextureCoords[0][i].y;
        }

        if (mesh->HasNormals())
        {
            vertex.nx = mesh->mNormals[i].x;
            vertex.ny = mesh->mNormals[i].y;
            vertex.nz = mesh->mNormals[i].z;
        }

        vertices.push_back(vertex);
    }

    for (u32 i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (u32 j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    sp_buffer_create(&out.vertex_buffer, vertices.size() * sizeof(sp_vertex), sizeof(sp_vertex), sp_buffer_usage::vertex);
    sp_buffer_set_data(&out.vertex_buffer, vertices.data());
    sp_buffer_create(&out.index_buffer, indices.size() * sizeof(u32), 0, sp_buffer_usage::index);
    sp_buffer_set_data(&out.index_buffer, indices.data());

    out.vertex_count = (i32)mesh->mNumVertices;
    out.index_count = (i32)indices.size();

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // Albedo
    {
        aiString str;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        if (str.length)
        {
            std::string tex_path = mod->directory + '/' + str.C_Str();
            sp_texture_load(&out.albedo_texture, tex_path.c_str());
            sp_texture_init_srv(&out.albedo_texture);
        }
    }
    // Metallic Roughness
    {
        aiString str;
        material->GetTexture(aiTextureType_UNKNOWN, 0, &str);
        if (str.length)
        {
            std::string tex_path = mod->directory + '/' + str.C_Str();
            sp_texture_load(&out.metallic_roughness_texture, tex_path.c_str());
            sp_texture_init_srv(&out.metallic_roughness_texture);
        }
    }
    // Normal
    {
        aiString str;
        material->GetTexture(aiTextureType_NORMALS, 0, &str);
        if (str.length)
        {
            std::string tex_path = mod->directory + '/' + str.C_Str();
            sp_texture_load(&out.normal_texture, tex_path.c_str());
            sp_texture_init_srv(&out.normal_texture);
        }
    }

    return out;
}

void sp_process_node(sp_model* out, aiNode* node, const aiScene* scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        out->meshes.push_back(sp_process_mesh(out, mesh, scene));
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        sp_process_node(out, node->mChildren[i], scene);
    }
}

void sp_model_load(sp_model* mod, const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        sp_log_err("Failed to load model with path %s", path.c_str());
        sp_log_crit("%s", importer.GetErrorString());
    }
    mod->directory = path.substr(0, path.find_last_of('/'));
    sp_process_node(mod, scene->mRootNode, scene);
}

void sp_model_free(sp_model* mod)
{
    for (sp_mesh mesh : mod->meshes)
    {
        sp_texture_free(&mesh.normal_texture);
        sp_texture_free(&mesh.metallic_roughness_texture);
        sp_texture_free(&mesh.albedo_texture);
        sp_buffer_free(&mesh.index_buffer);
        sp_buffer_free(&mesh.vertex_buffer);
    }
}