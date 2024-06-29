cmake_minimum_required(VERSION 3.8)

set(CMAKE_GENERATOR "Ninja")

if(NOT DEFINED ENV{EMSDK})
    message(FATAL_ERROR "Environment variable EMSDK is not defined. Follow the README steps and source emsdk_env.* from the EMSDK root directory in this shell before running CMake")
endif()

set(CMAKE_GENERATOR "Ninja")
set(CMAKE_TOOLCHAIN_FILE ../../vendor/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_TARGET_TRIPLET "wasm32-emscripten")

set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE $ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)

project(mbgl-wasm LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(libuv CONFIG REQUIRED)
find_package(JPEG REQUIRED)
find_package(ZLIB REQUIRED)

# set(MLN_WITH_CORE_ONLY ON)
set(MLN_WITH_WERROR OFF)
set(DEPS ${CMAKE_CURRENT_SOURCE_DIR}/deps)
add_subdirectory(../../ ./test)

set(CMAKE_EXECUTABLE_SUFFIX ".html")



# if (DEFINED EMSCRIPTEN)
    add_executable(mbgl-wasm ${PROJECT_SOURCE_DIR}/main.cpp)

    target_include_directories(mbgl-wasm PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR} 
        ${PROJECT_SOURCE_DIR}/../../include
        ${PROJECT_SOURCE_DIR}/../../platform/windows/include
        ${PROJECT_SOURCE_DIR}/../../platform/default/include
    )

    target_link_libraries(mbgl-wasm PRIVATE 
        $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv> 
        JPEG::JPEG 
        Mapbox::Base::Extras::args 
        mbgl-compiler-options 
        mbgl-core
    )

	set_target_properties(mbgl-wasm PROPERTIES COMPILE_FLAGS "-O0 -pthread -s DISABLE_EXCEPTION_CATCHING=0 -DEMSCRIPTEN_HAS_UNBOUND_TYPE_NAMES=0")
    set_target_properties(mbgl-wasm PROPERTIES LINK_FLAGS "--no-heap-copy -pthread -O0 -lembind -lhtml5.js -lhtml5_webgl.js -lglfw.js -s ENVIRONMENT='web,worker' -s MODULARIZE=0 -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s USE_GLFW=3 -s USE_WEBGPU=1 -s NO_EXIT_RUNTIME=0 -s STANDALONE_WASM=0 -s EXIT_RUNTIME=1 -s ASSERTIONS=1 -s STACK_OVERFLOW_CHECK=2 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -sFULL_ES3 -s DISABLE_EXCEPTION_CATCHING=0 -s SINGLE_FILE=0")

    set_target_properties(mbgl-wasm PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
# endif()
