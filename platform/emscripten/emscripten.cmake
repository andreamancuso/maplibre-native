if(NOT DEFINED ENV{EMSDK})
    message(FATAL_ERROR "Environment variable EMSDK is not defined. Follow the README steps and source emsdk_env.* from the EMSDK root directory in this shell before running CMake")
endif()

find_package(ICU OPTIONAL_COMPONENTS i18n uc)
find_package(JPEG REQUIRED)
find_package(libuv CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(WebP REQUIRED)
find_path(DLFCN_INCLUDE_DIRS dlfcn.h)
# find_path(LIBUV_INCLUDE_DIRS uv.h)
find_package(ZLIB REQUIRED)

target_sources(
    mbgl-core
    PRIVATE
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
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/util/run_loop.cpp
        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/util/timer.cpp

        # GLFW
        ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_view.cpp
        ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_renderer_frontend.cpp
        ${PROJECT_SOURCE_DIR}/platform/glfw/settings_json.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/map/map_snapshotter.cpp

        ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_gl_backend.cpp

        ${PROJECT_SOURCE_DIR}/platform/emscripten/src/mbgl/headless_backend_glfw.cpp
        ${PROJECT_SOURCE_DIR}/platform/windows/src/gl_functions.cpp
)

# GLFW
set_property(
    SOURCE ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_view.cpp
    PROPERTY COMPILE_DEFINITIONS MAPBOX_PUCK_ASSETS_PATH=\"${PROJECT_SOURCE_DIR}/platform/glfw/assets/\"
)

target_compile_definitions(
    mbgl-core
    PRIVATE MLN_RENDER_BACKEND_OPENGL=1
)

# # FIXME: Should not be needed, but now needed by node because of the headless frontend.
target_include_directories(
    mbgl-core
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/platform/default/include
    # PRIVATE
        ${PROJECT_SOURCE_DIR}/platform/windows/include
        # ${CURL_INCLUDE_DIRS}
		# ${DLFCN_INCLUDE_DIRS}
        ${JPEG_INCLUDE_DIRS}
        # ${LIBUV_INCLUDE_DIRS}
        ${WEBP_INCLUDE_DIRS}
)

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

target_link_libraries(
    mbgl-core
    PRIVATE
        # GLFW
        ${GLFW_INCLUDE_DIRS}
        ${GLFW_LIBRARIES}

        ${LIBUV_LIBRARIES}
        ${WEBP_LIBRARIES}
        $<$<NOT:$<BOOL:${MLN_USE_BUILTIN_ICU}>>:ICU::data>
        $<$<NOT:$<BOOL:${MLN_USE_BUILTIN_ICU}>>:ICU::i18n>
        $<$<NOT:$<BOOL:${MLN_USE_BUILTIN_ICU}>>:ICU::uc>
        $<$<BOOL:${MLN_USE_BUILTIN_ICU}>:mbgl-vendor-icu>
        PNG::PNG
        mbgl-vendor-nunicode
        ZLIB::ZLIB
        mbgl-vendor-sqlite
        $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>
)
