#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/camera1.h"
#include "lab_m1/Tema2/transform3D1.h"
#include "components/simple_scene.h"
#include "irrKlang-64bit-1.6.0/include/irrKlang.h"

#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <chrono>

using namespace std;
using namespace m1;
using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();
ISoundEngine* background = createIrrKlangDevice();
ISoundEngine* pick = createIrrKlangDevice();
ISoundEngine* deliver = createIrrKlangDevice();


int get_random(int limit_start, int limit_end) {
	return rand() % (limit_end - limit_start + 1) + limit_start;
}

glm::vec3 Tema2::get_random_position() {
	return glm::vec3(get_random(2, 100), 0.5, get_random(2, 100));
}

bool Tema2::sphere_check_col(glm::vec3 position, float radius) {
	for (int i = 0; i < objects.size(); i++) {
		if (objects[i].type == 1) {
			glm::vec3 obj_position = objects[i].modelMatrix * glm::vec4(0, 0, 0, 1);
			float distance = sqrt(pow(obj_position.x - position.x, 2) + 
				pow(obj_position.y - position.y, 2) + 
				pow(obj_position.z - position.z, 2));
			if (distance < radius + objects[i].radius) {
				return true;
			}
		}
	}
	return false;
}

void Tema2::generate_objects(int numar) {
	for (int i = 0; i < numar; i++) {
		object obj, crown;
		obj.mesh = meshes["tulp"];
		obj.color = glm::vec3(0.07f, 0.34f, 0);
		obj.type = 0;
		crown.mesh = meshes["coroana"];
		crown.color = glm::vec3(0, 1, 0);
		crown.type = 1;

		while (true) {
			obj.modelMatrix = glm::mat4(1);
			glm::vec3 position = get_random_position();
			obj.modelMatrix *= transform3D::Translate(position.x, position.y, position.z);
			int scale = get_random(4, 17);

			obj.modelMatrix *= transform3D::Scale(1, scale, 1);	
			obj.collision.min = position - glm::vec3(0, 0.5, 0);
			obj.collision.max = position + glm::vec3(1, 0.5 + scale / 2, 2);

			if (!CheckCollision(obj.collision)) {
				crown.modelMatrix = glm::mat4(1);
				crown.modelMatrix *= transform3D::Translate(position.x, position.y + 0.5 + scale / 2, position.z);
				crown.modelMatrix *= transform3D::Scale(6, 6, 6);
				crown.radius = 3;
				position.y += 0.5 + scale / 2;
				if (!sphere_check_col(position, crown.radius)) {
					break;
				}
			}
		}
		objects.push_back(obj);
		objects.push_back(crown);
	}
}

void Tema2::generate_packet() {
	object packet, logo;
	packet.mesh = meshes["tulp"];
	packet.color = glm::vec3(1, 0, 0);
	packet.type = 3;

	while (true) {
		packet.modelMatrix = glm::mat4(1);
		glm::vec3 position = get_random_position();
		packet.modelMatrix *= transform3D::Translate(position.x, position.y, position.z);
		packet.modelMatrix *= transform3D::Scale(1, 1, 1.5);
		packet.collision.min = position - glm::vec3(0, 0.5, 0);
		packet.collision.max = position + glm::vec3(1, 0.5, 2);
		pack_position = position;
		if (!CheckCollision(packet.collision)) {
			break;
		}
	}
	objects.push_back(packet);

	logo.mesh = meshes["tazz"];
	logo.color = glm::vec3(1, 1, 1);
	logo.type = 3;
	logo.modelMatrix = glm::mat4(1);
	logo.modelMatrix *= transform3D::Translate(pack_position.x + 0.7, pack_position.y - 0.1, pack_position.z - 0.4);
	logo.modelMatrix *= transform3D::Scale(0.5, 0.5, 0.5);
	logo.collision.min = glm::vec3(0, 0, 0);
	logo.collision.max = glm::vec3(0, 0, 0);
	objects.push_back(logo);

	deliver_packet = packet;
}

