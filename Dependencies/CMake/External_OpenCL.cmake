ExternalProject_Add (
    OpenCL-CLHPP
        
    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-CLHPP.git"
    GIT_TAG "master"
    
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>" "-DBUILD_DOCS=OFF" "-DBUILD_EXAMPLES=OFF" "-DBUILD_TESTS=OFF"
    
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Add (
    OpenCL-Headers

    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-Headers.git"
    GIT_TAG "opencl21"
    
    CONFIGURE_COMMAND "" 
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property (OpenCL-Headers SOURCE_DIR)

set (OCL_HEADERS_SOURCE_DIR ${SOURCE_DIR})

ExternalProject_Add (
    OpenCL-ICD-Loader

    DEPENDS OpenCL-Headers OpenCL-CLHPP
    
    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-ICD-Loader"
    GIT_TAG "master"
    
    CMAKE_ARGS "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=<INSTALL_DIR>"
    
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

ExternalProject_Get_Property (OpenCL-ICD-Loader SOURCE_DIR)

ExternalProject_Add_Step (OpenCL-ICD-Loader copy_opencl_header_files
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCL_HEADERS_SOURCE_DIR}/cl.h" "${SOURCE_DIR}/inc/CL/cl.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCL_HEADERS_SOURCE_DIR}/opencl.h" "${SOURCE_DIR}/inc/CL/opencl.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCL_HEADERS_SOURCE_DIR}/cl_platform.h" "${SOURCE_DIR}/inc/CL/cl_platform.h" 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCL_HEADERS_SOURCE_DIR}/cl_ext.h" "${SOURCE_DIR}/inc/CL/cl_ext.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCL_HEADERS_SOURCE_DIR}/cl_gl.h" "${SOURCE_DIR}/inc/CL/cl_gl.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCL_HEADERS_SOURCE_DIR}/cl_gl_ext.h" "${SOURCE_DIR}/inc/CL/cl_gl_ext.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCL_HEADERS_SOURCE_DIR}/cl_egl.h" "${SOURCE_DIR}/inc/CL/cl_egl.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCL_HEADERS_SOURCE_DIR}/cl_d3d10.h" "${SOURCE_DIR}/inc/CL/cl_d3d10.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCL_HEADERS_SOURCE_DIR}/cl_d3d11.h" "${SOURCE_DIR}/inc/CL/cl_d3d11.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCL_HEADERS_SOURCE_DIR}/cl_dx9_media_sharing.h" "${SOURCE_DIR}/inc/CL/cl_dx9_media_sharing.h"
	DEPENDEES update
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
