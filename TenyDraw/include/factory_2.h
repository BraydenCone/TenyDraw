#pragma once

#include <string>

#include "embedded.h"

#define MAX_COMMANDS 25
#define MAX_QUADS MAX_COMMANDS * 512
#define MAX_VERTICES (MAX_QUADS * 4)
#define MAX_INDICIES (MAX_QUADS * 6)
#define MAX_TEXTURES 1

namespace factory
{
#define PI 3.14159265358979323846f
#define MIN(a, b)         ((a) < (b) ? (a) : (b))
#define MAX(a, b)         ((a) > (b) ? (a) : (b))
#define CLAMP(x, a, b)    MIN(b, MAX(a, x))
#define DEG_TO_RAD (PI / 180.0f)
#define RAD_TO_DEG (180.0f / PI)

	typedef float vertice;
	typedef unsigned int indice;
	typedef unsigned int rgba;

	enum
	{
		x16 = 0,
		x32,
		x48,
		x64,
		max,
	};

	struct vec2
	{
		float x, y;

		vec2()
		{
			memset(this, 0, sizeof(*this));
		}

		vec2(float x, float y)
		{
			this->x = x;
			this->y = y;
		}
	};

	struct vec3
	{
		float x, y, z;

		vec3()
		{
			memset(this, 0, sizeof(*this));
		}

		vec3(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}
	};

	struct vec4
	{
		float x, y, z, w;

		vec4()
		{
			memset(this, 0, sizeof(*this));
		}

		vec4(float x, float y, float z, float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
	};

	template<typename T, uint64_t _capacity>
	struct new_stack
	{
	private:
		T _data[_capacity];
		uint64_t _size;
	public:
		typedef T value_type;
		typedef value_type* iterator;
	private:
	public:
		new_stack()
		{
			clear();
		}

		~new_stack()
		{
			clear();
		}

		inline T& operator[](uint64_t index)
		{
			return _data[index];
		}

		inline new_stack<T, _capacity>& operator=(const new_stack<T, _capacity>& src)
		{
			_size = src._size;
			for (uint64_t i = 0; i < _size; i++)
			{
				_data[i] = src._data[i];
			}
			return *this;
		}

		inline T* get(uint64_t index)
		{
			if (is_valid(index))
			{
				return _data[index];
			}
			return 0;
		}

		inline T* back()
		{
			return &_data[_size - 1];
		}

		inline T* begin()
		{
			return _data;
		}

		inline T* end()
		{
			return _data + _size;
		}

		inline T* data()
		{
			return _data;
		}

		inline bool empty() const
		{
			return _size == 0;
		}

		inline uint64_t size() const
		{
			return _size;
		}

		inline uint64_t capacity() const
		{
			return _capacity;
		}

		inline uint64_t max_size() const
		{
			return 0x7FFFFFFF / (uint64_t)sizeof(T);
		}

		inline void push_back(T value)
		{
			if (_size < _capacity)
			{
				_data[_size] = value;
				_size++;
			}
		}

		inline void pop_back()
		{
			if (_size > 0)
			{
				_size--;
			}
		}

		inline void expand(uint64_t value)
		{
			uint64_t new_size = _size + value;
			resize(new_size);
		}

		inline void shrink(uint64_t value)
		{
			uint64_t new_size = _size - value;
			resize(new_size);
		}

		inline void resize(uint64_t value)
		{
			if (is_valid(value))
			{
				_size = value;
			}
		}

		inline void clear()
		{
			memset(this, 0, sizeof(*this));
		}

		inline bool is_valid(uint64_t index)
		{
			return index >= 0 && index <= _capacity;
		}
	};

	struct vertex
	{
		vec3                                             position;
		vec2                                             uv;
		rgba                                             color;
	};									                 
										                 
	struct buffer_descriptor			                 
	{									                 
		vertex*                                          vertice_buffer;
		indice*                                          indice_buffer;
		indice*                                          index_size;
	};									                 
										                 
	struct glyph_base					                 
	{									                 
		int                                              advance;
		int                                              x0, y0, x1, y1;
		vec4                                             uv;
	};									                 
										                 
	struct glyph_container				                 
	{									                 
		glyph_base                                       glyhps[109];
	};									                 
										                 
	struct font							                 
	{									                 
		glyph_base*                                      quad;
		glyph_container*                                 glyph_containers[4];
		glyph_container*                                 current_container;
		float                                            size;
	};									                 
										                 
	struct batch_element				                 
	{									                 
		uint64_t                                         vertex_offset;
		uint64_t                                         index_offset;
		uint64_t                                         element_count;
		void*                                            vertex_shader;
		void*                                            pixel_shader;
		vec4                                             clip_rect;
	};

	struct batch_context
	{
	public:
		typedef new_stack<batch_element, MAX_COMMANDS>   command_stack;
		typedef new_stack<vertex, MAX_VERTICES>          vertice_stack;
		typedef new_stack<indice, MAX_INDICIES>          indicie_stack;

