project (UnsharpMask)

include (GenerateExportHeader)

find_package (OpenCL_CLHPP REQUIRED)
find_package (OpenCL_Headers REQUIRED)
find_package (OpenCL_ICD_Loader REQUIRED)

add_custom_command (
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Source/ProgramEntry.cpp" "${CMAKE_CURRENT_BINARY_DIR}/Include/ProgramEntry.hpp"
    COMMAND ${CMAKE_COMMAND} "-DCL_DIR=${CMAKE_CURRENT_SOURCE_DIR}/OpenCL" "-DFILE_OUT=ProgramEntry.cpp" "-DDIR_OUT=${CMAKE_CURRENT_BINARY_DIR}" -P "${CMAKE_SOURCE_DIR}/CMake/File2Code.cmake")

configure_file (
	"${CMAKE_SOURCE_DIR}/CMake/${PROJECT_NAME}Config.cmake"
	${CMAKE_CURRENT_BINARY_DIR}/Include/Config.hpp @ONLY)
    
file (GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp")

list (APPEND HEADER_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Include/ProgramEntry.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/Include/Config.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/Include/Export.hpp)
 
file (GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

list (APPEND SOURCE_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Source/ProgramEntry.cpp)

source_group ("Include\\" FILES ${HEADER_FILES})
source_group ("Source\\" FILES ${SOURCE_FILES})

add_library (${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories (${PROJECT_NAME} PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/Include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries (${PROJECT_NAME} PUBLIC OpenCL_CLHPP OpenCL_Headers OpenCL_ICD_Loader)

generate_export_header(${PROJECT_NAME}
	EXPORT_MACRO_NAME "UNSHARP_MASK_PUBLIC"
	EXPORT_FILE_NAME "Include/Export.hpp")
    
install (DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
    
install (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
    
install (TARGETS ${PROJECT_NAME} EXPORT ${EXPORT_NAME}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
