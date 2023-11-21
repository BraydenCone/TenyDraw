#include "factory_2.h"

static factory::font g_font =
{
	(factory::glyph_base*)(&get_glyph_data()[QUAD_OFFSET]),
	(factory::glyph_container*)(&get_glyph_data()[X16_OFFSET]),
	(factory::glyph_container*)(&get_glyph_data()[X32_OFFSET]),
	(factory::glyph_container*)(&get_glyph_data()[X48_OFFSET]),
	(factory::glyph_container*)(&get_glyph_data()[X64_OFFSET]),
	g_font.glyph_containers[0],
	16
};

static factory::vec2 g_white_quad_coords =
{
	g_font.quad->uv.x + ((g_font.quad->uv.z - g_font.quad->uv.x) / 2.0f),
	g_font.quad->uv.y + ((g_font.quad->uv.w - g_font.quad->uv.y) / 2.0f)
};

static factory::batch_context g_context;

void factory::batch_context::rect(vec2 position, vec2 size, factory::rgba color)
{
	vec4 uv = vec4(g_white_quad_coords.x, g_white_quad_coords.y, g_white_quad_coords.x, g_white_quad_coords.y);
	vec2 p1 = { position.x, position.y };
	vec2 p2 = { position.x + size.x, position.y + size.y };
	reserve(4, 6);
	quad(p1, p2, g_white_quad_coords, g_white_quad_coords, 0.0f, color);
}

void factory::batch_context::text(vec2 position, float scale, const char* str, factory::rgba color, uint64_t length)
{
	reserve(4 * length, 6 * length);

	factory::indice index = vertex_count;

	for (uint64_t i = 0; i < length; i++)
	{
		glyph_base glyph = *get_glyph(str[i] - 32);

		vec2 uv_a(glyph.uv.x, glyph.uv.y);
		vec2 uv_c(glyph.uv.z, glyph.uv.w);
		vec2 uv_b(uv_c.x, uv_a.y);
		vec2 uv_d(uv_a.x, uv_c.y);

		vec3 pos_a(position.x + (float)glyph.x0, position.y - ((float)glyph.y0 * scale), 0.0f);
		vec3 pos_c(pos_a.x + ((float)glyph.x1 * scale), pos_a.y + ((float)glyph.y1 * scale), 0.0f);
		vec3 pos_b(pos_c.x, pos_a.y, 0.0f);
		vec3 pos_d(pos_a.x, pos_c.y, 0.0f);

		write_index[0] = index + 0;
		write_index[1] = index + 1;
		write_index[2] = index + 2;
		write_index[3] = index + 0;
		write_index[4] = index + 2;
		write_index[5] = index + 3;

		write_vertex[0].position = pos_a;
		write_vertex[0].uv = uv_a;
		write_vertex[0].color = color;

		write_vertex[1].position = pos_b;
		write_vertex[1].uv = uv_b;
		write_vertex[1].color = color;

		write_vertex[2].position = pos_c;
		write_vertex[2].uv = uv_c;
		write_vertex[2].color = color;

		write_vertex[3].position = pos_d;
		write_vertex[3].uv = uv_d;
		write_vertex[3].color = color;

		write_vertex += 4;
		write_index += 6;
		index += 4;

		position.x += ((float)glyph.advance * scale);
	}

	vertex_count = index;
}

factory::batch_context* factory::get_context()
{
	return &g_context;
}

void factory::set_font(uint64_t index)
{
	uint64_t i = (index + max) % max;
	g_font.current_container = g_font.glyph_containers[i];
	g_font.size = (float)(16 * (i + 1));
}

float factory::get_font_size()
{
	return g_font.size;
}

factory::glyph_base* factory::get_glyph(uint64_t index)
{
	if (g_font.current_container)
	{
		index = CLAMP(index, 0, NUMBER_GLYPHS_SECTION);
		return &g_font.current_container->glyhps[index];
	}
	return 0;
}

float factory::get_string_width(const char* str, float scale, uint64_t length)
{
	float width = 0.0f;
	if (!length)
	{
		length = strlen(str);
	}

	for (uint64_t i = 0; i < length; i++)
	{
		uint64_t index = str[i] - 32;
		width += g_font.current_container->glyhps[index].advance * scale;
	}

	return width;
}

