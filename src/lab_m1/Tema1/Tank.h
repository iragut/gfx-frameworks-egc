#pragma once

#include "components/simple_scene.h"
#include "core/engine.h"
#include "utils/gl_utils.h"

namespace tank 
{
	Mesh* initTank(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
}