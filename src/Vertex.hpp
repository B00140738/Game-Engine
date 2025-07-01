#pragma once

#include <GL/glew.h>
#include <array>
#include <iostream>

/**
 * @brief Holds the position (x, y, z) and color (r, g, b) of a vertex
 *
 */
struct Vertex
{
	std::array<GLfloat, 3> position;
	std::array<GLfloat, 3> color;

	Vertex(std::array<GLfloat, 3> position, std::array<GLfloat, 3> color) : position(position), color(color) {}

	friend std::ostream &operator<<(std::ostream &o, const Vertex &v)
	{
		o << "Position: {";
		for (int i = 0; i < 3; i++)
		{
			o << v.position[i] << ", ";
		}
		o << "}\n";

		o << "Color: {";
		for (int i = 0; i < 3; i++)
		{
			o << v.color[i] << ", ";
		}
		o << "}\n";

		return o;
	}
};