void Tema2::generate_deliver_point() {
	object deliver;
	deliver.mesh = meshes["point"];
	deliver.color = glm::vec3(0, 0, 1);
	deliver.type = 4;
	while (true) {
		deliver.modelMatrix = glm::mat4(1);
		glm::vec3 position = get_random_position();
		deliver.modelMatrix *= transform3D::Translate(position.x, position.y, position.z);
		deliver.modelMatrix *= transform3D::Scale(1.5, 1.5, 1.5);
		deliver.collision.min = position - glm::vec3(0, 0.1, 0);
		deliver.collision.max = position + glm::vec3(1 * 1.5, 0.1 , 2 * 1.5);

		deliver_point_position = position;
		if (!CheckCollision(deliver.collision)) {
			break;
		}
	}
	objects.push_back(deliver);
	deliver_point = deliver;
}

Mesh* Tema2::CreateCube(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color)
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

Mesh* Tema2::CreateParalepid(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color)
{
	glm::vec3 corner = leftBottomCorner;
	std::vector<VertexFormat> vertices =
	{
        VertexFormat(corner, color),                                    
        VertexFormat(corner + glm::vec3(length, 0, -0.2), color),       
        VertexFormat(corner + glm::vec3(length, 0, length), color),     
        VertexFormat(corner + glm::vec3(0, 0, length + 0.2), color),          
	};
	std::vector<unsigned int> indices = {
		0, 1, 2, 0, 2, 3,
	};
	return CreateMesh(name.c_str(), vertices, indices);
}

Mesh* Tema2::CreateTerrain(const std::string& name, glm::vec3 leftBottomCorner, int m, int n, float length, glm::vec3 color)
{
	glm::vec3 corner = leftBottomCorner;
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	int i, j;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			vertices.push_back(VertexFormat(corner + glm::vec3(i * length, 0, j * length), color));
		}
	}
	for (i = 0; i < m - 1; i++) {
		for (j = 0; j < n - 1; j++) {
			indices.push_back(i * n + j);
			indices.push_back(i * n + j + 1);
			indices.push_back((i + 1) * n + j + 1);
			indices.push_back(i * n + j);
			indices.push_back((i + 1) * n + j + 1);
			indices.push_back((i + 1) * n + j);
		}
	}
	return CreateMesh(name.c_str(), vertices, indices);
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}

void Tema2::RenderMeshColor(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, implemented::Camera1* camera)
{
	if (!mesh || !shader || !shader->program)
		return;
	glUseProgram(shader->program);

	glUniform3fv(glGetUniformLocation(shader->program, "color"), 1, glm::value_ptr(color));

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	if (camera == minimap_camera) {
		glm::mat4 projectionMatrix = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.01f, 100.0f);
		int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
		glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	}
	else {
		glm::mat4 projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
		int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
		glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	}

	if (color == glm::vec3(-1, -1, -1)) {
		glUniform1i(glGetUniformLocation(shader->program, "new_color"), 0);
	}
	else {
		glUniform1i(glGetUniformLocation(shader->program, "new_color"), 1);
		glUniform3fv(glGetUniformLocation(shader->program, "color"), 1, glm::value_ptr(color));
	}
	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

bool intersection(struct collision_box a, struct collision_box b) {
	return a.min.x <= b.max.x &&
		a.max.x >= b.min.x &&
		a.min.y <= b.max.y &&
		a.max.y >= b.min.y &&
		a.min.z <= b.max.z &&
		a.max.z >= b.min.z;
}

bool sphere_intersection(glm::vec3 sphere, float radius, collision_box box) {
	float x = glm::clamp(sphere.x, box.min.x, box.max.x);
	float y = glm::clamp(sphere.y, box.min.y, box.max.y);
	float z = glm::clamp(sphere.z, box.min.z, box.max.z);

	float distanceSquared = (x - sphere.x) * (x - sphere.x) +
		(y - sphere.y) * (y - sphere.y) +
		(z - sphere.z) * (z - sphere.z);

	return distanceSquared < radius * radius;
}

