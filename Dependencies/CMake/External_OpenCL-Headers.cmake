include (ExternalProject)

set (OpenCL_Headers_VERSION_MAJOR 2 CACHE STRING "OpenCL major version." FORCE)
set (OpenCL_Headers_VERSION_MINOR 1 CACHE STRING "OpenCL minor version." FORCE)

ExternalProject_Add (
    OpenCL-Headers

    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-Headers.git"
    GIT_TAG "opencl${OpenCL_Headers_VERSION_MAJOR}${OpenCL_Headers_VERSION_MINOR}"
    
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property (OpenCL-Headers SOURCE_DIR)
ExternalProject_Get_Property (OpenCL-Headers INSTALL_DIR)

ExternalProject_Add_Step (OpenCL-Headers install_opencl_header_files
    COMMAND ${CMAKE_COMMAND} "-DSRC=${SOURCE_DIR}/*.h" "-DDST=${INSTALL_DIR}/include/CL" -P "${CMAKE_SOURCE_DIR}/CMake/CopyFiles.cmake"
    DEPENDEES install)
