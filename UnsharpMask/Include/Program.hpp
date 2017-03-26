#pragma once

#pragma warning(disable:4996)

#define __CL_ENABLE_EXCEPTIONS

#include "GL/glew.h"
#include "CL/cl.hpp"
#include "GLFW/glfw3.h"
#include "docopt.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

class Program
{
public:

    Program(std::vector<std::string> args);

    ~Program();

    int run(GLFWwindow* window);

private:

    std::map<std::string, docopt::value> args;

    std::vector<float> createGaussianFilter(std::int32_t radius, float sigma);

};
