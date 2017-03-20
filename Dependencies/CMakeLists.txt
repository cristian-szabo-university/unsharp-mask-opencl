cmake_minimum_required (VERSION 3.7)

include (ExternalProject)

project (Dependencies VERSION 1.0.0)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

set_directory_properties (PROPERTIES EP_BASE "${CMAKE_BINARY_DIR}")

include ("External_GLEW")
include ("External_GLFW")
include ("External_OpenCL")
