include ("External_OpenCL-Headers")

ExternalProject_Add (
    OpenCL-ICD-Loader

    DEPENDS OpenCL-Headers
    
    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-ICD-Loader"
    GIT_TAG "master"
    
    CMAKE_ARGS "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=<INSTALL_DIR>"
    
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property (OpenCL-ICD-Loader SOURCE_DIR)

ExternalProject_Add_Step (OpenCL-ICD-Loader copy_opencl_header_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SOURCE_DIR}/inc/CL"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_HEADER_FILES} "${SOURCE_DIR}/inc/CL/"
	DEPENDEES update
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