void builder::annulus_base
(
	factory::buffer_descriptor descriptor,
	factory::vec3 position, factory::vec2 size, unsigned int number_segments, factory::rgba color,
	float start, float end
)
{
	float x1, y1, x2, y2;
	float degree_increment, radius;
	factory::indice index;

	degree_increment = (end - start) / (float)number_segments;
	
	index = *descriptor.index_size;

	for (float degree = start; degree < end; degree += degree_increment)
	{
		radius = -(degree * DEG_TO_RAD);
		x1 = cos(radius);
		y1 = sin(radius);

		radius = (-(degree + degree_increment)) * DEG_TO_RAD;
		x2 = cos(radius);
		y2 = sin(radius);

		descriptor.indice_buffer[0] = index + 0;
		descriptor.indice_buffer[1] = index + 1;
		descriptor.indice_buffer[2] = index + 3;
		descriptor.indice_buffer[3] = index + 1;
		descriptor.indice_buffer[4] = index + 2;
		descriptor.indice_buffer[5] = index + 3;

		descriptor.vertice_buffer[0].position = factory::vec3(position.x + (x2 * size.y), position.y + (y2 * size.y), position.z);
		descriptor.vertice_buffer[0].uv = g_white_quad_coords;
		descriptor.vertice_buffer[0].color = color;

		descriptor.vertice_buffer[1].position = factory::vec3(position.x + (x1 * size.y), position.y + (y1 * size.y), position.z);
		descriptor.vertice_buffer[1].uv = g_white_quad_coords;
		descriptor.vertice_buffer[1].color = color;

		descriptor.vertice_buffer[2].position = factory::vec3(position.x + (x1 * size.x), position.y + (y1 * size.x), position.z);
		descriptor.vertice_buffer[2].uv = g_white_quad_coords;
		descriptor.vertice_buffer[2].color = color;

		descriptor.vertice_buffer[3].position = factory::vec3(position.x + (x2 * size.x), position.y + (y2 * size.x), position.z);
		descriptor.vertice_buffer[3].uv = g_white_quad_coords;
		descriptor.vertice_buffer[3].color = color;

		descriptor.indice_buffer += 6;
		descriptor.vertice_buffer += 4;
		index += 4;
	}
	*descriptor.index_size = index;
}

void builder::annulus_fragment
(
	factory::buffer_descriptor descriptor,
	factory::vec3 position, factory::vec2 size, unsigned int number_segments, factory::rgba color,
	unsigned int number_fragments, unsigned int fragment
)
{
	float start, end, increment;
	increment = 360.0f / (float)number_fragments;
	start = increment * (float)fragment;
	end = start + increment;
	builder::annulus_base(descriptor, position, size, number_segments, color, start, end);
}

void builder::annulus
(
	factory::buffer_descriptor descriptor,
	factory::vec3 position, factory::vec2 size, unsigned int number_segments, factory::rgba color
)
{
	builder::annulus_base(descriptor, position, size, number_segments, color, 0.0f, 360.0f);
}

void builder::quad
(
	factory::buffer_descriptor descriptor, factory::vec2 position_1, factory::vec2 position_2, factory::vec2 uv_1, factory::vec2 uv_2, float z, factory::rgba color
)
{
	factory::indice index = *descriptor.index_size;

	descriptor.indice_buffer[0] = index + 0;
	descriptor.indice_buffer[1] = index + 1;
	descriptor.indice_buffer[2] = index + 2;
	descriptor.indice_buffer[3] = index + 0;
	descriptor.indice_buffer[4] = index + 2;
	descriptor.indice_buffer[5] = index + 3;

	// Top Left
	descriptor.vertice_buffer[0].position.x = position_1.x;
	descriptor.vertice_buffer[0].position.y = position_1.y;
	descriptor.vertice_buffer[0].position.z = z;
	descriptor.vertice_buffer[0].uv.x = uv_1.x;
	descriptor.vertice_buffer[0].uv.y = uv_1.y;
	descriptor.vertice_buffer[0].color = color;

	// Top Right
	descriptor.vertice_buffer[1].position.x = position_2.x;
	descriptor.vertice_buffer[1].position.y = position_1.y;
	descriptor.vertice_buffer[1].position.z = z;
	descriptor.vertice_buffer[1].uv.x = uv_2.x;
	descriptor.vertice_buffer[1].uv.y = uv_1.y;
	descriptor.vertice_buffer[1].color = color;

	// Bottom Right
	descriptor.vertice_buffer[2].position.x = position_2.x;
	descriptor.vertice_buffer[2].position.y = position_2.y;
	descriptor.vertice_buffer[2].position.z = z;
	descriptor.vertice_buffer[2].uv.x = uv_2.x;
	descriptor.vertice_buffer[2].uv.y = uv_2.y;
	descriptor.vertice_buffer[2].color = color;

	// Bottom Left
	descriptor.vertice_buffer[3].position.x = position_1.x;
	descriptor.vertice_buffer[3].position.y = position_2.y;
	descriptor.vertice_buffer[3].position.z = z;
	descriptor.vertice_buffer[3].uv.x = uv_1.x;
	descriptor.vertice_buffer[3].uv.y = uv_2.y;
	descriptor.vertice_buffer[3].color = color;

	*descriptor.index_size += 4;
}

