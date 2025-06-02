#pragma once

// Standard library
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream> // Still useful for non-logging output or if spdlog uses it
#include <map>
#include <memory>
#include <mutex>
#include <bitset>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>


#if defined(_WIN32) // Check if compiling on Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h> // For APIENTRY and other Windows API specifics
#endif
// OpenGL and graphics
#include <glad/glad.h>
#include <glfw3.h>

// Math library - GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// Logging library - spdlog
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
 // For std::ostream support if needed, though usually not for basic types

// Forward declaration for utility functions (definitions below or in a .cpp if they grow)
inline void glClearError();
inline bool glLogCall(const char* function, const char* file, int line);
#ifndef SPDLOG_FUNC
#if defined(__PRETTY_FUNCTION__) // GCC, Clang
#define SPDLOG_FUNC __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)     // MSVC
#define SPDLOG_FUNC __FUNCSIG__
#elif defined(__func__)        // C99/C++11 standard
#define SPDLOG_FUNC __func__
#else
#define SPDLOG_FUNC "" // Fallback to empty string if no function macro is available
#endif
#endif
// Common macros
#ifdef _DEBUG
#define ASSERT(x) if(!(x)) __debugbreak()
#define GL_CALL(x) do { ::glClearError(); x; ASSERT(::glLogCall(#x, __FILE__, __LINE__)); } while(0) // glLogCall should use LOG_ERROR from below
#else
#define ASSERT(x) x
#define GL_CALL(x) x
#endif

// Logging macros using spdlog with source location
// Your spdlog pattern (set in main.cpp) will control how file/line/func are displayed.
// Example pattern: spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%# %!] %v");

#ifdef _DEBUG
#define LOG_TRACE(...)    ::spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNC}, spdlog::level::trace, __VA_ARGS__)
#define LOG_INFO(...)     ::spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNC}, spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...)     ::spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNC}, spdlog::level::warn, __VA_ARGS__)
#else
#define LOG_TRACE(...)    (void)0
#define LOG_INFO(...)     (void)0
#define LOG_WARN(...)     (void)0
// Alternative for release: let spdlog filter, but still provide source loc if needed for higher levels
// #define LOG_TRACE(...)    ::spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNC}, spdlog::level::trace, __VA_ARGS__)
// #define LOG_INFO(...)     ::spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNC}, spdlog::level::info, __VA_ARGS__)
// #define LOG_WARN(...)     ::spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNC}, spdlog::level::warn, __VA_ARGS__)
#endif

#define LOG_ERROR(...)    ::spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNC}, spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(...) do { \
    ::spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNC}, spdlog::level::critical, __VA_ARGS__); \
    ASSERT(false); \
} while(0)

// Utility functions
inline void glClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

inline bool glLogCall(const char* function, const char* file, int line)
{
    bool no_error = true;
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        // Use spdlog for error reporting
        LOG_ERROR("[OpenGL Error] ({0}): {1} in {2}:{3}", error, function, file, line);
        no_error = false;
    }
    return no_error;
}