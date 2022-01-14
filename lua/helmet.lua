-- Entity table
helmet = {
    handle
}

function helmet:init()
    sp_entity_load_mesh(helmet.handle, "data/models/helmet/DamagedHelmet.gltf")
    sp_entity_load_audio(helmet.handle, "data/audio/music.wav")
    sp_entity_set_rotation(helmet.handle, 90.0, 0.0, 0.0)
    sp_entity_set_material_index(helmet.handle, 0)
    --sp_entity_play_audio(helmet.handle)
end

function helmet:update()
    
end

function helmet:free()
    
end