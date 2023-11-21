#pragma once

#include "factory_2.h"

namespace bcdx11
{
	int initialize(void* device, void* context);
	int create_shaders_sdf();
	int create_textures();
	int create_buffers();
	int create_state();
	
	void resize(unsigned int width, unsigned int height);
	void clear_device_objects();
	void set_view_port();
	void set_matrix(float x, float y, float width, float height);
	void set_state();
	void shutdown();
	void render();
}
