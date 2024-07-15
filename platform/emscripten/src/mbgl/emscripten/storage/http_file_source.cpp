#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/client_options.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/timer.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/http_header.hpp>

#include <emscripten.h>
#include <dlfcn.h>
#include <queue>
#include <map>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <optional>

namespace mbgl {

class HTTPRequest : public AsyncRequest {
public:
    HTTPRequest(Resource, FileSource::Callback);
    ~HTTPRequest() override;

private:
    Resource resource;
    FileSource::Callback callback;
};

HTTPRequest::HTTPRequest(Resource resource_, FileSource::Callback callback_) 
    : resource(std::move(resource_)),
      callback(std::move(callback_)) {
    printf("HTTPRequest::HTTPRequest()\n");
}


HTTPRequest::~HTTPRequest() {
    
}

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
    printf("HTTPFileSource::Impl::Impl()\n");
}

HTTPFileSource::Impl::~Impl() {
   
}

void HTTPFileSource::Impl::setResourceOptions(ResourceOptions options) {
    std::lock_guard<std::mutex> lock(resourceOptionsMutex);
    resourceOptions = options;
}

ResourceOptions HTTPFileSource::Impl::getResourceOptions() {
    std::lock_guard<std::mutex> lock(resourceOptionsMutex);
    return resourceOptions.clone();
}

void HTTPFileSource::Impl::setClientOptions(ClientOptions options) {
    std::lock_guard<std::mutex> lock(clientOptionsMutex);
    clientOptions = options;
}

ClientOptions HTTPFileSource::Impl::getClientOptions() {
    std::lock_guard<std::mutex> lock(clientOptionsMutex);
    return clientOptions.clone();
}

HTTPFileSource::HTTPFileSource(const ResourceOptions &resourceOptions, const ClientOptions &clientOptions)
    : impl(std::make_unique<Impl>(resourceOptions, clientOptions)) {
        // EM_ASM(console.trace(););
        printf("HTTPFileSource::HTTPFileSource()\n");
    }

HTTPFileSource::~HTTPFileSource() = default;

std::unique_ptr<AsyncRequest> HTTPFileSource::request(const Resource &resource, Callback callback) {
    // EM_ASM(console.trace(););
    printf("HTTPFileSource::request()\n");

    return std::make_unique<HTTPRequest>(resource, callback);
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
