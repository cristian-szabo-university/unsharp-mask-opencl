include (ExternalProject)

include ("External_OpenCL-Headers")

ExternalProject_Add (
    OpenCL-ICD-Loader

    DEPENDS OpenCL-Headers
    
    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-ICD-Loader"
    GIT_TAG "master"
    
    INSTALL_COMMAND "")

ExternalProject_Get_Property (OpenCL-ICD-Loader SOURCE_DIR)

ExternalProject_Add_Step (OpenCL-ICD-Loader copy_opencl_header_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SOURCE_DIR}/inc/CL"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_HEADER_FILES} "${SOURCE_DIR}/inc/CL/"
	DEPENDEES update
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

ExternalProject_Get_Property (OpenCL-ICD-Loader BINARY_DIR)   
ExternalProject_Get_Property (OpenCL-ICD-Loader INSTALL_DIR)

set (FILE_NAME "OpenCL")

set (OUTPUT_SHARED_FILE_NAME "${FILE_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
set (OUTPUT_STATIC_FILE_NAME "${FILE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")

if (WIN32)
    ExternalProject_Add_Step (OpenCL-ICD-Loader install_opencl_dll_file
        COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DIR}/bin"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${BINARY_DIR}/bin/$<CONFIG>/${OUTPUT_SHARED_FILE_NAME}" "${INSTALL_DIR}/bin/"
        DEPENDEES install)
        
    ExternalProject_Add_Step (OpenCL-ICD-Loader install_opencl_lib_file
        COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DIR}/lib"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${BINARY_DIR}/$<CONFIG>/${OUTPUT_STATIC_FILE_NAME}" "${INSTALL_DIR}/lib/"
        DEPENDEES install)    
else ()
    ExternalProject_Add_Step (OpenCL-ICD-Loader install_opencl_file
        COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DIR}/bin"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${BINARY_DIR}/bin/${OUTPUT_SHARED_FILE_NAME}" "${INSTALL_DIR}/bin/"
        DEPENDEES install)
endif ()
