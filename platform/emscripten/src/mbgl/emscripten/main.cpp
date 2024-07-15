#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>

#include <mbgl/gfx/backend.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/style/style.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    const double lat = 0;
    const double lon = 0;
    const double zoom = 0;
    const double bearing = 0;
    const double pitch = 0;
    const double pixelRatio = 1;

    const uint32_t width = 512;
    const uint32_t height = 512;
    const std::string output = "out.png";
    const std::string cache_file = "cache.sqlite";
    const std::string asset_root = ".";

    // Try to load the apikey from the environment.
    const std::string apikey = std::string();

    const bool debug = false;

    using namespace mbgl;

    auto mapTilerConfiguration = mbgl::TileServerOptions::MapLibreConfiguration();
    std::string style = mapTilerConfiguration.defaultStyles().at(0).getUrl();

    util::RunLoop loop;

    HeadlessFrontend frontend({width, height}, static_cast<float>(pixelRatio));
    Map map(frontend,
            MapObserver::nullObserver(),
            MapOptions()
                .withMapMode(MapMode::Static)
                .withSize(frontend.getSize())
                .withPixelRatio(static_cast<float>(pixelRatio)),
            ResourceOptions()
                .withCachePath(cache_file)
                .withAssetPath(asset_root)
                .withApiKey(apikey)
                .withTileServerOptions(mapTilerConfiguration));

    // if (style.find("://") == std::string::npos) {
    //     style = std::string("file://") + style;
    // }

    map.getStyle().loadURL(style);
    map.jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom).withBearing(bearing).withPitch(pitch));

    if (debug) {
        map.setDebug(debug ? mbgl::MapDebugOptions::TileBorders | mbgl::MapDebugOptions::ParseStatus
                           : mbgl::MapDebugOptions::NoDebug);
    }

    try {
        // std::ofstream out(output, std::ios::binary);
        auto output = encodePNG(frontend.render(map).image);
        // out.close();
        printf("Output: %s\n", output.length());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // exit(1);
    }

    return 0;
}
