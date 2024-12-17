#include "lab_m1/Tema1/Tema1.h"
#include "lab_m1/Tema1/Object2D.h"
#include "lab_m1/Tema1/Transform2D.h"
//#include "irrKlang-64bit-1.6.0/include/irrKlang.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <random>

using namespace std;
using namespace m1;
//using namespace irrklang;
//
//ISoundEngine* SoundEngine = createIrrKlangDevice();
//ISoundEngine* bullet_sound = createIrrKlangDevice();
//ISoundEngine* explosion_sound = createIrrKlangDevice();
//ISound* exp_sound;

float terrainFunction(int x)
{	
	return 60 * sin(x * 0.008) + 80 * sin(x * 0.5 * 0.010) + 20 * 0.5 * sin(3 * x * 0.01) + 40 * sin(x * 0.01);
}

// Calculate the trajectory of the bullet
vector<point_positon> calculate_trajectory(float angle, float x, float y, float deltaTimeSeconds, int amplification)
{
	// Calculate the start angle of the trajectory
	vector<point_positon> trajectory;
	float vy = VELOCITY * sin(glm::radians(angle)) * amplification;
	float vx = VELOCITY * cos(glm::radians(angle)) * amplification;
	
	// Calculate the points for the trajectory until out of map
	while (y > 0) {
		point_positon point;
		point.x = x + vx * deltaTimeSeconds;
		point.y = y + vy * deltaTimeSeconds;
		vy -= GRAVITY * deltaTimeSeconds * amplification;
		y = point.y;
		x = point.x;
		trajectory.push_back(point);
	}

	return trajectory;
}

// Generate the height map
vector<point_positon> generate_terrain(int resolution_x, int resolution_y)
{
	vector<point_positon> height_map;

	for (float i = 0; i < resolution_x ; i += 1)
	{
		point_positon point;
		point.x = i;
		point.y = terrainFunction(i) ;
		height_map.push_back(point);
	}

	return height_map;
}

// Calculate the height of the terrain at a specific position
float height(float x, vector<point_positon>& height_map){
	struct point_positon A = height_map[(int)floor(x)];
	struct point_positon B = height_map[(int)ceil(x)];

	float t = (x - A.x) / (B.x - A.x);

	if (HEIGHT + A.y + (B.y - A.y) * t < 0) {
		return 0;
	}

	return HEIGHT + A.y + (B.y - A.y) * t;
}

// Calculate the angle of the tank
float angle_tank(float x, vector<point_positon>& height_map)
{
	struct point_positon A = height_map[(int)floor(x)];
	struct point_positon B = height_map[(int)ceil(x)];

	struct point_positon v;

	v.x = B.x - A.x;
	v.y = B.y - A.y;

	return (float)atan2(v.y, v.x);
	
}

// Calculate the interpolation between two points
float interpolate(float x, vector<point_positon>& height_map)
{
	struct point_positon point = height_map[(int)floor(x)];
	struct point_positon point2 = height_map[(int)ceil(x)];

	float t = (x - point.x) / (point2.x - point.x);

	return (1 - t) * point.y + t * point2.y;
}

void add_bullet(vector<bullet>& bullets, float tank_player1_gun_angle, glm::vec3 gun_position_1)
{
	bullet bullet1;
	struct point_positon point;
	float angle_x = cos(glm::radians(tank_player1_gun_angle));
	float angle_y = sin(glm::radians(tank_player1_gun_angle));

	point.x = angle_x * VELOCITY;
	point.y = angle_y * VELOCITY;
	bullet1.v = point;
	bullet1.position.x = gun_position_1.x + angle_x * 40;
	bullet1.position.y = gun_position_1.y + angle_y * 40;
	bullet1.radius = 5;
	bullet1.blast_radius = 60;
	bullets.push_back(bullet1);
}

// Calculate and destroy the terrain around the bullet
void collision_terrain(vector<point_positon>& height_map, float collision_radius, struct bullet bullet)
{
	struct point_positon point = bullet.position;
	for (int i = -bullet.blast_radius; i < bullet.blast_radius; i++) 
	{
		float index = point.x + i;
		float dx = index - point.x;

		// Check if the index is out of map
		if (index < 0 || index >= height_map.size()) {
			continue;
		}

		// Calculate the deformation and update the height
		float deformation = sqrt(bullet.blast_radius * bullet.blast_radius - dx * dx);
		float initial_height = height_map[index].y + HEIGHT;

		if (initial_height > bullet.position.y - deformation) {
			height_map[index].y = bullet.position.y - deformation - HEIGHT;
		}
	}
}

