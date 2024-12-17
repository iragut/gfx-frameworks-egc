#pragma once

#include <string>
#include <unordered_map>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object3D 
{	
	Mesh* CreateCube(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color);;
}
