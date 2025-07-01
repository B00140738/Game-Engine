#pragma once

#include <GL/glew.h>
#include <vector>

#include "Vertex.hpp"

/**
 * @brief A mesh holdes vertices and indices
 *
 */
struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<GLushort> indices;

	Mesh(std::vector<Vertex> vertices, std::vector<GLushort> indices) : vertices(vertices), indices(indices) {}

	template <size_t vertex_num, size_t index_count>
	Mesh(std::array<GLfloat, vertex_num> positions,
	     std::array<GLfloat, vertex_num> colors,
	     std::array<GLushort, index_count> indices)
	{
		vertices.reserve(positions.size() / 3);
		for (int i = 0; i < positions.size(); i += 3)
		{
			std::array<GLfloat, 3> position;
			std::array<GLfloat, 3> color;

			position[0] = positions[i];
			position[1] = positions[i + 1];
			position[2] = positions[i + 2];

			color[0] = colors[i];
			color[1] = colors[i + 1];
			color[2] = colors[i + 2];

			vertices.push_back(Vertex(position, color));
		}
		this->indices.reserve(indices.size());
		this->indices.insert(this->indices.end(), indices.begin(), indices.end());
	}
};
