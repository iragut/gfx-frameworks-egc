#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/camera1.h"

#include <vector>

using namespace std;

namespace m1
{
    #define NUM_OF_OBJECTS 50
        
	struct collision_box {
		glm::vec3 min;
		glm::vec3 max;
	};

    struct object {
        Mesh* mesh;
        glm::mat4 modelMatrix;
        struct collision_box collision;
		glm::vec3 color;
		int type;
		float radius;
    };

    struct ViewportArea
    {
        ViewportArea() : x(0), y(0), width(1), height(1) {}
        ViewportArea(int x, int y, int width, int height)
            : x(x), y(y), width(width), height(height) {
        }
        int x;
        int y;
        int width;
        int height;
    };

    class Tema2 : public gfxc::SimpleScene
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
        
        Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
		Mesh* CreateCube(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color);
		Mesh* CreateParalepid(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color);
		Mesh* CreateTerrain(const std::string& name, glm::vec3 leftBottomCorner, int m, int n, float length, glm::vec3 color);

		void RenderMeshColor(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, implemented::Camera1* camera);
		void RenderDrone(implemented::Camera1* camera);
		void RenderObjects(implemented::Camera1* camera);
		void generate_objects(int numar);
		void generate_packet();
		void generate_deliver_point();
		void generate_clouds();

		glm::vec3 get_random_position();

		bool CheckCollision(struct collision_box a);
        bool sphere_check_col(glm::vec3 position, float radius);
            
     protected:
        implemented::Camera1 *camera;
        glm::mat4 projectionMatrix;

        int score;

		implemented::Camera1* minimap_camera;
		glm::mat4 minimap_projection;

		ViewportArea minimap;

		GLfloat fov;

		glm::mat4 modelMatrix;

		float drone_position_x;
        float drone_position_y;
        float drone_position_z;
		float angle;
		float angle_prop;

		struct collision_box drone_colision;
		struct collision_box terrain_collision;
		vector<object> objects;
		vector<object> clouds;
		vector<glm::vec3> obj_positions;

		glm::vec3 pack_position;
		object deliver_packet;

		glm::vec3 deliver_point_position;
		object deliver_point;

        bool is_delivering;

		float couldown;
		bool nitro_active;
        bool start;
    };
}
