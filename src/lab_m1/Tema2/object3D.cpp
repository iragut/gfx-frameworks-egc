#include "object3D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include "Tema2.h"

using namespace std;

Mesh* object3D::CreateCube(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
		VertexFormat(corner, color), // 0
		VertexFormat(corner + glm::vec3(length, 0, 0), color), // 1
		VertexFormat(corner + glm::vec3(length, length, 0), color), // 2
		VertexFormat(corner + glm::vec3(0, length, 0), color), // 3
		VertexFormat(corner + glm::vec3(0, 0, length), color), // 4
		VertexFormat(corner + glm::vec3(length, 0, length), color), // 5
		VertexFormat(corner + glm::vec3(length, length, length), color), // 6
		VertexFormat(corner + glm::vec3(0, length, length), color) // 7

    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        0, 1, 5, 0, 5, 4,
        3, 2, 6, 3, 6, 7,
        0, 3, 7, 0, 7, 4,
        1, 2, 6, 1, 6, 5
    };
    
	Mesh* square = CreateMesh(name.c_str(), vertices, indices);
    return square;
}