glm::vec3 addvec3(glm::vec3 a, glm::vec3 b) {
	return glm::vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

void Tema2::RenderDrone(implemented::Camera1 *camera)
{   
	if (camera == minimap_camera) {
		modelMatrix = glm::mat4(1);
		modelMatrix *= transform3D::Translate(drone_position_x, drone_position_y, drone_position_z);
		modelMatrix *= transform3D::RotateOY(RADIANS(angle - 45));
		modelMatrix *= transform3D::Scale(1.5, 1.5, 1.5);
		RenderMeshColor(meshes["arrow"], shaders["TemaShader"], modelMatrix, glm::vec3(1, 1, 0), camera);
		return;
	}

	float x;
	float z;

	if (is_delivering) {
		x = deliver_point_position.x - drone_position_x;
		z = deliver_point_position.z - drone_position_z;
	}
	else {
		x = pack_position.x - drone_position_x;
		z = pack_position.z - drone_position_z;
	}


	float angle2 = atan2(x, z);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(drone_position_x, drone_position_y + 2, drone_position_z);
	modelMatrix *= transform3D::RotateOY(angle2);
	modelMatrix *= transform3D::RotateOY(RADIANS(180));
	modelMatrix *= transform3D::Scale(0.2, 0.1, 0.2);

	RenderMeshColor(meshes["arrow"], shaders["TemaShader"], modelMatrix, glm::vec3(1, 1, 0), camera);
	RenderMeshColor(meshes["arrow_w"], shaders["TemaShader"], modelMatrix, glm::vec3(0, 0, 0), camera);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(drone_position_x, drone_position_y, drone_position_z);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle));
	modelMatrix *= transform3D::Scale(2, 0.2, 0.2);
	modelMatrix *= transform3D::Translate(-0.5, 0 ,-0.5);
	RenderMeshColor(meshes["body"], shaders["TemaShader"], modelMatrix, glm::vec3(-1, -1, -1), camera);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(drone_position_x, drone_position_y, drone_position_z);
	modelMatrix *= transform3D::RotateOY(RADIANS(90 + angle));
	modelMatrix *= transform3D::Scale(2, 0.2, 0.2);
	modelMatrix *= transform3D::Translate(-0.5, 0, -0.5);
	RenderMeshColor(meshes["body"], shaders["TemaShader"], modelMatrix, glm::vec3(-1, -1, -1), camera);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(drone_position_x, 0.1 + drone_position_y, drone_position_z);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle));
	modelMatrix *= transform3D::Translate(0.8, 0, -0.1);
	RenderMeshColor(meshes["body_v2"], shaders["TemaShader"], modelMatrix, glm::vec3(-1, -1, -1), camera);

	modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(drone_position_x, 0.1 + drone_position_y,drone_position_z);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle));
	modelMatrix *= transform3D::Translate(-1, 0, -0.1);
	RenderMeshColor(meshes["body_v2"], shaders["TemaShader"], modelMatrix, glm::vec3(-1, -1, -1), camera);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(drone_position_x, 0.1 + drone_position_y, drone_position_z);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle));
	modelMatrix *= transform3D::Translate(-0.1, 0, 0.8);
	RenderMeshColor(meshes["body_v2"], shaders["TemaShader"], modelMatrix, glm::vec3(-1, -1, -1), camera);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(drone_position_x, 0.1 + drone_position_y, drone_position_z);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle));
	modelMatrix *= transform3D::Translate(-0.1, 0, -1);
	RenderMeshColor(meshes["body_v2"], shaders["TemaShader"], modelMatrix, glm::vec3(-1, -1, -1), camera);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(drone_position_x, 0.35 + drone_position_y, drone_position_z);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle));
	modelMatrix *= transform3D::Translate(0, 0, 0.9);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle_prop));
	modelMatrix *= transform3D::Translate(-0.05, 0, -0.15);
    modelMatrix *= transform3D::Scale(0.2, 0.5, 0.6);
	RenderMeshColor(meshes["propeller"], shaders["TemaShader"], modelMatrix, glm::vec3(-1, -1, -1), camera);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(drone_position_x, 0.35 + drone_position_y, drone_position_z);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle));
	modelMatrix *= transform3D::Translate(0, 0, -0.9);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle_prop));
	modelMatrix *= transform3D::Translate(-0.05, 0, -0.15);
	modelMatrix *= transform3D::Scale(0.2, 0.5, 0.6);
	RenderMeshColor(meshes["propeller"], shaders["TemaShader"], modelMatrix, glm::vec3(-1, -1, -1), camera);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(drone_position_x, 0.35 + drone_position_y, drone_position_z);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle));
	modelMatrix *= transform3D::Translate(0.9, 0, 0);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle_prop));
	modelMatrix *= transform3D::Translate(-0.05, 0, -0.15);
	modelMatrix *= transform3D::Scale(0.2, 0.5, 0.6);
	RenderMeshColor(meshes["propeller"], shaders["TemaShader"], modelMatrix, glm::vec3(-1, -1, -1), camera);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(drone_position_x, 0.35 + drone_position_y, drone_position_z);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle));
	modelMatrix *= transform3D::Translate(-0.9, 0, 0);
	modelMatrix *= transform3D::RotateOY(RADIANS(angle_prop));
	modelMatrix *= transform3D::Translate(-0.05, 0, -0.15);
	modelMatrix *= transform3D::Scale(0.2, 0.5, 0.6);
	RenderMeshColor(meshes["propeller"], shaders["TemaShader"], modelMatrix, glm::vec3(-1, -1, -1), camera);


	drone_colision.min = glm::vec3(drone_position_x - 0.3, drone_position_y, drone_position_z + 0.7);
	drone_colision.max = glm::vec3(drone_position_x + 1.3, drone_position_y + 0.3, drone_position_z + 1.3);

}

