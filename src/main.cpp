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

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

int main(void)
{
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

    Flock flock;

    // Create and use shader program
    // *** Relative file paths are relative to current working directory ***
    Shader shader(
        "/Users/michael/Documents/projects/boids/res/shaders/vertex.shader", 
        "/Users/michael/Documents/projects/boids/res/shaders/geometry.shader", 
        "/Users/michael/Documents/projects/boids/res/shaders/fragment.shader");
    shader.Bind();

    glm::mat4 proj_mat = glm::ortho(
        0.0f, static_cast<float>(WINDOW_WIDTH), 
        0.0f, static_cast<float>(WINDOW_HEIGHT));

    shader.SetUniformMatrix4f("proj_mat", proj_mat);

    Renderer renderer;

    float prev_time = 0, cur_time, dt;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    while (!glfwWindowShouldClose(window) && !Input::EscapePressed())
    {
        glfwPollEvents();

        cur_time = glfwGetTime();
        dt = cur_time - prev_time;
        prev_time = cur_time;

        if(Input::MouseButtonLeftPressed())
        {
            int x, y;
            Input::GetMousePosition(x,y);
            flock.AddBoid({x,-y+WINDOW_HEIGHT});
        }

        flock.Update(dt);

        renderer.Clear();
        flock.Render();

        { // imgui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Debug Window");

            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

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