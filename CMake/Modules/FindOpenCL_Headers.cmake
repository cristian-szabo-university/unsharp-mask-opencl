include (FindPackageHandleStandardArgs)
include (CheckSymbolExists)

find_path (OpenCL_Headers_INCLUDE_DIR
    NAMES
        CL/cl.h
    PATHS
        ${DEPENDENCIES_PATH}
    PATH_SUFFIXES
        OpenCL-Headers
    NO_DEFAULT_PATH)

foreach (VERSION "2_1" "2_0" "1_2" "1_1" "1_0")
    set(CMAKE_REQUIRED_INCLUDES "${OpenCL_Headers_INCLUDE_DIR}")

    CHECK_SYMBOL_EXISTS (
        CL_VERSION_${VERSION}
        "${OpenCL_Headers_INCLUDE_DIR}/CL/cl.h"
        OpenCL_Headers_VERSION_${VERSION})

    if (OpenCL_Headers_VERSION_${VERSION})
        string (REPLACE "_" "." VERSION "${VERSION}")
        
        set (OpenCL_Headers_VERSION_STRING ${VERSION})

        string (REGEX MATCHALL "[0-9]+" version_components "${VERSION}")

        list (GET version_components 0 major_version)
        list (GET version_components 1 minor_version)

        set (OpenCL_Headers_VERSION_MAJOR ${major_version})
        set (OpenCL_Headers_VERSION_MINOR ${minor_version})

        break ()
    endif ()
endforeach ()    

find_package_handle_standard_args (OpenCL_Headers
    FOUND_VAR OpenCL_Headers_FOUND
    REQUIRED_VARS OpenCL_Headers_INCLUDE_DIR
    VERSION_VAR OpenCL_Headers_VERSION_STRING)

mark_as_advanced (OpenCL_Headers_INCLUDE_DIR)

if (OpenCL_Headers_FOUND)
    add_library(OpenCL_Headers INTERFACE IMPORTED)
    
    set_target_properties(OpenCL_Headers PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OpenCL_Headers_INCLUDE_DIR}")
endif ()
