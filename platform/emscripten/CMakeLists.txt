cmake_minimum_required(VERSION 3.8)

set(CMAKE_GENERATOR "Ninja")

if(NOT DEFINED ENV{EMSDK})
    message(FATAL_ERROR "Environment variable EMSDK is not defined. Follow the README steps and source emsdk_env.* from the EMSDK root directory in this shell before running CMake")
endif()

project(mbgl-wasm LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(MLN_WITH_CORE_ONLY ON)
set(MLN_WITH_WERROR OFF)
set(DEPS ${CMAKE_CURRENT_SOURCE_DIR}/deps)
add_subdirectory(../../ ./test)


set(CMAKE_EXECUTABLE_SUFFIX ".mjs")

# if (DEFINED EMSCRIPTEN)
    add_executable(mbgl-wasm ${PROJECT_SOURCE_DIR}/main.cpp)

    target_include_directories(mbgl-wasm PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR} 
        ../../include
    )

    target_link_libraries(mbgl-wasm PRIVATE mbgl-core)

	set_target_properties(mbgl-wasm PROPERTIES COMPILE_FLAGS "-O0 -s DISABLE_EXCEPTION_CATCHING=0")
    set_target_properties(mbgl-wasm PROPERTIES LINK_FLAGS "--no-heap-copy -O0 -lembind --emit-tsd interface.d.ts -lhtml5.js -lhtml5_webgl.js -lglfw.js -s ENVIRONMENT='web' -s MODULARIZE=1 -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s USE_GLFW=3 -s USE_WEBGPU=1 -s NO_EXIT_RUNTIME=0 -s STANDALONE_WASM=0 -s EXIT_RUNTIME=1 -s ASSERTIONS=1 -s STACK_OVERFLOW_CHECK=2 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -sFULL_ES3 -s DISABLE_EXCEPTION_CATCHING=0 -s SINGLE_FILE=0")

    set_target_properties(mbgl-wasm PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
# endif()