void builder::rect_uv
(
	factory::buffer_descriptor descriptor, factory::vec3 position, factory::vec2 size, factory::vec4 uv, factory::rgba color
)
{
	factory::indice index = *descriptor.index_size;

	descriptor.indice_buffer[0] = index + 0;
	descriptor.indice_buffer[1] = index + 1;
	descriptor.indice_buffer[2] = index + 2;
	descriptor.indice_buffer[3] = index + 0;
	descriptor.indice_buffer[4] = index + 2;
	descriptor.indice_buffer[5] = index + 3;

	// Top Left
	descriptor.vertice_buffer[0].position = factory::vec3(position.x, position.y, position.z);
	descriptor.vertice_buffer[0].uv = factory::vec2(uv.x, uv.y);
	descriptor.vertice_buffer[0].color = color;

	// Top Right
	descriptor.vertice_buffer[1].position = factory::vec3(position.x + size.x, position.y, position.z);
	descriptor.vertice_buffer[1].uv = factory::vec2(uv.z, uv.y);
	descriptor.vertice_buffer[1].color = color;

	// Bottom Right
	descriptor.vertice_buffer[2].position = factory::vec3(position.x + size.x, position.y + size.y, position.z);
	descriptor.vertice_buffer[2].uv = factory::vec2(uv.z, uv.w);
	descriptor.vertice_buffer[2].color = color;

	// Bottom Left
	descriptor.vertice_buffer[3].position = factory::vec3(position.x, position.y + size.y, position.z);
	descriptor.vertice_buffer[3].uv = factory::vec2(uv.x, uv.w);
	descriptor.vertice_buffer[3].color = color;

	*descriptor.index_size += 4;
}

void builder::rect
(
	factory::buffer_descriptor descriptor, factory::vec3 position, factory::vec2 size, factory::rgba color
)
{
	factory::vec4 uv = factory::vec4(g_white_quad_coords.x, g_white_quad_coords.y, g_white_quad_coords.x, g_white_quad_coords.y);
	//rect_uv(descriptor, position, size, uv, color);
	factory::vec2 p1 = { position.x, position.y };
	factory::vec2 p2 = { position.x + size.x, position.y + size.y };
	quad(descriptor, p1, p2, g_white_quad_coords, g_white_quad_coords, position.z, color);
}

void builder::text
(
	factory::buffer_descriptor descriptor, factory::vec3 position, factory::rgba color, float scale, const char* str, uint64_t length
)
{
	factory::indice index = *descriptor.index_size;

	for (uint64_t i = 0; i < length; i++)
	{
		factory::glyph_base glyph = g_font.current_container->glyhps[str[i] - 32];
		
		factory::vec2 uv_a(glyph.uv.x, glyph.uv.y);
		factory::vec2 uv_c(glyph.uv.z, glyph.uv.w);
		factory::vec2 uv_b(uv_c.x, uv_a.y);
		factory::vec2 uv_d(uv_a.x, uv_c.y);

		factory::vec3 pos_a(position.x + (float)glyph.x0, position.y - ((float)glyph.y0 * scale), position.z);
		factory::vec3 pos_c(pos_a.x + ((float)glyph.x1 * scale), pos_a.y + ((float)glyph.y1 * scale), position.z);
		factory::vec3 pos_b(pos_c.x, pos_a.y, position.z);
		factory::vec3 pos_d(pos_a.x, pos_c.y, position.z);

		descriptor.indice_buffer[0] = index + 0;
		descriptor.indice_buffer[1] = index + 1;
		descriptor.indice_buffer[2] = index + 2;
		descriptor.indice_buffer[3] = index + 0;
		descriptor.indice_buffer[4] = index + 2;
		descriptor.indice_buffer[5] = index + 3;

		// Top Left
		descriptor.vertice_buffer[0].position = pos_a;
		descriptor.vertice_buffer[0].uv = uv_a;
		descriptor.vertice_buffer[0].color = color;

		// Top Right
		descriptor.vertice_buffer[1].position = pos_b;
		descriptor.vertice_buffer[1].uv = uv_b;
		descriptor.vertice_buffer[1].color = color;

		// Bottom Right
		descriptor.vertice_buffer[2].position = pos_c;
		descriptor.vertice_buffer[2].uv = uv_c;
		descriptor.vertice_buffer[2].color = color;

		// Bottom Left
		descriptor.vertice_buffer[3].position = pos_d;
		descriptor.vertice_buffer[3].uv = uv_d;
		descriptor.vertice_buffer[3].color = color;

		descriptor.indice_buffer += 6;
		descriptor.vertice_buffer += 4;
		index += 4;

		position.x += ((float)glyph.advance * scale);
	}
	*descriptor.index_size = index;
}