vector<point_positon> terrain_displasment(vector<point_positon>& height_map)
{
	int start, end;
	int count = 0;
	float new_height = 0;
	float total_heights = 0;
	float average_height = 0;
	std::vector<point_positon> height_map_smoothed = height_map;

	for (int i = 0; i < height_map.size() - 1; i++) {
		total_heights = 0; count = 0;

		// Calculate the limits of range
		if (i < SMOOTH_RADIUS) {
			start = 0;
			end = SMOOTH_RADIUS + i;
		}
		else if (i > height_map.size() - SMOOTH_RADIUS) {
			start = height_map.size() - SMOOTH_RADIUS;
			end = height_map.size();
		}
		else {
			start = i - SMOOTH_RADIUS;
			end = i + SMOOTH_RADIUS;
		}

		// Calculate the average height
		for (int j = start; j < end; j++) {
			total_heights += height_map[j].y;
			count++;
		}
		average_height = total_heights / count;

		// Update the new height
		new_height = height_map[i].y * 0.8f + average_height * 0.2f;
		if (abs(height_map[i].y - average_height) > 0.7) {
			height_map_smoothed[i].y = new_height;
		}
	}
	return height_map_smoothed;
}

// Tank 1 render
void Tema1::menu_tank_render(){
	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(310, 350);
	model_matrix *= transform2D::Scale(6, 3);
	RenderMesh2D(meshes["body"], shaders["VertexColor"], model_matrix);

	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(490, 360);
	model_matrix *= transform2D::Rotate(glm::radians(180.0f));
	model_matrix *= transform2D::Translate(17, 6);
	model_matrix *= transform2D::Scale(5, 3);
	RenderMesh2D(meshes["tracks"], shaders["VertexColor"], model_matrix);

	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(400, 380);
	model_matrix *= transform2D::Scale(3.6, 3.6);
	RenderMesh2D(meshes["turret"], shaders["VertexColor"], model_matrix);

	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(480, 450);
	model_matrix *= transform2D::Rotate(glm::radians(145.0f));
	model_matrix *= transform2D::Scale(3.6, 0.3);
	RenderMesh2D(meshes["gun"], shaders["VertexColor"], model_matrix);
}

// Tank 2 render
void Tema1::menu_tank2_render() {
	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(780, 350);
	model_matrix *= transform2D::Scale(8, 2);
	RenderMesh2D(meshes["body"], shaders["VertexColor"], model_matrix);

	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(1025, 360);
	model_matrix *= transform2D::Rotate(glm::radians(180.0f));
	model_matrix *= transform2D::Translate(17, 6);
	model_matrix *= transform2D::Scale(7.3, 3);
	RenderMesh2D(meshes["tracks"], shaders["VertexColor"], model_matrix);

	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(930, 350);
	model_matrix *= transform2D::Scale(2, 1);
	RenderMesh2D(meshes["body_tank2"], shaders["VertexColor"], model_matrix);

	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(810, 350);
	model_matrix *= transform2D::Rotate(glm::radians(-15.0f));
	model_matrix *= transform2D::Scale(2, 1.4);
	RenderMesh2D(meshes["body_tank2"], shaders["VertexColor"], model_matrix);

	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(810, 370);
	model_matrix *= transform2D::Rotate(glm::radians(-15.0f));
	model_matrix *= transform2D::Scale(7, 0.3);
	RenderMesh2D(meshes["gun"], shaders["VertexColor"], model_matrix);

	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(990, 415);
	model_matrix *= transform2D::Rotate(glm::radians(-15.0f));
	model_matrix *= transform2D::Scale(2, 0.6);
	RenderMesh2D(meshes["gun"], shaders["VertexColor"], model_matrix);
}

