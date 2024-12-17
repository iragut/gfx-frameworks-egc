#pragma once

#include "components/simple_scene.h"

#include <vector>


namespace m1
{	
	#define VELOCITY 300.f
	#define GRAVITY 240.f
    #define HEIGHT 250
    #define SMOOTH_RADIUS 10
	#define BACKGROUND_MUSIC_PATH "assets/music/background.mp3"
	#define SHOOT_SOUND_PATH "assets/music/shoot.wav"
    #define EXPLOSION_SOUND_PATH "assets/music/explosion.wav"

    struct point_positon
    {
        float x, y;
    };

	struct bullet
	{	
		point_positon position;
		point_positon v;
		float radius;
		float blast_radius;
	};

	class Tema1 : public gfxc::SimpleScene
	{
	public:
		void Init() override;

	private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;


        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

		void menu();
		void menu_tank_render();
		void menu_tank2_render();

	protected:
		glm::mat3 model_matrix;
        glm::ivec2 resolution;
           
		// Terrain
        std::vector<point_positon> height_map;
		float collision_radius;
		int smooth_radius;


		// Player 1
		int health_player1;
        float tank_player1_y;
		float tank_player1_x;
		float tank_player1_angle;
		float tank_player1_speed;
		float tank_player1_gun_angle;

		glm::vec3 gun_position_1;
		glm::vec3 collision_position;
		std::vector<point_positon> trajectory_player1;
		std::vector<bullet> bullets;

		bool is_alive_player1;

		// Player 2
		int health_player2;
		float tank_player2_y;
		float tank_player2_x;
		float tank_player2_angle;
		float tank_player2_speed;
		float tank_player2_gun_angle;
		glm::vec3 gun_position_2;
		glm::vec3 collision_position2;
		std::vector<point_positon> trajectory_player2;
		std::vector<bullet> bullets2;

		bool is_alive_player2;

		// Menu
		bool in_menu;
		int selected_tank;

		// Tank 2(Artilery Tank)
		float tank2_x;
		float tank2_y;
		float tank2_angle;
		float tank2_angle_gun;

	};

}