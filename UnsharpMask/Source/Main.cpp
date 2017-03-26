#include "Main.hpp"

#include "Program.hpp"

int main(int argc, char** argv)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialise GLFW libray!\n";

        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Unsharp Mask", NULL, NULL);
    
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

    int error_code;

    try
    {
        Program app({ argv + 1, argv + argc });

        error_code = app.run(window);
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

    glfwDestroyWindow(window);

    glfwTerminate();

    return error_code;
}