// Render tanks options
void Tema1::menu()
{	
	menu_tank_render();
	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(250, 250);
	RenderMesh2D(meshes["picture"], shaders["VertexColor"], model_matrix);

	menu_tank2_render();
	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Translate(750, 250);
	RenderMesh2D(meshes["picture"], shaders["VertexColor"], model_matrix);

	model_matrix = glm::mat3(1);
	model_matrix *= transform2D::Scale((float)resolution.x, (float)resolution.y);
	RenderMesh2D(meshes["menu"], shaders["VertexColor"], model_matrix);
}


void Tema1::Init()
{
    resolution = window->GetResolution();
	std::cout << "[Init] Resolution: " << resolution.x << " " << resolution.y << "\n";
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

	Mesh* background = object2D::CreateSquare("background", glm::vec3(0, 0, 0), 1, glm::vec3(0.08f, 0.87f, 0.95f), true);
	AddMeshToList(background);   
    
	Mesh* square = object2D::CreateSquare("square", glm::vec3(0, 0, 0), 30, glm::vec3(0, 1, 0), true);
	AddMeshToList(square);

	Mesh* trapezoid_1 = object2D::CreateTrapezoide("body", glm::vec3(0, 0, 0), 30, 10, 8, glm::vec3(0.8f , 0.68f, 0.53f), true);
	AddMeshToList(trapezoid_1);

	Mesh* trapezoid_2 = object2D::CreateTrapezoide("body_2", glm::vec3(0, 0, 0), 30, 10, 8, glm::vec3(1, 0 ,0), true);
	AddMeshToList(trapezoid_2);

	Mesh* trapezoid_3 = object2D::CreateTrapezoide("tracks", glm::vec3(0, 0, 0), 30, 10, 5, glm::vec3(0.45f, 0.39f, 0.30f), true);
	AddMeshToList(trapezoid_3);

	Mesh* circle = object2D::CreateCircle("turret", glm::vec3(0, 0, 0), 10, glm::vec3(0.8f, 0.68f, 0.53f));
	AddMeshToList(circle);

	Mesh* circle_2 = object2D::CreateCircle("turret_2", glm::vec3(0, 0, 0), 10, glm::vec3(1, 0, 0));
	AddMeshToList(circle_2);

	Mesh* square_2 = object2D::CreateSquare("gun", glm::vec3(0, 0, 0), 30, glm::vec3(0, 0, 0), true);
	AddMeshToList(square_2);

	Mesh* circle_3 = object2D::CreateCircle("bullet", glm::vec3(0, 0, 0), 5, glm::vec3(0, 0, 0));
	AddMeshToList(circle_3);

	Mesh* square_3 = object2D::CreateSquare("line", glm::vec3(0, 0, 0), 5, glm::vec3(1, 1, 1), true);
	AddMeshToList(square_3);

	Mesh* square_4 = object2D::CreateSquare("health_bar_frame", glm::vec3(0, 0, 0), 20, glm::vec3(1, 1, 1), false);
	AddMeshToList(square_4);

	Mesh* square_5 = object2D::CreateSquare("health_bar", glm::vec3(0, 0, 0), 18, glm::vec3(1, 1, 1), true);
	AddMeshToList(square_5);

	Mesh* circle_4 = object2D::CreateCircle("collision_1", glm::vec3(0, 0, 0), 30, glm::vec3(1, 1, 1));
	AddMeshToList(circle_4);

	Mesh* circle_5 = object2D::CreateCircle("collision_2", glm::vec3(0, 0, 0), 30, glm::vec3(1, 1, 1));
	AddMeshToList(circle_5);

	Mesh* menu = object2D::CreateSquare("menu", glm::vec3(0, 0, 0), 30, glm::vec3(1, 1, 0), true);
	AddMeshToList(menu);

	Mesh* square_6 = object2D::CreateSquare("picture", glm::vec3(0, 0, 0), 300, glm::vec3(0, 0, 1), true);
	AddMeshToList(square_6);

	Mesh* square_7 = object2D::CreateSquare("body_tank2", glm::vec3(0, 0, 0), 30, glm::vec3(0.8f, 0.68f, 0.53f), true);
	AddMeshToList(square_7);

	height_map = generate_terrain(resolution.x, resolution.y);
	collision_radius = 30;

	is_alive_player1 = true;
	health_player1 = 10;
	tank_player1_x = 60.5f;
	tank_player1_y = height_map[60].y + HEIGHT;	
	tank_player1_angle = 0;
	tank_player1_speed = 0;
	tank_player1_gun_angle = 90;
	
	is_alive_player2 = true;
	health_player2 = 10;
	tank_player2_x = 1200.5f;
	tank_player2_y = height_map[1200].y + HEIGHT;
	tank_player2_angle = 0;
	tank_player2_speed = 0;
	tank_player2_gun_angle = 90;

	smooth_radius = 10;

	in_menu = true;
	selected_tank = 1;
	tank2_x = 120.5f;
	tank2_y = height_map[120].y + HEIGHT;
	tank2_angle_gun = 15;

	cout << "Medium tank (Left)\n";
	cout << "Health: 10\n";
	cout << "Medium range\n";
	cout << "Medium speed\n";

	cout << "Artillery tank (Right)\n";
	cout << "Health: 10\n";
	cout << "Long range\n";
	cout << "Fast bullets\n";
	cout << "Can't move\n";

	//SoundEngine->play2D(BACKGROUND_MUSIC_PATH, true);
}

