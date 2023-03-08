#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
#include <vector>
#include <fstream>

#include "renderer.h"
#include "vertex_buffer.h"
#include "shader.h"
#include "index_buffer.h"
#include "vertex_buffer_layout.h"
#include "vertex_array.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

struct Vertex
{
    float position[3];
    float normal[3];
    float color[3];
};

bool ImportObjFile(std::string filepath, std::vector<Vertex> &vertices, std::vector<unsigned int> &indices)
{
    std::ifstream infile(filepath);
    char c;
    float x,y,z;
    unsigned int i1,i2,i3;
    while(infile >> c)
    {
        if(c == 'v')
        {
            infile >> x >> y >> z;
            vertices.push_back({{x,y,z}, {0.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f}});
        }
        else if(c == 'f')
        {
            infile >> i1 >> i2 >> i3;
            i1--; i2--; i3--;
            indices.insert(indices.end(), {i1,i2,i3});

            glm::vec3 a{vertices[i1].position[0],vertices[i1].position[1],vertices[i1].position[2]};
            glm::vec3 b{vertices[i2].position[0],vertices[i2].position[1],vertices[i2].position[2]};
            glm::vec3 c{vertices[i3].position[0],vertices[i3].position[1],vertices[i3].position[2]};

            glm::vec3 normal = glm::cross(b-a, c-a);
            vertices[i1].normal[0] += normal.x;
            vertices[i1].normal[1] += normal.y;
            vertices[i1].normal[2] += normal.z;

            vertices[i2].normal[0] += normal.x;
            vertices[i2].normal[1] += normal.y;
            vertices[i2].normal[2] += normal.z;

            vertices[i3].normal[0] += normal.x;
            vertices[i3].normal[1] += normal.y;
            vertices[i3].normal[2] += normal.z;
        }
        else if(c != '#')
        {
            return false;
        }
    }

    return true;
}

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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Min number of screen updates to wait for until the buffers are swapped by glfwSwapBuffers
    glfwSwapInterval(1);

    // Create OpenGL rendering context before calling glewInit
    // Initialize the extension entry points
    if(glewInit() != GLEW_OK)
        std::cout << "GLEW ERROR!" << std::endl;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    if(!ImportObjFile("/Users/michael/Documents/projects/boids/res/models/teapot_seamless.obj", vertices, indices))
        std::cout << "Error - ImportObjFile" << std::endl;

    VertexArray va;
    VertexBuffer vb(&vertices[0], vertices.size() * sizeof(Vertex));

    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(3); // color

    IndexBuffer ib(&indices[0], indices.size());

    va.AddBuffer(vb, layout, ib);

    // Create and use shader program
    // *** Relative file paths are relative to current working directory ***
    Shader shader(
        "/Users/michael/Documents/projects/boids/res/shaders/vertex.shader", 
        "/Users/michael/Documents/projects/boids/res/shaders/fragment.shader");
    shader.Bind();

    Camera camera(glm::vec3(0.0f,10.0f,40.0f), glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,1.0f,0.0f));

    glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 1.0f, 150.0f);
    glm::mat4 view_mat;

	glm::mat4 model_mat(1.0f);
    model_mat = glm::scale(glm::vec3(5.0f,5.0f,5.0f)) * model_mat;

    glm::mat4 normal_mat = glm::transpose(glm::inverse(model_mat));
    shader.SetUniformMatrix4f("model_mat", model_mat);
    shader.SetUniformMatrix3f("normal_mat", normal_mat);

    // glm::vec3 light_pos(0.0f,10.0f,0.0f);
    glm::vec3 light_intensity(5.0f, 5.0f, 5.0f);
    shader.SetUniform3f("light_pos", camera.GetPosition());
    shader.SetUniform3f("light_intensity", light_intensity);

    glm::vec3 ambient_coef(0.01f, 0.01f, 0.01f);
    glm::vec3 diffuse_coef(0.15f, 0.15f, 0.15f);
    glm::vec3 specular_coef(0.15f, 0.15f, 0.15f);
    shader.SetUniform3f("ambient_coef", ambient_coef);
    shader.SetUniform3f("diffuse_coef", diffuse_coef);
    shader.SetUniform3f("specular_coef", specular_coef);

    int phong_exp = 64;
    shader.SetUniform1i("phong_exp", phong_exp);

    float prev_time = 0, cur_time, dt;
    Renderer renderer;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 150";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window) && !Input::EscapePressed())
    {
        // Poll for and process events
        glfwPollEvents();

        {
            cur_time = glfwGetTime();
            dt = cur_time - prev_time;
            prev_time = cur_time;

            if(Input::UpPressed()) camera.MoveForward(dt);
            if(Input::DownPressed()) camera.MoveBackward(dt);
            if(Input::LeftPressed()) camera.MoveLeft(dt);
            if(Input::RightPressed()) camera.MoveRight(dt);
            if(Input::SpacePressed()) camera.MoveUp(dt);
            if(Input::RightShiftPressed()) camera.MoveDown(dt);

            int delta_x, delta_y;
            Input::GetMouseDelta(window, delta_x, delta_y);
            camera.Rotate(delta_x, -delta_y);

            view_mat = camera.GetViewMatrix();
            shader.SetUniformMatrix4f("mvp_mat", proj_mat * view_mat * model_mat);
        }

        shader.SetUniform3f("light_pos", camera.GetPosition());
        shader.SetUniform3f("view_pos", camera.GetPosition());

        renderer.Clear();
        renderer.Draw(GL_TRIANGLES, va, shader);

        /*
        { // imgui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Debug Window");

            ImGui::SliderInt("phong_exp", &phong_exp, 1, 128);
            ImGui::SliderFloat3("light_intensity", &light_intensity.x, 0.0f, 100.0f); 
            ImGui::SliderFloat3("ambient_coef", &ambient_coef.x, 0.0f, 1.0f); 
            ImGui::SliderFloat3("diffuse_coef", &diffuse_coef.x, 0.0f, 1.0f); 
            ImGui::SliderFloat3("specular_coef", &specular_coef.x, 0.0f, 1.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        */

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