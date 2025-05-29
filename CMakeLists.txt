cmake_minimum_required(VERSION 3.15)

set(APP_NAME "Wanderer")
project(${APP_NAME} VERSION 1.0)

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Project Source Dir: ${PROJECT_SOURCE_DIR}")
message(STATUS "Project Binary Dir: ${PROJECT_BINARY_DIR}")

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "C++ Standard Requested: ${CMAKE_CXX_STANDARD}")
message(STATUS "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# --- Compiler Warning Flags ---
if(MSVC)
    string(REPLACE "/WX" "/WX-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # Remove /WX if present
    add_compile_options(/W4 /WX-) # High warning level, treat warnings as errors OFF
else()
    # For GCC/Clang, -Werror treats warnings as errors.
    string(REPLACE "-Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # Remove -Werror
    add_compile_options(-Wall) # Common warning flags
endif()
# --- End Compiler Warning Flags ---

# --- Source Files ---
# Recursively find all source files in the src directory and its subdirectories
# CONFIGURE_DEPENDS ensures CMake reruns if new files are added/removed matching the glob
file(GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

set(GLAD_C_PATH_RELATIVE "src/glad/glad.c") # Expected path relative to project root
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${GLAD_C_PATH_RELATIVE}")
    message(WARNING "GLAD source file not found at '${GLAD_C_PATH_RELATIVE}'. Update path if necessary.")
endif()

# PCH configuration
set(PCH_HEADER_RELATIVE "pch.h") # Relative to project root
set(PCH_SOURCE_RELATIVE "src/pch.cpp")   # Relative to project root


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PCH_SOURCE_RELATIVE}")
    message(STATUS "PCH source found: ${PCH_SOURCE_RELATIVE}")
else()
    message(WARNING "PCH source file not found at ${PCH_SOURCE_RELATIVE}")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PCH_HEADER_RELATIVE}")
    message(WARNING "PCH header file not found at ${PCH_HEADER_RELATIVE}")
endif()

message(STATUS "Source files identified:")
foreach(FILE ${SOURCE_FILES})
    message(STATUS "  ${FILE}")
endforeach()

# --- Add Executable ---
add_executable(${APP_NAME} ${SOURCE_FILES})

# --- Include Directories ---
target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"           # For your project headers (e.g., core/, game/, graphics/, utils/)
    "${CMAKE_CURRENT_SOURCE_DIR}/vendors/glfw/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendors/spdlog/include"
)

# --- Precompiled Headers (PCH) ---
# Set glad.c (or any other generated/third-party C file) to not use precompiled headers
# Ensure GLAD_C_PATH_RELATIVE points to the correct location of glad.c
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${GLAD_C_PATH_RELATIVE}")
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/${GLAD_C_PATH_RELATIVE}" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
    message(STATUS "Configured ${GLAD_C_PATH_RELATIVE} to skip PCH.")
else()
    message(WARNING "Could not set SKIP_PRECOMPILE_HEADERS for GLAD: ${GLAD_C_PATH_RELATIVE} not found by CMake.")
endif()

# Configure precompiled header for your project sources
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PCH_HEADER_RELATIVE}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PCH_SOURCE_RELATIVE}")
    if(MSVC)
        target_compile_options(${APP_NAME} PRIVATE "/Yu${PCH_HEADER_RELATIVE}")
        set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/${PCH_SOURCE_RELATIVE}" PROPERTIES COMPILE_FLAGS "/Yc${PCH_HEADER_RELATIVE}")
        message(STATUS "MSVC PCH configured with ${PCH_HEADER_RELATIVE} (creator: ${PCH_SOURCE_RELATIVE})")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_precompile_headers(${APP_NAME} PRIVATE "${PCH_HEADER_RELATIVE}")
        message(STATUS "GCC/Clang PCH configured with ${PCH_HEADER_RELATIVE}")
    endif()
else()
    message(STATUS "PCH not configured (header or source file missing).")
endif()

# --- Dependencies ---

# 1. OpenGL
find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    target_link_libraries(${APP_NAME} PRIVATE OpenGL::GL)
    message(STATUS "OpenGL library found and linked.")
else()
    message(FATAL_ERROR "OpenGL library not found.")
endif()

