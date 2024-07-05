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

        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/storage/http_file_source.cpp

        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/util/work_task.hpp
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/util/work_task_impl.hpp

        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/util/run_loop.hpp
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/util/run_loop.cpp

        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/util/thread.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/thread_local.cpp

        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/util/thread_pool.hpp
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/util/thread_pool.cpp
        
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/util/async_task.cpp
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/util/timer.cpp
        
        # ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/util/logging.cpp

        # GLFW
        # ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_view.cpp
        # ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_renderer_frontend.cpp
        # ${PROJECT_SOURCE_DIR}/platform/glfw/settings_json.cpp
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/map/map_snapshotter.cpp

        # ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_gl_backend.cpp

        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/emscripten/headless_backend_glfw.cpp
        ${PROJECT_SOURCE_DIR}/platform/windows/src/gl_functions.cpp
)

add_library(mbgl-core STATIC)

target_compile_definitions(
    mbgl-core
    PRIVATE MLN_RENDER_BACKEND_OPENGL=1
    PUBLIC
        "MLN_LEGACY_RENDERER=$<BOOL:${MLN_LEGACY_RENDERER}>"
        "MLN_DRAWABLE_RENDERER=$<BOOL:${MLN_DRAWABLE_RENDERER}>"
        "MLN_USE_UNORDERED_DENSE=$<BOOL:${MLN_USE_UNORDERED_DENSE}>"
)

target_sources(
        mbgl-core PRIVATE
        ${INCLUDE_FILES}
        ${SRC_FILES}
        ${EXTRA_SRC_FILES}
    )

target_include_directories(
    mbgl-core
    PRIVATE ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/vendor/libwebp/src
)

target_include_directories(
    mbgl-core
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/platform/windows/include
    ${PROJECT_SOURCE_DIR}/platform/emscripten/src
    ${PROJECT_SOURCE_DIR}/platform/default/include
)

target_link_libraries(mbgl-core
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

set_target_properties(mbgl-core PROPERTIES COMPILE_FLAGS "-O3 -pthread --use-port=libjpeg --use-port=sqlite3 --use-port=zlib --use-port=libpng")
set_target_properties(mbgl-core PROPERTIES LINK_FLAGS "-O3 -pthread --use-port=libjpeg --use-port=zlib --use-port=sqlite3 --use-port=libpng -s USE_GLFW=3 -s USE_WEBGPU=1 -s FULL_ES3=1 -sOFFSCREENCANVAS_SUPPORT -s EXPORT_ALL=1")

