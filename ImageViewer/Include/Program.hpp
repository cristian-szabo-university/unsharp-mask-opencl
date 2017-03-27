#pragma once

#include "UnsharpMask.hpp"

#include "docopt.h"
#include "GL\glew.h"
#include "GLFW\glfw3.h"

class Program
{
public:

    void create(std::vector<std::string> cli, GLFWwindow* window);

    void render();

    void destroy();

    void onResizeEvent(int width, int height);

    void onKeyEvent(int key, int scancode, int action, int mod);

private:

    GLFWwindow* window;

    PPM image;

    cl::Image* orig_tex;

    cl_GLuint sharp_tex;

    std::vector<float> createGaussianFilter(std::int32_t radius, float sigma);

};
