ExternalProject_Add (
    OpenCL-CLHPP
        
    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-CLHPP.git"
    GIT_TAG "master"
    
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>" "-DBUILD_DOCS=OFF" "-DBUILD_EXAMPLES=OFF" "-DBUILD_TESTS=OFF")