void Tema1::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{	
	// Render selected tank menu
	if (in_menu) {
		menu();
		return;
	}

	// Calculate tank position and angle
	{
		tank_player1_y = height(tank_player1_x, height_map);
		if (tank_player1_y == 0) {
			tank_player1_angle = 0;
		}
		else {
			tank_player1_angle = angle_tank(tank_player1_x, height_map);
		}

		tank_player2_y = height(tank_player2_x, height_map);
		if (tank_player2_y == 0) {
			tank_player2_angle = 0;
		}
		else {
			tank_player2_angle = angle_tank(tank_player2_x, height_map);
		}

		tank2_y = height(tank2_x, height_map);
		if (tank2_y == 0) {
			tank2_angle = 0;
		}
		else {
			tank2_angle = angle_tank(tank2_x, height_map);
		}
	}
	
	// Render bullets
	{
		// Player 1
		int amplification;
		if (is_alive_player1) {
			if (selected_tank == 1) {
				amplification = 1;
			}
			else {
				amplification = 3;
			}
			for (int i = 0; i < bullets.size(); i++)
			{
				// Check if bullet is out of bounds
				if (bullets[i].position.x < 10 || bullets[i].position.x > resolution.x - 10 || bullets[i].position.y < 0) {
					bullets.erase(bullets.begin() + i);
					continue;
				}
				float Iy = interpolate(bullets[i].position.x, height_map);
				float distance = sqrt(pow(collision_position2.x - bullets[i].position.x, 2) +
					pow(collision_position2.y - bullets[i].position.y, 2));
				// Check if bullet hit the other player
				if (distance < collision_radius + bullets[i].radius) {

					health_player2--;
					if (health_player2 == 0) {
						is_alive_player2 = false;
						collision_position2 = glm::vec3(0, 0, 0);
					}

					//exp_sound = explosion_sound->play2D(EXPLOSION_SOUND_PATH, false, false, true);
					//exp_sound->setVolume(0.3f);
					bullets.erase(bullets.begin() + i);
					continue;
				
				
				}
				// Check if bullet hit the terrain
				else if (bullets[i].position.y - Iy < HEIGHT) {
					collision_terrain(height_map, collision_radius, bullets[i]);
					//exp_sound = explosion_sound->play2D(EXPLOSION_SOUND_PATH, false, false, true);
					//exp_sound->setVolume(0.3f);
					bullets.erase(bullets.begin() + i);
					continue;
				}

				// Update bullet position otherwises for player 1
				bullets[i].position.x += bullets[i].v.x * deltaTimeSeconds * amplification;
				bullets[i].position.y += bullets[i].v.y * deltaTimeSeconds * amplification;

				bullets[i].v.y -= GRAVITY * deltaTimeSeconds;

				model_matrix = glm::mat3(1);
				model_matrix *= transform2D::Translate(bullets[i].position.x, bullets[i].position.y);
				RenderMesh2D(meshes["bullet"], shaders["VertexColor"], model_matrix);
			}
		}


		// Player 2
		if (is_alive_player2) {
			for (int i = 0; i < bullets2.size(); i++)
			{	
				// Check if bullet is out of bounds
				if (bullets2[i].position.x < 10 || bullets2[i].position.x > resolution.x - 10 || bullets2[i].position.y < 0) {
					bullets2.erase(bullets2.begin() + i);
					continue;
				}


				float Iy = interpolate(bullets2[i].position.x, height_map);
				float distance = sqrt(pow(collision_position.x - bullets2[i].position.x, 2) +
					pow(collision_position.y - bullets2[i].position.y, 2));
				// Check if bullet hit the other player
				if (distance < collision_radius + bullets2[i].radius) {

					health_player1--;
					if (health_player1 == 0) {
						is_alive_player1 = false;
						collision_position = glm::vec3(0, 0, 0);
					}
					//exp_sound = explosion_sound->play2D(EXPLOSION_SOUND_PATH, false, false, true);
					//exp_sound->setVolume(0.3f);
					bullets2.erase(bullets2.begin() + i);
					continue;

				}
				// Check if bullet hit the terrain
				else if (bullets2[i].position.y - Iy < HEIGHT) {
					collision_terrain(height_map, collision_radius, bullets2[i]);
					//exp_sound = explosion_sound->play2D(EXPLOSION_SOUND_PATH, false, false, true);
					//exp_sound->setVolume(0.3f);
					bullets2.erase(bullets2.begin() + i);
					continue;
				}

				// Update bullet position otherwises for player 2
				bullets2[i].position.x += bullets2[i].v.x * deltaTimeSeconds;
				bullets2[i].position.y += bullets2[i].v.y * deltaTimeSeconds;

				bullets2[i].v.y -= GRAVITY * deltaTimeSeconds;

				model_matrix = glm::mat3(1);
				model_matrix *= transform2D::Translate(bullets2[i].position.x, bullets2[i].position.y);
				RenderMesh2D(meshes["bullet"], shaders["VertexColor"], model_matrix);
			}
		}
	}

	// Render terrain
	{
		// Smooth terrain if needed
		height_map = terrain_displasment(height_map);

		for (int i = 0; i < height_map.size() - 1; i++) {
			point_positon point1 = height_map[i];
			point_positon point2 = height_map[i + 1];

			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(point1.x, point1.y + HEIGHT);
			model_matrix *= transform2D::Shear((point2.y - point1.y) / (point2.x - point1.x), 0);
			model_matrix *= transform2D::Scale((point2.x - point1.x) / 30.0f, -20.0f);

			RenderMesh2D(meshes["square"], shaders["VertexColor"], model_matrix);
		}
	}

	
	// Render tanks
	{
		// Render tank player1
		if (is_alive_player1 && selected_tank == 1)
		{
			// Health bar
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player1_x - 60, tank_player1_y + 60);
			model_matrix *= transform2D::Scale(5, 1);
			RenderMesh2D(meshes["health_bar_frame"], shaders["VertexColor"], model_matrix);

			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player1_x - 57, tank_player1_y + 63);
			model_matrix *= transform2D::Scale(health_player1 * 0.52, 0.8);
			RenderMesh2D(meshes["health_bar"], shaders["VertexColor"], model_matrix);

			// Tank body
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player1_x, tank_player1_y);
			model_matrix *= transform2D::Rotate(-tank_player1_angle);
			model_matrix *= transform2D::Translate(-50, 5);
			model_matrix *= transform2D::Scale(3, 1.5);
			RenderMesh2D(meshes["body"], shaders["VertexColor"], model_matrix);

			// Tank tracks
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player1_x, tank_player1_y);
			model_matrix *= transform2D::Rotate(-tank_player1_angle);
			model_matrix *= transform2D::Translate(17, 6);
			model_matrix *= transform2D::Rotate(glm::radians(180.0f));
			model_matrix *= transform2D::Translate(-15, 0);
			model_matrix *= transform2D::Scale(2.5, 1);
			RenderMesh2D(meshes["tracks"], shaders["VertexColor"], model_matrix);

			// Tank turret
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player1_x, tank_player1_y);
			model_matrix *= transform2D::Rotate(-tank_player1_angle);
			model_matrix *= transform2D::Translate(-5, 20);
			model_matrix *= transform2D::Scale(1.7, 1.7);
			RenderMesh2D(meshes["turret"], shaders["VertexColor"], model_matrix);

			// Tank gun
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player1_x, tank_player1_y);
			model_matrix *= transform2D::Rotate(-tank_player1_angle);
			model_matrix *= transform2D::Translate(-4, 23);
			model_matrix *= transform2D::Rotate(glm::radians(-tank_player1_gun_angle ) + tank_player1_angle);
			model_matrix *= transform2D::Scale(1.3f, 0.15f);
			RenderMesh2D(meshes["gun"], shaders["VertexColor"], model_matrix);

			gun_position_1 = model_matrix * glm::vec3(0, 0, 1);

			// Calculate trajectory and render it
			float pos_x = cos(glm::radians(tank_player1_gun_angle)) * 30 + gun_position_1.x;
			float pos_y = sin(glm::radians(tank_player1_gun_angle)) * 30 + gun_position_1.y;

			trajectory_player1 = calculate_trajectory(tank_player1_gun_angle, pos_x, pos_y, deltaTimeSeconds, 1);
			for (int i = 0; i < trajectory_player1.size(); i++) {
				model_matrix = glm::mat3(1);
				model_matrix *= transform2D::Translate(trajectory_player1[i].x, trajectory_player1[i].y);
				RenderMesh2D(meshes["line"], shaders["VertexColor"], model_matrix);
			}

		}
		else if (is_alive_player1 && selected_tank == 2) {
			// Health bar
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank2_x - 60, tank2_y + 60);
			model_matrix *= transform2D::Scale(5, 1);
			RenderMesh2D(meshes["health_bar_frame"], shaders["VertexColor"], model_matrix);

			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank2_x - 57, tank2_y + 63);
			model_matrix *= transform2D::Scale(health_player1 * 0.52, 0.8);
			RenderMesh2D(meshes["health_bar"], shaders["VertexColor"], model_matrix);
			
			// Tank body
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank2_x, tank2_y);
			model_matrix *= transform2D::Rotate(-tank2_angle);
			model_matrix *= transform2D::Translate(-60, 5);
			model_matrix *= transform2D::Scale(4.5, 1);
			RenderMesh2D(meshes["body"], shaders["VertexColor"], model_matrix);

			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank2_x, tank2_y);
			model_matrix *= transform2D::Rotate(-tank2_angle);
			model_matrix *= transform2D::Translate(17, 6);
			model_matrix *= transform2D::Rotate(glm::radians(180.0f));
			model_matrix *= transform2D::Translate(-50, 0);
			model_matrix *= transform2D::Scale(4, 1.5);
			RenderMesh2D(meshes["tracks"], shaders["VertexColor"], model_matrix);

			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank2_x, tank2_y);
			model_matrix *= transform2D::Rotate(-tank2_angle);
			model_matrix *= transform2D::Translate(-35, 5);
			model_matrix *= transform2D::Rotate(glm::radians(-15.0f));
			model_matrix *= transform2D::Scale(1.2, 0.7);
			RenderMesh2D(meshes["body_tank2"], shaders["VertexColor"], model_matrix);

			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank2_x, tank2_y);
			model_matrix *= transform2D::Rotate(-tank2_angle);
			model_matrix *= transform2D::Translate(30, 5);
			model_matrix *= transform2D::Scale(1, 0.5);
			RenderMesh2D(meshes["body_tank2"], shaders["VertexColor"], model_matrix);

			// Tank turret
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank2_x, tank2_y);
			model_matrix *= transform2D::Rotate(-tank2_angle);
			model_matrix *= transform2D::Translate(-5, 20);
			model_matrix *= transform2D::Rotate(glm::radians(-tank2_angle_gun) + tank2_angle);
			model_matrix *= transform2D::Scale(2.5, 0.15);
			RenderMesh2D(meshes["gun"], shaders["VertexColor"], model_matrix);

			gun_position_1 = model_matrix * glm::vec3(0, 0, 1);

			// Calculate trajectory and render it
			float pos_x = cos(glm::radians(tank2_angle_gun)) * 30 + gun_position_1.x;
			float pos_y = sin(glm::radians(tank2_angle_gun)) * 30 + gun_position_1.y;

			trajectory_player1 = calculate_trajectory(tank2_angle_gun, pos_x, pos_y, deltaTimeSeconds, 3);
			for (int i = 0; i < trajectory_player1.size(); i++) {
				model_matrix = glm::mat3(1);
				model_matrix *= transform2D::Translate(trajectory_player1[i].x, trajectory_player1[i].y);
				RenderMesh2D(meshes["line"], shaders["VertexColor"], model_matrix);
			}

		}


		// Render tank player2
		if (is_alive_player2)
		{
			// Health bar
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player2_x - 60, tank_player2_y + 60);
			model_matrix *= transform2D::Scale(5, 1);
			RenderMesh2D(meshes["health_bar_frame"], shaders["VertexColor"], model_matrix);

			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player2_x - 57, tank_player2_y + 63);
			model_matrix *= transform2D::Scale(health_player2 * 0.52, 0.8);
			RenderMesh2D(meshes["health_bar"], shaders["VertexColor"], model_matrix);


			// Tank body
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player2_x, tank_player2_y);
			model_matrix *= transform2D::Rotate(-tank_player2_angle);
			model_matrix *= transform2D::Translate(-50, 5);
			model_matrix *= transform2D::Scale(3, 1.5);
			RenderMesh2D(meshes["body_2"], shaders["VertexColor"], model_matrix);

			// Tank tracks
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player2_x, tank_player2_y);
			model_matrix *= transform2D::Rotate(-tank_player2_angle);
			model_matrix *= transform2D::Translate(17, 6);
			model_matrix *= transform2D::Rotate(glm::radians(180.0f));
			model_matrix *= transform2D::Translate(-15, 0);
			model_matrix *= transform2D::Scale(2.5, 1);
			RenderMesh2D(meshes["tracks"], shaders["VertexColor"], model_matrix);

			// Tank turret
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player2_x, tank_player2_y);
			model_matrix *= transform2D::Rotate(-tank_player2_angle);
			model_matrix *= transform2D::Translate(-5, 20);
			model_matrix *= transform2D::Scale(1.7, 1.7);
			RenderMesh2D(meshes["turret_2"], shaders["VertexColor"], model_matrix);

			// Tank gun
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player2_x, tank_player2_y);
			model_matrix *= transform2D::Rotate(-tank_player2_angle);
			model_matrix *= transform2D::Translate(-4, 23);
			model_matrix *= transform2D::Rotate(glm::radians(-tank_player2_gun_angle) + tank_player2_angle);
			model_matrix *= transform2D::Scale(1.3f, 0.15f);
			RenderMesh2D(meshes["gun"], shaders["VertexColor"], model_matrix);

			gun_position_2 = model_matrix * glm::vec3(0, 0, 1);


			// Calculate trajectory and render it
			float pos1_x = cos(glm::radians(tank_player2_gun_angle)) * 30 + gun_position_2.x;
			float pos1_y = sin(glm::radians(tank_player2_gun_angle)) * 30 + gun_position_2.y;
			trajectory_player2 = calculate_trajectory(tank_player2_gun_angle, pos1_x, pos1_y, deltaTimeSeconds, 1);
			for (int i = 0; i < trajectory_player2.size(); i++) {
				model_matrix = glm::mat3(1);
				model_matrix *= transform2D::Translate(trajectory_player2[i].x, trajectory_player2[i].y);
				RenderMesh2D(meshes["line"], shaders["VertexColor"], model_matrix);
			}
		}
	}

	// Background rendering
	{
        model_matrix = glm::mat3(1);
        model_matrix *= transform2D::Scale((float)resolution.x, (float)resolution.y);
        RenderMesh2D(meshes["background"], shaders["VertexColor"], model_matrix);
    }

	// Collision rendering
	{
		// Collision player 1

		if (is_alive_player1 && selected_tank == 1) {
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player1_x - 5, tank_player1_y + 10);
			model_matrix *= transform2D::Rotate(-tank_player1_angle);
			RenderMesh2D(meshes["collision_1"], shaders["VertexColor"], model_matrix);
			collision_position = model_matrix * glm::vec3(0, 0, 1);
		}
		else if (is_alive_player1 && selected_tank == 2) {
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank2_x - 5, tank2_y + 10);
			model_matrix *= transform2D::Rotate(-tank2_angle);
			RenderMesh2D(meshes["collision_1"], shaders["VertexColor"], model_matrix);
			collision_position = model_matrix * glm::vec3(0, 0, 1);
		}
		// Collision player 2
		if (is_alive_player2) {
			model_matrix = glm::mat3(1);
			model_matrix *= transform2D::Translate(tank_player2_x - 5, tank_player2_y + 10);
			model_matrix *= transform2D::Rotate(-tank_player2_angle);
			RenderMesh2D(meshes["collision_2"], shaders["VertexColor"], model_matrix);
			collision_position2 = model_matrix * glm::vec3(0, 0, 1);
		}
	}
}

