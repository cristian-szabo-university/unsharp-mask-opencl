include (FindPackageHandleStandardArgs)

find_path (OpenCL_CLHPP_INCLUDE_DIR
    NAMES
        CL/cl.hpp
    PATHS
        ${DEPENDENCIES_PATH}
    PATH_SUFFIXES
        OpenCL-CLHPP
    NO_DEFAULT_PATH)

find_package_handle_standard_args (OpenCL_CLHPP
    FOUND_VAR OpenCL_CLHPP_FOUND
    REQUIRED_VARS OpenCL_CLHPP_INCLUDE_DIR
    VERSION_VAR "1.2")

mark_as_advanced (OpenCL_CLHPP_INCLUDE_DIR)

if (OpenCL_CLHPP_FOUND)
    add_library(OpenCL_CLHPP INTERFACE IMPORTED)
    
    set_target_properties(OpenCL_CLHPP PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OpenCL_CLHPP_INCLUDE_DIR}")
endif ()