void Tema2::generate_clouds() {
	object cloud;
	cloud.mesh = meshes["cloud"];
	cloud.color = glm::vec3(1, 1, 1);
	cloud.type = 2;
	for (int i = 0; i < 20; i++) {
		cloud.modelMatrix = glm::mat4(1);
		glm::vec3 position = get_random_position();
		cloud.modelMatrix *= transform3D::Translate(position.x, 20, position.z);
		clouds.push_back(cloud);
	}
}

void Tema2::RenderObjects(implemented::Camera1* camera)
{
	for (auto& object : objects) {
		char* name = (char*)object.mesh->GetMeshID();
		if (strcmp(name, "terrain") == 0) {
			RenderMeshColor(object.mesh, shaders["TerrainShader"], object.modelMatrix, object.color, camera);
		}
		else if ((object.type == 3 || object.type == 4) && camera == minimap_camera  ) {
			object.modelMatrix *= transform3D::Scale(5, 5, 5);
			RenderMeshColor(object.mesh, shaders["TemaShader"], object.modelMatrix, object.color, camera);
			object.modelMatrix *= transform3D::Scale(1.0f / 5, 1.0f / 5, 1.0f / 5);
		}
		else if (object.type == 3 && is_delivering) {
			object.modelMatrix = glm::mat4(1);

			if (strcmp(name, "tazz") == 0) {
				object.modelMatrix *= transform3D::Translate(pack_position.x + 0.7, pack_position.y - 0.1, pack_position.z - 0.4);
				object.modelMatrix *= transform3D::Scale(0.5, 0.5, 0.5);
			} else {
				object.modelMatrix *= transform3D::Translate(drone_position_x, drone_position_y - 1, drone_position_z);
				object.modelMatrix *= transform3D::Scale(1, 1, 1.5);
				pack_position = object.modelMatrix * glm::vec4(0, 0, 0, 1);
			}	
			RenderMeshColor(object.mesh, shaders["TemaShader"], object.modelMatrix, object.color, camera);
		}
		else {
			RenderMeshColor(object.mesh, shaders["TemaShader"], object.modelMatrix, object.color, camera);
		}
	}
}

