#include "Config.hpp"

#include "GLFW\glfw3.h"

#include "Main.hpp"

#include "Program.hpp"
#include "ImageViewer.hpp"

void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << std::to_string(error) << "\n";
    std::cerr << description << std::endl;
}

int main(int argc, char** argv)
{
    Program prog;

    glfwSetErrorCallback(error_callback);

    std::shared_ptr<App> app = std::make_shared<ImageViewer>(
        std::vector<std::string>(argv + 1, argv + argc));

    if (!prog.create())
    {
        return 1;
    }

    if (!prog.execute(app))
    {
        return 2;
    }

    prog.loop();

    prog.destroy();

    return 0;
}
