#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

void error_callback(int error, const char* description);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void window_close_callback(GLFWwindow* window);

int main(int argc, char** argv);