bool Tema2::CheckCollision(struct collision_box a) {
	
	for (int i = 0; i < objects.size(); i++) {
		if (objects[i].type == 0 || objects[i].type == 3 || objects[i].type == 4) {
			if (intersection(a, objects[i].collision)) {
				return true;
			}
		}
		else if (objects[i].type == 1) {
			glm::vec3 pos = objects[i].modelMatrix * glm::vec4(0, 0, 0, 1);
			pos.x += 0.5;
			pos.z += 1;
			if (sphere_intersection(pos, objects[i].radius, a)) {
				return true;
			}
		}
	}
	return false;
}

void Tema2::Init()
{
	{
		Shader* shader = new Shader("TemaShader");
		shader->AddShader("src/lab_m1/Tema2/shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("src/lab_m1/Tema2/shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("TerrainShader");
		shader->AddShader("src/lab_m1/Tema2/shaders/VertexShaderNoise.glsl", GL_VERTEX_SHADER);
		shader->AddShader("src/lab_m1/Tema2/shaders/FragmentShaderNoise.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	fov = 70;
    camera = new implemented::Camera1();
    camera->Set(glm::vec3(0, 3, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);

	glm::ivec2 resolution = window->GetResolution();
	minimap = ViewportArea(50, 50, 250, 250);

	minimap_camera = new implemented::Camera1();
	minimap_camera->Set(glm::vec3(0, 35, 0.000001), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	minimap_camera->MoveForward(-50);
	minimap_camera->MoveRight(50);



	Mesh* box = CreateCube("body", glm::vec3(0, 0, 0), 1, glm::vec3(1, 1, 1));
	Mesh* box2 = CreateCube("body_v2", glm::vec3(0, 0, 0), 0.2, glm::vec3(1, 1, 1));
	Mesh* propeller = CreateParalepid("propeller", glm::vec3(0, 0, 0), 0.5, glm::vec3(1, 0, 0));
	Mesh* terrain = CreateTerrain("terrain", glm::vec3(0, 0, 0), 100, 100, 1, glm::vec3(0, 0.88f, 0.67f));

	Mesh* tulp = new Mesh("tulp");
	tulp->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
	meshes[tulp->GetMeshID()] = tulp;

	Mesh* coroana = new Mesh("coroana");
	coroana->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
	meshes[coroana->GetMeshID()] = coroana;

	Mesh* arrow = new Mesh("arrow");
	arrow->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "blender"), "arrow.obj");
	meshes[arrow->GetMeshID()] = arrow;

	Mesh* arrow_w = new Mesh("arrow_w");
	arrow_w->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "blender"), "arrow_w.obj");
	meshes[arrow_w->GetMeshID()] = arrow_w;

	Mesh* point = new Mesh("point");
	point->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "blender"), "point.obj");
	meshes[point->GetMeshID()] = point;

	Mesh* tazz = new Mesh("tazz");
	tazz->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "blender"), "tazz.obj");
	meshes[tazz->GetMeshID()] = tazz;

	Mesh* cloud = new Mesh("cloud");
	cloud->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "blender"), "cloud.obj");
	meshes[cloud->GetMeshID()] = cloud;

	drone_colision = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) };
	drone_position_x = 0;
	drone_position_y = 1;
	drone_position_z = 0;
	angle = 45;
	angle_prop = 0;
	score = 0;

	generate_objects(NUM_OF_OBJECTS);
	generate_packet();
	generate_clouds();

	modelMatrix = glm::mat4(1);
	terrain_collision = { glm::vec3(0, -0.1, 0), glm::vec3(100, 0.1, 100)};
	objects.push_back({ meshes["terrain"], modelMatrix, terrain_collision, glm::vec3(-1, -1, -1), 0});

	is_delivering = false;

	couldown = 0;
	nitro_active = true;

	background->play2D("assets/music/background.wav", GL_TRUE);

	start = false;

}


