ExternalProject_Add (
    GLFW
    
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "master"
       
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>")