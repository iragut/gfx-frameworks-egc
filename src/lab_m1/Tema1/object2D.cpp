#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateCircle(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color
)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float angle = 2 * M_PI / 100;
    vertices.push_back(VertexFormat(center, color));


    for (int i = 0; i <= 100; i++) {  
        float currentAngle = i * angle;
        vertices.push_back(VertexFormat(center + glm::vec3(radius * cos(currentAngle), radius * sin(currentAngle), 0), color));
        indices.push_back(i + 1); 
    }

    for (int i = 1; i <= 100; i++) {
        indices.push_back(0);     
        indices.push_back(i);     
        indices.push_back(i + 1); 
    }

    Mesh* circle = new Mesh(name);
    circle->SetDrawMode(GL_TRIANGLES);
    circle->InitFromData(vertices, indices);
    return circle;
}


Mesh* object2D::CreateTrapezoide(
	const std::string& name,
	glm::vec3 leftBottomCorner,
	float length,
	float height,
	float angle,
	glm::vec3 color,
	bool fill)
{
	glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length - height + angle, height, 0), color),
        VertexFormat(corner + glm::vec3(height - angle , height, 0), color)
	};

	Mesh* trapezoide = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 3 };

	if (!fill) {
		trapezoide->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		indices.push_back(0);
		indices.push_back(2);
	}

	trapezoide->InitFromData(vertices, indices);
	return trapezoide;
}