void Tema2::FrameStart()
{
    glClearColor(0.08f, 0.87f, 0.95f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}


void Tema2::Update(float deltaTimeSeconds)
{   
	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);

	RenderDrone(camera);
	RenderObjects(camera);

	for (int i = 0; i < clouds.size(); i++) {
		glm::vec3 position = clouds[i].modelMatrix * glm::vec4(0, 0, 0, 1);
		position.x += 1 * deltaTimeSeconds;
		if (position.x > 100) {
			position.x = 0;
		}
		clouds[i].modelMatrix = glm::mat4(1);
		clouds[i].modelMatrix *= transform3D::Translate(position.x, position.y, position.z);
		RenderMeshColor(clouds[i].mesh, shaders["TemaShader"], clouds[i].modelMatrix, clouds[i].color, camera);
	}


	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(minimap.x, minimap.y, minimap.width, minimap.height);

	RenderDrone(minimap_camera);
	RenderObjects(minimap_camera);
	if (angle_prop > 360) {
		angle_prop = 0;
	}
    else {
		angle_prop += deltaTimeSeconds * 1000;
    }
	if (couldown > 0.0f) {
		couldown -= deltaTimeSeconds;
		if (couldown <= 2.0f && nitro_active == false) {
			nitro_active = true;
			cout << "Nitro is ready" << endl;
		}
	}

}


void Tema2::FrameEnd()
{
	fov = 70;
}



