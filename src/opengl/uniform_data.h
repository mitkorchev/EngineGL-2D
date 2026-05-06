#pragma once

#include <variant>

#include "../../common/common.h"

/*

The point of this class hierarchy is to enable setting of custom/additional
uniforms without re-writing the whole Renderer2D class.

*/


using UniformValue = std::variant<
	int,
	float,
	glm::mat4,
	glm::vec2,
	glm::vec3,
	glm::vec4
>;

struct UniformData {
	std::string name;
	UniformValue value;

	UniformData(const char* _name, UniformValue _val)
		:name(_name), value(_val) {}

};

typedef std::vector<UniformData> UniformDataVector;