#pragma once

#include <array>
#include <fstream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "Mesh.hpp"
#include "Model.hpp"

/**
 * @brief Splits a string at whitespace into a vector of strings
 *
 * @param s The string to split
 * @return std::vector<std::string> The split sections of the string
 */
inline std::vector<std::string> split_at_whitespace(const std::string &s)
{
	std::vector<std::string> result;
	std::istringstream ss(s);

	std::copy(std::istream_iterator<std::string>(ss),
		  std::istream_iterator<std::string>(),
		  std::back_inserter(result)); //
	return result;
}

/**
 * @brief Extracts the first number in a given string before the delimeter i.e 15 in 15/22/50
 *
 * @param str The string to extract the first number from
 * @param delim The delimeter
 * @return std::string A string representation of the number extracted
 */
inline std::string extract_first_num(const std::string &str, char delim)
{
	std::string result = str;
	if (str.find(delim) != str.npos)
	{
		result = str.substr(0, str.find('/'));
	}
	return result;
}

/**
 * @brief Loads a wavefront .obj file into a Model
 *
 * @param filename The .obj file to load
 * @param color The color of the model, defaults to white (1.0f, 1.0f, 1.0f)
 * @return std::optional<Model> Either None or the Model
 */
inline std::optional<Model> load_obj(std::string_view filename, std::array<GLfloat, 3> color = {1.0f, 1.0f, 1.0f})
{
	std::optional<Model> model;
	std::fstream file{std::string(filename)};
	if (!file)
	{
		return model; // couldn't open file
	}

	std::string line;
	std::vector<Vertex> vertices;
	std::vector<GLushort> indices;

	std::array<std::string, 3> pos_array_str;
	std::array<GLfloat, 3> pos_array_float;

	std::string index, index1, index2, index3, index4;

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		auto split_string = split_at_whitespace(line);

		std::string line_type = split_string[0];

		if (line_type == "v") // vertex information
		{
			for (int i = 0; i < 3; i++)
			{
				pos_array_float[i] = std::stof(split_string[i + 1]);
			}
			vertices.push_back(Vertex(pos_array_float, color));
		}

		else if (line_type == "f") // face information
		{
			if (split_string.size() == 5) // f looks like "f 1 2 3 4"
			{

				index1 = extract_first_num(split_string[1], '/');
				index2 = extract_first_num(split_string[2], '/');
				index3 = extract_first_num(split_string[3], '/');
				index4 = extract_first_num(split_string[4], '/');

				indices.push_back(std::stoi(index1) - 1);
				indices.push_back(std::stoi(index2) - 1);
				indices.push_back(std::stoi(index3) - 1);

				indices.push_back(std::stoi(index1) - 1);
				indices.push_back(std::stoi(index3) - 1);
				indices.push_back(std::stoi(index4) - 1);

				continue;
			}
			for (int i = 0; i < 3; i++)
			{
				index = split_string[i + 1];
				// just in case if line looks like "f 1/2/3 ..."
				index = extract_first_num(index, '/');

				indices.push_back(std::stoi(index) - 1); // faces start at 1 :O
			}
		}
	}

	model = std::move(Mesh(vertices, indices));
	return model;
};