void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float speed = 7.0f;
    glm::vec3 localForward = { cos(RADIANS(angle + 45)), 0, -sin(RADIANS(angle + 45)) };
	glm::vec3 localRight = { sin(RADIANS(angle + 45)), 0, cos(RADIANS(angle + 45)) };
    glm::vec3 direction;
		
	if (couldown <= 5.0f && nitro_active == true) {
		if (window->KeyHold(GLFW_KEY_LEFT_SHIFT)) {
			speed = 20.0f;
			fov = 130;
			couldown += deltaTime * 3;	
			
			if (start == false) {
				start = true;
				SoundEngine->play2D("assets/music/elice.wav", GL_TRUE);
				SoundEngine->setSoundVolume(0.2f);
			}
		}
	}
	else {
		nitro_active = false;
	}


	if (window->KeyHold(GLFW_KEY_W)) {
        direction = localForward * speed * deltaTime;
		drone_colision.min = addvec3(drone_colision.min, direction);
		drone_colision.max = addvec3(drone_colision.max, direction);

		if (CheckCollision(drone_colision) == false) {
			drone_position_x += direction.x;
			drone_position_z += direction.z;
			camera->MoveForward(speed * deltaTime);
			drone_colision.min = addvec3(drone_colision.min, -direction);
			drone_colision.max = addvec3(drone_colision.max, -direction);
		}
		drone_colision.min = addvec3(drone_colision.min, -direction);
		drone_colision.max = addvec3(drone_colision.max, -direction);
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        direction = -localRight * speed * deltaTime;
		drone_colision.min = addvec3(drone_colision.min, direction);
		drone_colision.max = addvec3(drone_colision.max, direction);

		if (CheckCollision(drone_colision) == false) {
			drone_position_x += direction.x;
			drone_position_z += direction.z;
			camera->TranslateRight(-speed * deltaTime);
			drone_colision.min = addvec3(drone_colision.min, -direction);
			drone_colision.max = addvec3(drone_colision.max, -direction);
		}
		drone_colision.min = addvec3(drone_colision.min, -direction);
		drone_colision.max = addvec3(drone_colision.max, -direction);

    }

    if (window->KeyHold(GLFW_KEY_S)) {
        direction = -localForward * speed * deltaTime;
		drone_colision.min = addvec3(drone_colision.min, direction);
		drone_colision.max = addvec3(drone_colision.max, direction);

		if (CheckCollision(drone_colision) == false) {
			drone_position_x += direction.x;
			drone_position_z += direction.z;
			camera->MoveForward(-speed * deltaTime);
			drone_colision.min = addvec3(drone_colision.min, -direction);
			drone_colision.max = addvec3(drone_colision.max, -direction);
		}
		drone_colision.min = addvec3(drone_colision.min, -direction);
		drone_colision.max = addvec3(drone_colision.max, -direction);

    }

    if (window->KeyHold(GLFW_KEY_D)) {
        direction = localRight * speed * deltaTime;
		drone_colision.min = addvec3(drone_colision.min, direction);
		drone_colision.max = addvec3(drone_colision.max, direction);

		if (CheckCollision(drone_colision) == false) {
			drone_position_x += direction.x;
			drone_position_z += direction.z;
			camera->TranslateRight(speed * deltaTime);
			drone_colision.min = addvec3(drone_colision.min, -direction);
			drone_colision.max = addvec3(drone_colision.max, -direction);
		} 

		drone_colision.min = addvec3(drone_colision.min, -direction);
		drone_colision.max = addvec3(drone_colision.max, -direction);

    }

    if (window->KeyHold(GLFW_KEY_Q)) {
		drone_colision.min.y += speed * deltaTime;
		drone_colision.max.y += speed * deltaTime;

		if (CheckCollision(drone_colision) == false) {
			drone_position_y += speed * deltaTime;
			camera->MoveUpward(-speed * deltaTime);
			//minimap_camera->MoveUpward(-speed * deltaTime);
			drone_colision.min.y -= speed * deltaTime;
			drone_colision.max.y -= speed * deltaTime;
		}
		drone_colision.min.y -= speed * deltaTime;
		drone_colision.max.y -= speed * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_E)) {
		drone_colision.min.y -= speed * deltaTime;
		drone_colision.max.y -= speed * deltaTime;

		if (CheckCollision(drone_colision) == false) {
			drone_position_y -= speed * deltaTime;
			camera->MoveUpward(speed * deltaTime);
			//minimap_camera->MoveUpward(speed * deltaTime);
			drone_colision.min.y += speed * deltaTime;
			drone_colision.max.y += speed * deltaTime;
		}
		drone_colision.min.y += speed * deltaTime;
		drone_colision.max.y += speed * deltaTime;
    }

	if (window->KeyHold(GLFW_KEY_LEFT)) {
		angle += deltaTime * 100;
		camera->RotateThirdPerson_OY(RADIANS(deltaTime * 100));
		camera->MoveRight(deltaTime * 3);	}
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        angle -= deltaTime * 100;
		camera->RotateThirdPerson_OY(RADIANS(-deltaTime * 100));
		camera->MoveRight(-deltaTime * 3);
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_V) {
		collision_box pack = deliver_packet.collision;
		collision_box d_point = deliver_point.collision;
		pack.max.y += 1;
		d_point.max.y += 3;
		object obj = objects[objects.size() - 1];
		objects.erase(objects.end() - 1);
		if (intersection(drone_colision, pack) && is_delivering == false) {
			generate_deliver_point();
			is_delivering = true;
			pick->play2D("assets/music/take.mp3", GL_FALSE);
			pick->setSoundVolume(1.f);
		}
		else if (intersection(drone_colision, d_point) && is_delivering == true) {
			is_delivering = false;
			objects.erase(objects.end() - 1);
			objects.erase(objects.end() - 1);
			objects.erase(objects.end() - 1);
			generate_packet();
			score++;
			cout << "Score: " << score << endl;
			deliver->play2D("assets/music/del.mp3", GL_FALSE);
		}
		objects.push_back(obj);
	}
}


void Tema2::OnKeyRelease(int key, int mods)
{
	if (key == GLFW_KEY_LEFT_SHIFT) {
		SoundEngine->stopAllSounds();
		start = false;
	}
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{

}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{

}
