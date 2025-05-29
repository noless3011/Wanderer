cmake_minimum_required(VERSION 3.15) # Or higher, modern CMake is recommended

# Define app name as a variable
set(APP_NAME "Wanderer")

project(${APP_NAME} VERSION 1.0)
message(STATUS "C++ Standard Requested: ${CMAKE_CXX_STANDARD}")
message(STATUS "C++ Standard Required: ${CMAKE_CXX_STANDARD_REQUIRED}")
message(STATUS "C++ Extensions Enabled: ${CMAKE_CXX_EXTENSIONS}")
message(STATUS "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# Set C++ Standard (Choose your desired standard)
set(CMAKE_CXX_STANDARD 17) # Or 14, 20, etc.
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF) # Optional: Disable compiler extensions

# --- Compiler Warning Flags ---
if(MSVC)
    # For MSVC, /WX treats warnings as errors. /WX- disables this.
    # Add /W3 or /W4 for warning levels if not already set by default or other means.
    # target_compile_options(${APP_NAME} PRIVATE /W3 /WX-) # Example
    string(REPLACE "/WX" "/WX-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_compile_options(/WX-) # Add /WX- globally if not already managed by CMAKE_CXX_FLAGS
else()
    # For GCC/Clang, -Werror treats warnings as errors.
    # We can remove -Werror if it's present or add -Wno-error.
    # string(REPLACE "-Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # Remove -Werror
    add_compile_options(-Wno-error) # Add -Wno-error globally
endif()
# --- End Compiler Warning Flags ---

# Source Files
# Find all source files recursively
file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

# Add glad.c explicitly since it's important
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c")

# Print the found source files for debugging
message(STATUS "Source files: ${SOURCE_FILES}")

# PCH configuration
set(PCH_HEADER "include/pch.h")
set(PCH_SOURCE "src/pch.cpp")

# Add PCH source file if it exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PCH_SOURCE}")
    list(APPEND SOURCE_FILES ${PCH_SOURCE})
endif()

# Include Directories
include_directories(include) # Base include directory
include_directories(include/glad) # Specifically for glad.h
include_directories(include/spdlog) # For spdlog headers

# Add executable target
add_executable(${APP_NAME} ${SOURCE_FILES})

# Set glad.c to not use precompiled headers
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

# Add this after your other add_custom_command calls
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${CMAKE_SOURCE_DIR}/glfw3.dll" 
    "$<TARGET_FILE_DIR:${APP_NAME}>/glfw3.dll"
    COMMENT "Copying GLFW DLL to build directory"
)
# Configure precompiled header
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${APP_NAME} PRIVATE "/Yu${PCH_HEADER}")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PCH_SOURCE}")
        set_source_files_properties(${PCH_SOURCE} PROPERTIES COMPILE_FLAGS "/Yc${PCH_HEADER}")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC/Clang style PCH support
    target_precompile_headers(${APP_NAME} PRIVATE ${PCH_HEADER})
endif()

# --- Dependencies ---

# 1. OpenGL (Assuming system OpenGL is used)
find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    target_link_libraries(${APP_NAME} PRIVATE OpenGL::GL) # Link OpenGL library
else()
    message(FATAL_ERROR "OpenGL library not found. Please make sure it's installed.")
endif()

# 2. GLFW (Linking against pre-built GLFW library from 'lib' directory)
set(GLFW_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(GLFW_LIB_NAME glfw3dll) # Corrected: Removed 'lib' prefix

find_library(GLFW_LIBRARY
    NAMES lib${GLFW_LIB_NAME}.a # Adjusted to find libglfw3dll.a specifically if needed, or rely on linker paths
    PATHS ${GLFW_LIB_DIR}
    PATH_SUFFIXES ""
)

if(GLFW_LIBRARY)
    message(STATUS "Found GLFW library: ${GLFW_LIBRARY}")
    target_link_directories(${APP_NAME} PRIVATE ${GLFW_LIB_DIR})
    target_link_libraries(${APP_NAME} PRIVATE ${GLFW_LIB_NAME}) # Now correctly links against glfw3dll
    target_link_libraries(${APP_NAME} PRIVATE spdlogd) # Link GLFW library
else()
    message(FATAL_ERROR "GLFW library not found in ${GLFW_LIB_DIR}. Please check if libglfw3dll.a exists there.")
endif()

# --- Assets ---
set(ASSETS_DIR "assets")

# Copy assets to the build directory
file(GLOB ASSET_FILES "${ASSETS_DIR}/*")
foreach(ASSET_FILE ${ASSET_FILES})
    get_filename_component(ASSET_FILENAME ${ASSET_FILE} NAME)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSET_FILE}" $<TARGET_FILE_DIR:${APP_NAME}>/${ASSETS_DIR}/${ASSET_FILENAME}
        COMMENT "Copying asset: ${ASSET_FILENAME}"
    )
endforeach()


# --- Optional: Compiler flags ---
target_compile_options(${APP_NAME} PRIVATE
    -Wall
)

# For Release builds, you might want to add optimizations:
if(CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_options(${APP_NAME} PRIVATE
        -O3
    )
endif()