# 2. GLFW (from vendors)
set(GLFW_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendors/glfw/lib")
# Common names: glfw3dll (for glfw3.dll), glfw3 (for static lib glfw3.lib/libglfw3.a)
# Check your vendors/glfw/lib folder for the exact name.
# For MSVC, it's often glfw3.lib (static) or glfw3dll.lib (for DLL). For MinGW, libglfw3.a or libglfw3dll.a
find_library(GLFW_LIBRARY_FILE NAMES glfw3dll glfw3 PATHS "${GLFW_LIBRARY_DIR}")

if(GLFW_LIBRARY_FILE)
    message(STATUS "Found GLFW library: ${GLFW_LIBRARY_FILE}")
    # target_link_directories(${APP_NAME} PRIVATE "${GLFW_LIBRARY_DIR}") # Not strictly needed if find_library gives full path
    target_link_libraries(${APP_NAME} PRIVATE "${GLFW_LIBRARY_FILE}")
else()
    message(FATAL_ERROR "GLFW library not found in ${GLFW_LIBRARY_DIR}. Searched for 'glfw3dll' or 'glfw3'. Check names (e.g., glfw3.lib, libglfw3.a).")
endif()

# Copy GLFW DLL to build directory (for Windows)
if(WIN32)
    set(GLFW_DLL_PATH "${CMAKE_SOURCE_DIR}/vendors/glfw/bin/glfw3.dll")
    if(EXISTS "${GLFW_DLL_PATH}")
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLFW_DLL_PATH}"
            "$<TARGET_FILE_DIR:${APP_NAME}>/glfw3.dll"
            COMMENT "Copying GLFW DLL to build directory"
        )
        message(STATUS "Configured copying of GLFW DLL: ${GLFW_DLL_PATH}")
    else()
        message(WARNING "GLFW DLL not found at ${GLFW_DLL_PATH}. Runtime issues may occur.")
    endif()
endif()


# 3. spdlog (from vendors)
set(SPDLOG_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendors/spdlog/lib")
# Common names: spdlogd (debug), spdlog (release). Check your vendors/spdlog/lib.
# For header-only spdlog, you might not need to link a library.
# The old CMakeLists.txt linked `spdlogd`, implying a pre-built debug library.
find_library(SPDLOG_LIBRARY_FILE NAMES spdlogd spdlog PATHS "${SPDLOG_LIBRARY_DIR}")

if(SPDLOG_LIBRARY_FILE)
    message(STATUS "Found spdlog library: ${SPDLOG_LIBRARY_FILE}")
    # target_link_directories(${APP_NAME} PRIVATE "${SPDLOG_LIBRARY_DIR}") # Not strictly needed
    target_link_libraries(${APP_NAME} PRIVATE "${SPDLOG_LIBRARY_FILE}")
else()
    # If spdlog is header-only in your setup, this is not an error.
    message(WARNING "spdlog library not found in ${SPDLOG_LIBRARY_DIR}. Searched for 'spdlogd' or 'spdlog'. If spdlog is header-only, this is fine.")
endif()

# 4. glm
set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendors/glm")
if(EXISTS "${GLM_INCLUDE_DIR}")
    target_include_directories(${APP_NAME} PRIVATE "${GLM_INCLUDE_DIR}")
    message(STATUS "Configured GLM include directory: ${GLM_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "GLM include directory not found: ${GLM_INCLUDE_DIR}. Ensure GLM is present in vendors/glm.")
endif()


# --- Assets ---
set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ASSETS_DEST_DIR "$<TARGET_FILE_DIR:${APP_NAME}>/assets") # Destination next to executable

if(EXISTS "${ASSETS_SOURCE_DIR}")
    # Using add_custom_command with file(GLOB...) can be tricky for ensuring updates.
    # A more robust way for directories is to copy the entire directory.
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${ASSETS_SOURCE_DIR}" "${ASSETS_DEST_DIR}"
        COMMENT "Copying assets directory to build output"
    )
    message(STATUS "Configured copying of assets from ${ASSETS_SOURCE_DIR} to ${ASSETS_DEST_DIR}")
else()
    message(WARNING "Assets source directory not found: ${ASSETS_SOURCE_DIR}")
endif()


# --- Optional: Build type specific compiler flags ---
# The general warning flags are set above. These are for optimization/debug symbols.
target_compile_options(${APP_NAME} PRIVATE
    # Debug symbols - applied to all configurations (generally useful)
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<CXX_COMPILER_ID:MSVC>:/Zi>
    $<$<CXX_COMPILER_ID:GNU>:-g>
    $<$<CXX_COMPILER_ID:Clang>:-g>

    # Debug configuration specific flags
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:
        /Od       # Disable optimization (compatible with /RTC1)
        /DDEBUG
    >
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>: # For GCC
        -O0       # No optimization (or -Og for debug-friendly optimization)
        -DDEBUG
    >
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>>: # For Clang
        -O0       # No optimization (or -Og)
        -DDEBUG
    >

    # Release configuration specific flags
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:
        /O2 /Ob2  # Release optimizations
        /DNDEBUG
    >
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU>>: # For GCC
        -O2       # Release optimizations
        -DNDEBUG
    >
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:Clang>>: # For Clang
        -O2       # Release optimizations
        -DNDEBUG
    >
)
message(STATUS "CMake configuration finished for ${APP_NAME}.")