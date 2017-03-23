project (UnsharpMask)

find_package (glfw3 REQUIRED CONFIG PATHS ${DEPENDENCIES_PATH})
find_package (glew REQUIRED CONFIG PATHS ${DEPENDENCIES_PATH})

find_package (OpenCL_CLHPP REQUIRED)
find_package (OpenCL_Headers REQUIRED)
find_package (OpenCL_ICD_Loader REQUIRED)

file (GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp")
file (GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

source_group ("Include\\" FILES ${HEADER_FILES})
source_group ("Source\\" FILES ${SOURCE_FILES})

add_executable (${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories (${PROJECT_NAME} PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>)

target_link_libraries (${PROJECT_NAME} PUBLIC OpenCL_CLHPP OpenCL_Headers OpenCL_ICD_Loader glfw GLEW::glew_s)

install (TARGETS ${PROJECT_NAME} EXPORT ${EXPORT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