void Tema1::FrameEnd()
{
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	// Player 1
	if (in_menu == false)
	{
		if (is_alive_player1)
		{
			if (1270 > tank_player1_x) {
				if (window->KeyHold(GLFW_KEY_D))
				{
					tank_player1_x += 100 * deltaTime;
				}
			}
			if (30 < tank_player1_x) {
				if (window->KeyHold(GLFW_KEY_A))
				{
					tank_player1_x -= 100 * deltaTime;
				}
			}

			if (selected_tank == 1) {
				if (window->KeyHold(GLFW_KEY_W))
				{
					tank_player1_gun_angle += 100 * deltaTime;
				}
				else if (window->KeyHold(GLFW_KEY_S))
				{
					tank_player1_gun_angle -= 100 * deltaTime;
				}
			}
			else if (selected_tank == 2) {
				if (window->KeyHold(GLFW_KEY_W))
				{
					tank2_angle_gun += 50 * deltaTime;
				}
				else if (window->KeyHold(GLFW_KEY_S))
				{
					tank2_angle_gun -= 50 * deltaTime;
				}
			}
		}
	}

	// Player 2
	if (in_menu == false)
	{
		if (is_alive_player2)
		{
			if (1270 > tank_player2_x) {
				if (window->KeyHold(GLFW_KEY_RIGHT))
				{
					tank_player2_x += 100 * deltaTime;
				}
			}
			if (30 < tank_player2_x) {
				if (window->KeyHold(GLFW_KEY_LEFT))
				{
					tank_player2_x -= 100 * deltaTime;
				}
			}

			if (window->KeyHold(GLFW_KEY_UP))
			{
				tank_player2_gun_angle += 100 * deltaTime;
			}
			else if (window->KeyHold(GLFW_KEY_DOWN))
			{
				tank_player2_gun_angle -= 100 * deltaTime;
			}
		}
	}
}

