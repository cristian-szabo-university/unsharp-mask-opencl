include (ExternalProject)

ExternalProject_Add (
    docopt

    GIT_REPOSITORY "https://github.com/docopt/docopt.cpp.git"
    GIT_TAG "master"
       
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>")
