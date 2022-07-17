-- Entity table
light_sphere = {
	handle
}

function light_sphere:init()
	sp_entity_set_position(light_sphere.handle, 3.0, 3.0, 3.0)
	sp_entity_set_light_source(light_sphere.handle, true)
	sp_entity_set_light_color(light_sphere.handle, 10.0, 10.0, 10.0)
end

function light_sphere:update()

end

function light_sphere:free()

end
