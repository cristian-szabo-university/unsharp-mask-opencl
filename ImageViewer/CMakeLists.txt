project (ImageViewer)

find_package (docopt REQUIRED CONFIG PATHS ${DEPENDENCIES_PATH})
find_package (glfw3 REQUIRED CONFIG PATHS ${DEPENDENCIES_PATH})

find_package (OpenGL REQUIRED)

if (OpenGL_FOUND)
    add_library(OpenGL INTERFACE IMPORTED)
    
    set_target_properties(OpenGL PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${OPENGL_LIBRARIES}")
endif ()

file (GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp")
file (GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

source_group ("Include\\" FILES ${HEADER_FILES})
source_group ("Source\\" FILES ${SOURCE_FILES})

add_executable (${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories (${PROJECT_NAME} PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>)

target_link_libraries (${PROJECT_NAME} PUBLIC UnsharpMask OpenGL glfw docopt_s)

add_custom_command (TARGET ${PROJECT_NAME} 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "$<TARGET_FILE_DIR:UnsharpMask>/$<TARGET_FILE_NAME:UnsharpMask>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

install (TARGETS ${PROJECT_NAME} EXPORT ${EXPORT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})