#pragma once

// Standard library
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <filesystem>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <stack>
#include <memory>
#include <functional>
#include <algorithm>
#include <chrono>
#include <utility>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <random>

// OpenGL and graphics
#include "glad/glad.h"
#include "GLFW/glfw3.h"

// Common macros
#ifdef _DEBUG
#define ASSERT(x) if(!(x)) __debugbreak()
#define GL_CALL(x) do { ::glClearError(); x; ASSERT(::glLogCall(#x, __FILE__, __LINE__)); } while(0)
#else
#define ASSERT(x) x
#define GL_CALL(x) x
#endif

// Utility functions
inline void glClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

inline bool glLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cerr << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}