#pragma once

// Standard library
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

// OpenGL and graphics
#include <glad/glad.h>
#include <glfw3.h>

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