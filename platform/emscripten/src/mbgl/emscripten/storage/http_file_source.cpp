#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/client_options.hpp>
#include <mbgl/util/logging.hpp>

#include <mbgl/util/util.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/timer.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/http_header.hpp>

#include <dlfcn.h>
#include <queue>
#include <map>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <optional>

namespace mbgl {

class HTTPFileSource::Impl {
public:
    Impl(const ResourceOptions &resourceOptions_, const ClientOptions &clientOptions_);
    ~Impl();

    void setResourceOptions(ResourceOptions options);
    ResourceOptions getResourceOptions();

    void setClientOptions(ClientOptions options);
    ClientOptions getClientOptions();

private:
    mutable std::mutex resourceOptionsMutex;
    mutable std::mutex clientOptionsMutex;
    ResourceOptions resourceOptions;
    ClientOptions clientOptions;
};

HTTPFileSource::Impl::Impl(const ResourceOptions &resourceOptions_, const ClientOptions &clientOptions_)
    : resourceOptions(resourceOptions_.clone()),
      clientOptions(clientOptions_.clone()) {
    
}

HTTPFileSource::Impl::~Impl() {
   
}

void HTTPFileSource::Impl::setResourceOptions(ResourceOptions options) {
    // std::lock_guard<std::mutex> lock(resourceOptionsMutex);
    resourceOptions = options;
}

ResourceOptions HTTPFileSource::Impl::getResourceOptions() {
    // std::lock_guard<std::mutex> lock(resourceOptionsMutex);
    return resourceOptions.clone();
}

void HTTPFileSource::Impl::setClientOptions(ClientOptions options) {
    // std::lock_guard<std::mutex> lock(clientOptionsMutex);
    clientOptions = options;
}

ClientOptions HTTPFileSource::Impl::getClientOptions() {
    // std::lock_guard<std::mutex> lock(clientOptionsMutex);
    return clientOptions.clone();
}

HTTPFileSource::HTTPFileSource(const ResourceOptions &resourceOptions, const ClientOptions &clientOptions)
    : impl(std::make_unique<Impl>(resourceOptions, clientOptions)) {}

HTTPFileSource::~HTTPFileSource() = default;

std::unique_ptr<AsyncRequest> HTTPFileSource::request(const Resource &resource, Callback callback) {
    printf("HTTPFileSource::request()\n");
}

void HTTPFileSource::setResourceOptions(ResourceOptions options) {
    impl->setResourceOptions(options.clone());
}

ResourceOptions HTTPFileSource::getResourceOptions() {
    return impl->getResourceOptions();
}

void HTTPFileSource::setClientOptions(ClientOptions options) {
    impl->setClientOptions(options.clone());
}

ClientOptions HTTPFileSource::getClientOptions() {
    return impl->getClientOptions();
}

} // namespace mbgl
