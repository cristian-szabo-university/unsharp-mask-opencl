#include "GL/glew.h"
#include "CL/cl.hpp"
#include "GLFW/glfw3.h"

#include "Program.hpp"

Program::Program(std::vector<std::string> cli)
{
}

Program::~Program()
{
}

int Program::run()
{
    if (!glfwInit())
    {
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();

        return 2;
    }

    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
