include (FindPackageHandleStandardArgs)

find_library (OpenCL_ICD_Loader_LIBRARY
    NAMES 
        OpenCL
    PATHS
        ${DEPENDENCIES_PATH}
    PATH_SUFFIXES
        OpenCL-ICD-Loader/lib
        OpenCL-ICD-Loader/bin
    NO_DEFAULT_PATH)
    
find_package_handle_standard_args (OpenCL_ICD_Loader
    FOUND_VAR OpenCL_ICD_Loader_FOUND
    REQUIRED_VARS OpenCL_ICD_Loader_LIBRARY
    VERSION_VAR "1.0")

mark_as_advanced (OpenCL_ICD_Loader_LIBRARY)

if (OpenCL_ICD_Loader_FOUND)
    add_library(OpenCL_ICD_Loader INTERFACE IMPORTED)
    
    set_target_properties(OpenCL_ICD_Loader PROPERTIES
        INTERFACE_LINK_LIBRARIES "${OpenCL_ICD_Loader_LIBRARY}")
endif ()
