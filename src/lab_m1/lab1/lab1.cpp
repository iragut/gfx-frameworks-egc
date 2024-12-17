﻿#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab1::Lab1()
{
    // TODO(student): Never forget to initialize class variables!
	colorIndex = 0;
	meshIndex = 0;
	X = 0;
	Y = 0;
	Z = 0;
    pos = 0;
}


Lab1::~Lab1()
{
}


void Lab1::Init()
{
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
  //  {
  //      Mesh* mesh = new Mesh("box");
  //      mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
  //      meshes[mesh->GetMeshID()] = mesh;
  //  }
  //  
  //  {
		//Mesh* mesh = new Mesh("sphere");
		//mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
		//meshes[mesh->GetMeshID()] = mesh;
  //  }

  //  {
		//Mesh* mesh = new Mesh("teapot");
		//mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
		//meshes[mesh->GetMeshID()] = mesh;
  //  }

    Mesh* new_mesh = new Mesh("play");
    new_mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "images"), "play.fbx");
    meshes[new_mesh->GetMeshID()] = new_mesh;

    // TODO(student): Load some more meshes. The value of RESOURCE_PATH::MODELS
    // is actually a path on disk, go there and you will find more meshes.

}


void Lab1::FrameStart()
{
}


void Lab1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffer


    // TODO(student): Generalize the arguments of `glClearColor`.
    // You can, for example, declare three variables in the class header,
    // that will store the color components (red, green, blue).
	glClearColor(colorIndex, colorIndex, colorIndex, 1);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    // Render the object
    RenderMesh(meshes["box"], glm::vec3(X, Y, Z), glm::vec3(0.5f));
   
    // Render the object again but with different properties
    RenderMesh(meshes["box"], glm::vec3(-1, 0.5f, 0));



    float radius = 1.0f;    // Radius of the Moon's orbit
    float speed = 1.0f;     // Speed of the Moon's movement
    static float angle = 0.0f;   // Angle in radians, needs to persist across frames

    // Update the angle over time (increase the angle gradually to make the Moon orbit)
    angle += speed * deltaTimeSeconds;  // Ensure deltaTime is a valid time per frame (in seconds)

    // Keep the angle within 0 to 2π for cleaner values (though not necessary)
    if (angle > 2 * M_PI) {
        angle -= 2 * M_PI;
    }

    // Earth
    RenderMesh(meshes["sphere"], glm::vec3(0, 1, -3));

    // Moon's circular orbit calculation
    float moonX = radius * cos(angle);  // X position based on the angle
    float moonZ = radius * sin(angle);  // Z position based on the angle

    // Render the Moon orbiting around the Earth
    RenderMesh(meshes["sphere"], glm::vec3(moonX, 1, -3 + moonZ), glm::vec3(0.5f));



    if (meshIndex == 0) {
        RenderMesh(meshes["teapot"], glm::vec3(1, 0, 0), glm::vec3(0.5f));
    }
    else if (meshIndex == 1) {
        RenderMesh(meshes["box"], glm::vec3(1, 0, 0), glm::vec3(0.5f));
    }
    else if (meshIndex == 2) {
	    RenderMesh(meshes["sphere"], glm::vec3(1, 0, 0), glm::vec3(0.5f));
    }

	RenderMesh(meshes["play"], glm::vec3(0, 0, 0), glm::vec3(0.5f));

    // TODO(student): We need to render (a.k.a. draw) the mesh that
    // was previously loaded. We do this using `RenderMesh`. Check the
    // signature of this function to see the meaning of its parameters.
    // You can draw the same mesh any number of times.
    

}


void Lab1::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    // TODO(student): Add some key hold events that will let you move
    // a mesh instance on all three axes. You will also need to
    // generalize the position used by `RenderMesh`.

	//if (window->KeyHold(GLFW_KEY_W)) {
	//	Y += 1 * deltaTime;
	//}
	//else if (window->KeyHold(GLFW_KEY_S)) {
	//	Y -= 1 * deltaTime;
	//}
	//else if (window->KeyHold(GLFW_KEY_A)) {
	//	X -= 1 * deltaTime;
	//}
	//else if (window->KeyHold(GLFW_KEY_D)) {
	//	X += 1 * deltaTime;
	//}
	//else if (window->KeyHold(GLFW_KEY_Q)) {
	//	Z += 1 * deltaTime;
	//}
 //   else if (window->KeyHold(GLFW_KEY_E)) {
	//	Z -= 1 * deltaTime;
	//}
 //   else if (window->KeyHold(GLFW_KEY_R)) {
 //       X = 0;
 //       Y = 0;
 //       Z = 0;
 //   }
}


void Lab1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F) {
        // TODO(student): Change the values of the color components.
		if (colorIndex == 0) {
			colorIndex = 1;
		}
		else {
			colorIndex = 0;
		}   
    } else if (key == GLFW_KEY_G) {
		if (meshIndex == 2) {
			meshIndex = 0;
		}
		else {
			meshIndex++;
		}
    }

    // TODO(student): Add a key press event that will let you cycle
    // through at least two meshes, rendered at the same position.
    // You will also need to generalize the mesh name used by `RenderMesh`.

}


void Lab1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
