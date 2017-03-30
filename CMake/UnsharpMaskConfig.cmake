#pragma once

#pragma warning(disable:4251)
#pragma warning(disable:4996)

#include <iostream>
#include <fstream>
#include <sstream>

#include <memory>
#include <algorithm>
#include <functional>
#include <chrono>
#include <ctime>
#include <future>
#include <numeric>

#include <cstdint>
#include <cstdarg>
#include <iomanip>

#include <array>
#include <string>
#include <vector>
#include <list>
#include <map>

#ifdef _WIN32
#   include <io.h> 
#   include <direct.h>
#else
#   include <unistd.h>
#   include <cxxabi.h>
#endif

#define __CL_ENABLE_EXCEPTIONS

#if defined(__APPLE__)
#   include <OpenCL/cl.hpp>
#else
#   include <CL/cl.hpp>
#endif

#define NOMINMAX

#include <GL/glew.h>
#include <GL/wglew.h>

#include "Export.hpp"
