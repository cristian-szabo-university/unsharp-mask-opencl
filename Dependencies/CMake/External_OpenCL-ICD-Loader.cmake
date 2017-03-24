include (ExternalProject)

include ("External_OpenCL-Headers")

ExternalProject_Add (
    OpenCL-ICD-Loader

    DEPENDS OpenCL-Headers
    
    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-ICD-Loader"
    GIT_TAG "master"
    
    INSTALL_COMMAND "")

ExternalProject_Get_Property (OpenCL-Headers INSTALL_DIR)
ExternalProject_Get_Property (OpenCL-ICD-Loader SOURCE_DIR)

ExternalProject_Add_Step (OpenCL-ICD-Loader before_build
    COMMAND ${CMAKE_COMMAND} "-DSRC=${INSTALL_DIR}/include/CL/*.h" "-DDST=${SOURCE_DIR}/inc/CL" -P "${CMAKE_SOURCE_DIR}/CMake/CopyFiles.cmake"
    DEPENDEES update    
	DEPENDERS build)

ExternalProject_Get_Property (OpenCL-ICD-Loader BINARY_DIR)   
ExternalProject_Get_Property (OpenCL-ICD-Loader INSTALL_DIR)

if (WIN32)
    ExternalProject_Add_Step (OpenCL-ICD-Loader after_install
        COMMAND ${CMAKE_COMMAND} "-DSRC=${BINARY_DIR}/bin/$<CONFIG>/*${CMAKE_SHARED_LIBRARY_SUFFIX}" "-DDST=${INSTALL_DIR}/bin" -P "${CMAKE_SOURCE_DIR}/CMake/CopyFiles.cmake"
        COMMAND ${CMAKE_COMMAND} "-DSRC=${BINARY_DIR}/$<CONFIG>/*${CMAKE_STATIC_LIBRARY_SUFFIX}" "-DDST=${INSTALL_DIR}/lib" -P "${CMAKE_SOURCE_DIR}/CMake/CopyFiles.cmake"
        DEPENDEES install)    
else ()
    ExternalProject_Add_Step (OpenCL-ICD-Loader after_install
        COMMAND ${CMAKE_COMMAND} "-DSRC=${BINARY_DIR}/bin/*${CMAKE_SHARED_LIBRARY_SUFFIX}" "-DDST=${INSTALL_DIR}/bin" -P "${CMAKE_SOURCE_DIR}/CMake/CopyFiles.cmake"
        DEPENDEES install)
endif ()
