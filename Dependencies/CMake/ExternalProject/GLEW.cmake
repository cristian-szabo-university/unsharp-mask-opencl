include (ExternalProject)

ExternalProject_Add (
    glew
    
    SOURCE_SUBDIR "build/cmake"
    
    URL "https://downloads.sourceforge.net/project/glew/glew/1.13.0/glew-1.13.0.zip"
    
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>" "-DBUILD_UTILS=OFF")