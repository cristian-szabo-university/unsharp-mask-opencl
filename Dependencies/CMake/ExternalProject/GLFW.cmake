include (ExternalProject)

ExternalProject_Add (
    glfw3
    
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "master"
       
    CMAKE_ARGS "-DGLFW_BUILD_EXAMPLES=OFF" "-DGLFW_BUILD_TESTS=OFF" "-DGLFW_BUILD_DOCS=OFF" "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>")