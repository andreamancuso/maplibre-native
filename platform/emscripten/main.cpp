#include <emscripten.h>
#include <emscripten/bind.h>
#include <cstdint>
#include <stdio.h>
#include <mbgl/util/type_list.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
// #include <mbgl/gfx/headless_frontend.hpp>

int main(int argc, char* argv[]) {
    const double pixelRatio = 1;
    const uint32_t width = 512;
    const uint32_t height = 512;
    auto mapTilerConfiguration = mbgl::TileServerOptions::MapTilerConfiguration();
    mbgl::Size size{width, height};

    printf("mapTilerConfiguration.baseURL(): %s\n", mapTilerConfiguration.baseURL().c_str());

    // mbgl::HeadlessFrontend frontend(size, static_cast<float>(pixelRatio));

    // printf("frontend initialised\n");

    // mbgl::Map map(frontend,
    //         mbgl::MapObserver::nullObserver(),
    //         mbgl::MapOptions()
    //             .withMapMode(mbgl::MapMode::Static)
    //             .withSize(frontend.getSize())
    //             .withPixelRatio(static_cast<float>(pixelRatio)),
    //         mbgl::ResourceOptions()
    //             .withTileServerOptions(mapTilerConfiguration));

    return 0;
}

void _exit() {
    
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("exit", &_exit);
}