	private:
		command_stack                                    commands;
		vertice_stack                                    vertices;
		indicie_stack                                    indices;
						                                 
	public:				                                 
		factory::vertex*                                 write_vertex;
		factory::indice*                                 write_index;
		uint64_t                                         vertex_count;

		batch_context()
		{
			write_vertex   = 0;
			write_index    = 0;
			vertex_count   = 0;
		}

		void push_command(batch_element command)
		{
			commands.push_back(command);
		}

		void reserve(uint64_t vertice_size, uint64_t indicie_size)
		{
			commands.back()->element_count += indicie_size;

			write_vertex = vertices.data() + vertices.size();
			vertices.expand(vertice_size);

			write_index = indices.data() + indices.size();
			indices.expand(indicie_size);
		}

		void flush()
		{
			commands.resize(0);
			vertices.resize(0);
			indices.resize(0);
			write_vertex = 0;
			write_index = 0;
			vertex_count = 0;
		}

		void clear()
		{
			commands.clear();
			vertices.clear();
			indices.clear();
			write_vertex = 0;
			write_index = 0;
			vertex_count = 0;
		}

		void push_command(vec4 clip_rect)
		{
			batch_element command;
			command.vertex_offset   = vertices.size();
			command.index_offset    = indices.size();
			command.element_count   = 0;
			command.vertex_shader   = 0;
			command.pixel_shader    = 0;
			command.clip_rect       = clip_rect;
			commands.push_back(command);
		}

		command_stack* get_commands()
		{
			return &commands;
		}

		vertice_stack* get_vertices()
		{
			return &vertices;
		}

		indicie_stack* get_indicies()
		{
			return &indices;
		}

		inline void quad(vec2 position_1, vec2 position_2, vec2 uv_1, vec2 uv_2, float z, rgba color)
		{
			uint64_t index = vertex_count;

			write_index[0] = index + 0;
			write_index[1] = index + 1;
			write_index[2] = index + 2;
			write_index[3] = index + 0;
			write_index[4] = index + 2;
			write_index[5] = index + 3;

			write_vertex[0].position.x = position_1.x;
			write_vertex[0].position.y = position_1.y;
			write_vertex[0].position.z = z;
			write_vertex[0].uv.x = uv_1.x;
			write_vertex[0].uv.y = uv_1.y;
			write_vertex[0].color = color;

			write_vertex[1].position.x = position_2.x;
			write_vertex[1].position.y = position_1.y;
			write_vertex[1].position.z = z;
			write_vertex[1].uv.x = uv_2.x;
			write_vertex[1].uv.y = uv_1.y;
			write_vertex[1].color = color;

			write_vertex[2].position.x = position_2.x;
			write_vertex[2].position.y = position_2.y;
			write_vertex[2].position.z = z;
			write_vertex[2].uv.x = uv_2.x;
			write_vertex[2].uv.y = uv_2.y;
			write_vertex[2].color = color;

			write_vertex[3].position.x = position_1.x;
			write_vertex[3].position.y = position_2.y;
			write_vertex[3].position.z = z;
			write_vertex[3].uv.x = uv_1.x;
			write_vertex[3].uv.y = uv_2.y;
			write_vertex[3].color = color;

			vertex_count += 4;
			write_vertex += 4;
			write_index += 6;
		}
		
		void rect(vec2 position, vec2 size, rgba color);
		void text(vec2 position, float scale, const char* str, rgba color, uint64_t length);
		void glyph_icon(vec2 position, float scale, uint64_t code_point, rgba color);
	};

	batch_context* get_context();

	void set_font(uint64_t index);
	float get_font_size();

	glyph_base* get_glyph(uint64_t index);
	
	float get_string_width(const char* str, float scale, uint64_t length = 0);
}

namespace builder
{
	void annulus_base
	(
		factory::buffer_descriptor descriptor,
		factory::vec3 position, factory::vec2 size, unsigned int number_segments, factory::rgba color,
		float start, float end
	);

	void annulus_fragment
	(
		factory::buffer_descriptor descriptor,
		factory::vec3 position, factory::vec2 size, unsigned int number_segments, factory::rgba color,
		unsigned int number_fragments, unsigned int fragment
	);

	void annulus
	(
		factory::buffer_descriptor descriptor,
		factory::vec3 position, factory::vec2 size, unsigned int number_segments, factory::rgba color
	);

	void quad
	(
		factory::buffer_descriptor descriptor, factory::vec2 position_1, factory::vec2 position_2, factory::vec2 uv_1, factory::vec2 uv_2, float z, factory::rgba color
	);

	void rect_uv
	(
		factory::buffer_descriptor descriptor, factory::vec3 position, factory::vec2 size, factory::vec4 uv_coordinates, factory::rgba color
	);

	void rect
	(
		factory::buffer_descriptor descriptor, factory::vec3 position, factory::vec2 size, factory::rgba color
	);

	void text
	(
		factory::buffer_descriptor descriptor, factory::vec3 position, factory::rgba color, float scale, const char* str, uint64_t length
	);
}
