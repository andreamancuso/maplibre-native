if(NOT DEFINED ENV{EMSDK})
    message(FATAL_ERROR "Environment variable EMSDK is not defined. Follow the README steps and source emsdk_env.* from the EMSDK root directory in this shell before running CMake")
endif()

find_package(ICU OPTIONAL_COMPONENTS i18n uc)
find_path(DLFCN_INCLUDE_DIRS dlfcn.h)

include(${PROJECT_SOURCE_DIR}/vendor/nunicode.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/sqlite.cmake)

if(NOT ${ICU_FOUND} OR "${ICU_VERSION}" VERSION_LESS 62.0)
    message(STATUS "ICU not found or too old, using builtin.")

    set(MLN_USE_BUILTIN_ICU TRUE)
    include(${PROJECT_SOURCE_DIR}/vendor/icu.cmake)

    set_source_files_properties(
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/i18n/number_format.cpp
        PROPERTIES
        COMPILE_DEFINITIONS
        MBGL_USE_BUILTIN_ICU
    )
endif()

list(APPEND EXTRA_SRC_FILES
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_backend.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_frontend.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gl/headless_backend.cpp

        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/i18n/collator.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/i18n/number_format.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/layermanager/layer_manager.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/platform/time.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/asset_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/database_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/file_source_manager.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/file_source_request.cpp
        # ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/http_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/local_file_request.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/local_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/mbtiles_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/main_resource_loader.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline_database.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline_download.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/online_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/sqlite3.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/bidi.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/local_glyph_rasterizer.cpp
        
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/compression.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/filesystem.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/image.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/jpeg_reader.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/webp_reader.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/logging_stderr.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/monotonic_timer.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/png_reader.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/png_writer.cpp
        
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/string_stdlib.cpp
        
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/utf.cpp

        
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/util/thread.cpp
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/util/thread_local.cpp
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/util/async_task.cpp
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/util/timer.cpp
        
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/util/logging.cpp

        # GLFW
        # ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_view.cpp
        # ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_renderer_frontend.cpp
        # ${PROJECT_SOURCE_DIR}/platform/glfw/settings_json.cpp
        # ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/map/map_snapshotter.cpp

        # ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_gl_backend.cpp

        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/headless_backend_glfw.cpp
        ${PROJECT_SOURCE_DIR}/platform/windows/src/gl_functions.cpp
)

add_executable(mbgl-core-wasm ${INCLUDE_FILES} ${SRC_FILES} ${EXTRA_SRC_FILES})

target_include_directories(mbgl-core-wasm PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/platform/windows/include
    ${PROJECT_SOURCE_DIR}/platform/default/include
    ${PROJECT_SOURCE_DIR}/vendor/libwebp/src
)

target_link_libraries(mbgl-core-wasm 
    PRIVATE 
    Mapbox::Base::Extras::kdbush.hpp
        Mapbox::Base::supercluster.hpp
        Mapbox::Base::shelf-pack-cpp
        Mapbox::Base::geojson-vt-cpp
        Mapbox::Base::cheap-ruler-cpp
        mbgl-compiler-options
        mbgl-vendor-boost
        mbgl-vendor-csscolorparser
        mbgl-vendor-earcut.hpp
        mbgl-vendor-eternal
        mbgl-vendor-parsedate
        mbgl-vendor-polylabel
        mbgl-vendor-protozero
        mbgl-vendor-unique_resource
        mbgl-vendor-vector-tile
        mbgl-vendor-wagyu
        mbgl-vendor-nunicode
        $<$<BOOL:${MLN_USE_BUILTIN_ICU}>:mbgl-vendor-icu>
        Mapbox::Base::Extras::args 
        
    PUBLIC
        Mapbox::Base
        Mapbox::Base::Extras::expected-lite
        Mapbox::Base::Extras::rapidjson
        Mapbox::Base::geojson.hpp
        Mapbox::Base::geometry.hpp
        Mapbox::Base::variant
        unordered_dense
)

set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
set_target_properties(mbgl-core-wasm PROPERTIES COMPILE_FLAGS "-O0 -pthread --use-port=libpng --use-port=libjpeg --use-port=zlib --use-port=sqlite3 -s DISABLE_EXCEPTION_CATCHING=0 -DEMSCRIPTEN_HAS_UNBOUND_TYPE_NAMES=0 -DMLN_RENDER_BACKEND_OPENGL -DMLN_DRAWABLE_RENDERER")
set_target_properties(mbgl-core-wasm PROPERTIES LINK_FLAGS "--no-heap-copy -pthread --use-port=libpng --use-port=libjpeg --use-port=zlib --use-port=sqlite3 -O0 -lembind -lhtml5.js -lhtml5_webgl.js -lglfw.js -s ENVIRONMENT='web,worker' -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s USE_GLFW=3 -s USE_WEBGPU=1 -s STANDALONE_WASM=1 -s ASSERTIONS=1 -s STACK_OVERFLOW_CHECK=2 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -sFULL_ES3 -s DISABLE_EXCEPTION_CATCHING=0 -s SINGLE_FILE=0 -sSIDE_MODULE")

set_target_properties(mbgl-core-wasm PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/platform/emscripten/build)
