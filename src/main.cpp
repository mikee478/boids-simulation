#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <vector>

#include "renderer.h"
#include "vertex_buffer.h"
#include "shader.h"
#include "index_buffer.h"
#include "vertex_buffer_layout.h"
#include "vertex_array.h"
#include "renderer.h"
#include "input.h"
#include "flock.h"
#include "obstacle.h"
#include "point_obstacle.h"
#include "line_obstacle.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

int main(void)
{
    srand(time(NULL));

    // Initialize the GLFW library
    if (!glfwInit())
        return -1;

    // specify the client API version that the created context must be compatible with
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    /* 
    specifies whether the OpenGL context should be forward-compatible, i.e. one
    where all functionality deprecated in the requested version  of OpenGL is 
    removed. This must only be used if the requested OpenGL version is 3.0 or above.
    */
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a windowed mode window and its OpenGL context
    const int WINDOW_WIDTH = 900;
    const int WINDOW_HEIGHT = 600;
    const char* WINDOW_TITLE = "Boids";
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) // Error
    {
        std::cout << "Window create error" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the OpenGL context of the specified window current
        An OpenGL context represents many things. A context stores all of the state 
        associated with this instance of OpenGL. It represents the (potentially visible) 
        default framebuffer that rendering commands will draw to when not drawing to a 
        framebuffer object. Think of a context as an object that holds all of OpenGL; 
        when a context is destroyed, OpenGL is destroyed.
     */
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, Input::KeyCallback);
    glfwSetCursorPosCallback(window, Input::MousePosCallback);
    glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);

	// Min number of screen updates to wait for until the buffers are swapped by glfwSwapBuffers
    glfwSwapInterval(1);

    // Create OpenGL rendering context before calling glewInit
    // Initialize the extension entry points
    if(glewInit() != GLEW_OK)
        std::cout << "GLEW ERROR!" << std::endl;

    glEnable(GL_BLEND);  
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    Flock flock;
    float cohesion_weight = flock.GetCohesionWeight();
    float separation_weight = flock.GetSeparationWeight();
    float alignment_weight = flock.GetAlignmentWeight();
    float obst_avoid_weight = flock.GetObstacleAvoidanceWeight();

    auto mouse_obstacle = std::make_shared<PointObstacle>(0.0f, 0.0f, 50.0f);
    auto bot_wall_obst = std::make_shared<LineObstacle>(0.0f, 0.0f, WINDOW_WIDTH, 0.0f, 50.0f);
    auto top_wall_obst = std::make_shared<LineObstacle>(0.0f, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, 50.0f);
    auto mid_wall_obst = std::make_shared<LineObstacle>(WINDOW_WIDTH/3.0, WINDOW_HEIGHT/2.0, WINDOW_WIDTH*2.0/3.0, WINDOW_HEIGHT/2.0, 50.0f);

    std::vector<std::shared_ptr<Obstacle>> obstacles;
    obstacles.push_back(mouse_obstacle);
    obstacles.push_back(bot_wall_obst);
    obstacles.push_back(top_wall_obst);
    obstacles.push_back(mid_wall_obst);

    float prev_time = 0, cur_time, dt;
    bool paused = false;
    double mouse_x, mouse_y;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    while (!glfwWindowShouldClose(window) && !Input::EscapePressed())
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Configuration");
        
        ImGui::SliderFloat("Cohesion", &cohesion_weight, 0.0f, 20.0f);
        ImGui::SliderFloat("Separation", &separation_weight, 0.0f, 20.0f);
        ImGui::SliderFloat("Alignment", &alignment_weight, 0.0f, 20.0f);
        ImGui::SliderFloat("Obstacle Avoidance", &obst_avoid_weight, 0.0f, 20.0f);
        ImGui::Checkbox("Mouse Obstacle", &mouse_obstacle->enabled);
        ImGui::Checkbox("Paused", &paused);
        ImGui::Separator();
        ImGui::Text("%lu/%lu Boids", flock.GetSize(), Flock::MAX_BOIDS);
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        
        ImGui::End();

        flock.SetCohesionWeight(cohesion_weight);
        flock.SetSeparationWeight(separation_weight);
        flock.SetAlignmentWeight(alignment_weight);
        flock.SetObstacleAvoidanceWeight(obst_avoid_weight);

        cur_time = glfwGetTime();
        dt = cur_time - prev_time;
        prev_time = cur_time;

        if(!paused)
        {
            glfwGetCursorPos(window, &mouse_x, &mouse_y);

            if(Input::MouseButtonLeftPressed())
                flock.AddBoid({mouse_x, -mouse_y + WINDOW_HEIGHT});

            mouse_obstacle->SetCenter(mouse_x, -mouse_y + WINDOW_HEIGHT);

            flock.Update(obstacles, dt);
        }

        Clear();

        for(const auto &obst : obstacles)
            obst->Render();
        flock.Render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap front and back buffers
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Destroys all remaining windows and cursors and frees allocated resources
    glfwTerminate();
    return 0;
}