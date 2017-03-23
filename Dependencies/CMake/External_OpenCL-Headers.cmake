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

set (OPENCL_HEADER_FILES
    "${SOURCE_DIR}/cl.h"
    "${SOURCE_DIR}/opencl.h"
    "${SOURCE_DIR}/cl_platform.h"
    "${SOURCE_DIR}/cl_ext.h"
    "${SOURCE_DIR}/cl_gl.h"
    "${SOURCE_DIR}/cl_gl_ext.h"
    "${SOURCE_DIR}/cl_egl.h"
    "${SOURCE_DIR}/cl_d3d10.h"
    "${SOURCE_DIR}/cl_d3d11.h"
    "${SOURCE_DIR}/cl_dx9_media_sharing.h")

ExternalProject_Get_Property (OpenCL-Headers INSTALL_DIR)

ExternalProject_Add_Step (OpenCL-Headers install_opencl_header_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DIR}/CL"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_HEADER_FILES} "${INSTALL_DIR}/CL/"
    DEPENDEES install)
