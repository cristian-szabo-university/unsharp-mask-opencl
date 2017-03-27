#include "Main.hpp"

#include "UnsharpMask.hpp"

#include "Program.hpp"

static Program app;

void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << std::to_string(error) << "\n";
    std::cerr << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    app.onKeyEvent(key, scancode, action, mods);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    app.onResizeEvent(width, height);
}

void window_close_callback(GLFWwindow* window)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(int argc, char** argv)
{
    GLFWwindow* window;

    glewExperimental = GL_TRUE;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        std::cerr << "Failed to initialise GLFW libray!\n";

        return 1;
    }

    window = glfwCreateWindow(1280, 720, "Visualiser", NULL, NULL);
    
    if (!window)
    {
        std::cerr << "Failed to create OpenGL window!\n";

        return 2;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialise GLEW libray!\n";

        return 3;
    }

    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSwapInterval(1);

    int error_code = 0;

    try
    {
        app.create({ argv + 1, argv + argc }, window);

        while (!glfwWindowShouldClose(window))
        {
            app.render();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    catch (cl::Error& ex)
    {
        std::cerr << "OpenCL Error: " << ex.err() << std::endl;
        std::cerr << ex.what() << std::endl;

        error_code = 4;
    }
    catch (std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << std::endl;

        error_code = 5;
    }

    app.destroy();

    glfwDestroyWindow(window);

    glfwTerminate();

    return error_code;
}
