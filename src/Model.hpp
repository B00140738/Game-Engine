#pragma once

#include <array>
#include <iostream>
#include <cstring>


#include <GL/glew.h>

#include "Mesh.hpp"
#include "Vertex.hpp"

class Model
{
	Mesh m_mesh;
	GLuint m_vao;
	GLuint m_vbos[2];
	GLuint m_ebo;
	bool should_be_destroyed;

	/**
	 * @brief Set the up OpenGL buffers (vbo for position, vbo for color, ebo for indices, and vao to store buffers)
	 *
	 */
	void setup_opengl_bs()
	{
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(2, m_vbos);

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbos[0]);
		glBufferData(GL_ARRAY_BUFFER,
			     m_mesh.vertices.size() * sizeof(Vertex),
			     m_mesh.vertices.data(),
			     GL_STATIC_DRAW); //

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbos[1]);
		glBufferData(GL_ARRAY_BUFFER,
			     m_mesh.vertices.size() * sizeof(Vertex),
			     m_mesh.vertices.data(),
			     GL_STATIC_DRAW); //

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &m_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			     m_mesh.indices.size() * sizeof(GLushort),
			     m_mesh.indices.data(),
			     GL_STATIC_DRAW);
	}

    public:
	friend class Renderer;

	template <size_t vertex_num, size_t index_count>
	Model(std::array<GLfloat, vertex_num> positions,
	      std::array<GLfloat, vertex_num> colors,
	      std::array<GLushort, index_count> indices)
	    : m_mesh(positions, colors, indices), should_be_destroyed(true)
	{
		setup_opengl_bs();
	}

	Model(Mesh m) : m_mesh(m), should_be_destroyed(true) { setup_opengl_bs(); }

	Model(Model &&other) : m_mesh(other.m_mesh), m_vao(other.m_vao), should_be_destroyed(true)
	{
		std::memcpy(this->m_vbos, other.m_vbos, 2 * sizeof(GLuint));
		other.m_vao = 0;
		std::memset(other.m_vbos, 0, 2 * sizeof(GLuint));
		other.should_be_destroyed = false;
	}

	/**
	 * @brief Destructive move - A model should have unique information, so copying models doesn;t really make sense
	 *
	 * @param other Other model to move from
	 * @return Model&
	 */
	Model &operator=(Model &&other)
	{
		if (&other == this)
		{
			return *this;
		}
		this->cleanup();

		this->m_mesh = other.m_mesh;
		this->m_vao = other.m_vao;
		this->should_be_destroyed = true;
		std::memcpy(this->m_vbos, other.m_vbos, 2 * sizeof(GLuint));

		other.m_vao = 0;
		other.should_be_destroyed = false;
		std::memset(other.m_vbos, 0, 2 * sizeof(GLuint));

		return *this;
	}

	/**
	 * @brief Deletes buffers and vao
	 *
	 */
	void cleanup()
	{
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(2, m_vbos);
		glDeleteBuffers(1, &m_ebo);
		should_be_destroyed = false;
	}

	/**
	 * @brief Prints simple info, like memory address, vao id, vbos id, num vertices, and num indices
	 *
	 */
	void print_debug_info()
	{
		std::cout << "Model " << this << " info:\n";
		std::cout << '\t' << this->m_vao << '\n';
		std::cout << '\t' << this->m_vbos[0] << '\n';
		std::cout << '\t' << this->m_vbos[1] << '\n';
		std::cout << '\t' << "Num vertices: " << this->m_mesh.vertices.size() << '\n';
		std::cout << '\t' << "Num indices: " << this->m_mesh.indices.size() << '\n';
	}

	/**
	 * @brief Destroy the Model object if it should be destroyed
	 *
	 */
	~Model()
	{
		if (should_be_destroyed)
		{
			cleanup();
		}
	}
};