void Tema1::OnKeyPress(int key, int mods)
{
	if (is_alive_player1 && in_menu == false)
	{
		if (key == GLFW_KEY_SPACE && selected_tank == 1)
		{
			add_bullet(bullets, tank_player1_gun_angle, gun_position_1);
			//bullet_sound->play2D(SHOOT_SOUND_PATH, false);
		}
		else if (key == GLFW_KEY_SPACE && selected_tank == 2) {
			add_bullet(bullets, tank2_angle_gun, gun_position_1);
			//bullet_sound->play2D(SHOOT_SOUND_PATH, false);
		}
	}
	
	if (is_alive_player2 && in_menu == false)
	{
		if (key == GLFW_KEY_ENTER)
		{
			add_bullet(bullets2, tank_player2_gun_angle, gun_position_2);
			//bullet_sound->play2D(SHOOT_SOUND_PATH, false);
		}
	}

	if (key == GLFW_KEY_M)
	{
		in_menu = !in_menu;
	}
}

void Tema1::OnKeyRelease(int key, int mods)
{
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (in_menu) {
		if (mouseX > 250 && mouseX < 550 && mouseY > 150 && mouseY < 400) {
			in_menu = false;
			selected_tank = 1;
		}
		else if (mouseX > 750 && mouseX < 1050 && mouseY > 150 && mouseY < 400) {
			in_menu = false;
			selected_tank = 2;
		}
	}
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{	
	resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

