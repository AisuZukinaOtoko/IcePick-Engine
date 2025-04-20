//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include <iostream>
//#include <fstream>
//#include <string>
//#include "Render Systems/VertexBuffer.h"
//#include "Render Systems/IndexBuffer.h"
//#include "Render Systems/VertexArray.h"
//#include "Render Systems/Texture.h"
//#include "Vendor/glm/glm.hpp"
//#include "Vendor/glm/gtc/matrix_transform.hpp"
//#include "Vendor/imgui/imgui.h"
//#include "Vendor/imgui/imgui_impl_glfw.h"
//#include "Vendor/imgui/imgui_impl_opengl3.h"
#include "IcePickRuntime.h"


int main(void)
{
    IcePick::Engine RuntimeEngine;
    if (!RuntimeEngine.Init()) {
        std::cout << "Engine failed to init.\n";
        RuntimeEngine.Terminate();
        //glfwTerminate();
        return -1;
    }

    //if (!RuntimeEngine.EnableEditor()) {
    //    std::cout << "Engine failed to init editor support.\n";
    //    RuntimeEngine.Terminate();
    //    //glfwTerminate();
    //    return -1;
    //};


    //GLFWwindow* window;
    ///* Initialize the library */
    //if (!glfwInit())
    //    return -1;

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //window = glfwCreateWindow(1280, 960, "Hello World", NULL, NULL);
    ///* Create a windowed mode window and its OpenGL context */
    //if (!window)
    //{
    //    glfwTerminate();
    //    return -1;
    //}
    ///* Make the window's context current */
    //glfwMakeContextCurrent(window);
    //glfwSwapInterval(1);
    //GLenum err = glewInit();
    //if (GLEW_OK != err)
    //{
    //    /* Problem: glewInit failed, something is seriously wrong. */
    //    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    //    return -1;
    //}

    // Imgui initialisation
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //ImGui::StyleColorsDark();
    //ImGui_ImplGlfw_InitForOpenGL(window, true);
    //ImGui_ImplOpenGL3_Init();


    //float positions[] = { // vertex coords vec3, texture coords vec2
    //    -0.4f,  0.2f, 0.5f, 0.0f, 1.0f,
    //    -1.0f, -1.0f, 0.5f, 0.0f, 0.0f,
    //     1.0f, -1.0f, 0.5f, 1.0f, 0.0f,
    //     0.4f,  0.2f, 0.5f, 1.0f, 1.0f
    //};

    //unsigned int indices[] = {
    //    3, 0, 1,
    //    1, 2, 3
    //};

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Texture texture("res/textures/CatWithHat.png");
    ////Texture texture("res/textures/Sword.png");
    //texture.Bind(0);

    //unsigned int vao;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    //VertexArray vertexArray;
    //VertexBuffer vertexBuffer(positions, 5 * 4 * sizeof(float));
    //VertexBufferLayout layout;
    //layout.Push<float>(3); // change this to reflect vertex buffer layout
    //layout.Push<float>(2);
    //vertexArray.AddBuffer(vertexBuffer, layout);
    //IndexBuffer indexBuffer(indices, 6);

    //unsigned int bufferID;
   // glGenBuffers(1, &bufferID);
    //glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    //glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    


    //unsigned int indexBufferObject;
    //glGenBuffers(1, &indexBufferObject);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);


    //Renderer renderer;

    //glBindVertexArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //bool show_demo_window = false;
    //float clear_color[3] = {};

    RuntimeEngine.Run();

    /* Loop until the user closes the window */
    /*while (true) {
        glClearColor(1.0f, 0.0f, 0.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        vertexArray.Bind();
        renderer.Draw(vertexArray, indexBuffer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }*/

    // Cleanup
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();

    //glDeleteProgram(shader);
    //glfwTerminate();
    return 0;
}