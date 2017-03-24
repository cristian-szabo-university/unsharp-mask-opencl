project (UnsharpMask)

find_package (docopt REQUIRED CONFIG PATHS ${DEPENDENCIES_PATH})
find_package (glfw3 REQUIRED CONFIG PATHS ${DEPENDENCIES_PATH})
find_package (glew REQUIRED CONFIG PATHS ${DEPENDENCIES_PATH})

find_package (OpenCL_CLHPP REQUIRED)
find_package (OpenCL_Headers REQUIRED)
find_package (OpenCL_ICD_Loader REQUIRED)

find_package (OpenGL REQUIRED)

if (OpenGL_FOUND)
    add_library(OpenGL INTERFACE IMPORTED)
    
    set_target_properties(OpenGL PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OpenGL_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${OpenGL_LIBRARY}")
endif ()

add_custom_command (
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Source/ProgramEntry.cpp" "${CMAKE_CURRENT_BINARY_DIR}/Include/ProgramEntry.hpp"
    COMMAND ${CMAKE_COMMAND} "-DCL_DIR=${CMAKE_CURRENT_SOURCE_DIR}/OpenCL" "-DFILE_OUT=ProgramEntry.cpp" "-DDIR_OUT=${CMAKE_CURRENT_BINARY_DIR}" -P "${CMAKE_SOURCE_DIR}/CMake/File2Code.cmake")

file (GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp")

list (APPEND HEADER_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Include/ProgramEntry.hpp)
 
file (GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

list (APPEND SOURCE_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Source/ProgramEntry.cpp)

source_group ("Include\\" FILES ${HEADER_FILES})
source_group ("Source\\" FILES ${SOURCE_FILES})

add_executable (${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories (${PROJECT_NAME} PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/Include>)

target_link_libraries (${PROJECT_NAME} PUBLIC 
    OpenCL_CLHPP OpenCL_Headers OpenCL_ICD_Loader 
    glfw GLEW::glew_s OpenGL
    docopt_s)

install (TARGETS ${PROJECT_NAME} EXPORT ${EXPORT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
