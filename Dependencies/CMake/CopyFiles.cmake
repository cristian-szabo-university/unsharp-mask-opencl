file (GLOB FOUND_FILES ${SRC})

if (NOT FOUND_FILES)
    message (FATAL_ERROR "No files found!")
endif ()

file (MAKE_DIRECTORY ${DST})

execute_process (COMMAND ${CMAKE_COMMAND} -E copy ${FOUND_FILES} ${DST})
