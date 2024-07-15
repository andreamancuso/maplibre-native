#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/util/client_options.hpp>
#include <mbgl/util/string.hpp>

#include <algorithm>
#include <list>
#include <map>
#include <mutex>
#include <tuple>

namespace mbgl {

struct FileSourceInfo {
    FileSourceInfo(FileSourceType type_, std::string id_, std::weak_ptr<FileSource> fileSource_)
        : type(type_),
          id(std::move(id_)),
          fileSource(std::move(fileSource_)) {}

    FileSourceType type;
    std::string id;
    std::weak_ptr<FileSource> fileSource;
};

class FileSourceManager::Impl {
public:
    std::list<FileSourceInfo> fileSources;
    std::map<FileSourceType, FileSourceFactory> fileSourceFactories;
    std::recursive_mutex mutex;
};

FileSourceManager::FileSourceManager()
    : impl(std::make_unique<Impl>()) {}

FileSourceManager::~FileSourceManager() = default;

std::shared_ptr<FileSource> FileSourceManager::getFileSource(FileSourceType type,
                                                             const ResourceOptions& resourceOptions,
                                                             const ClientOptions& clientOptions) noexcept {
    // printf("FileSourceManager::getFileSource() a\n");
    std::lock_guard<std::recursive_mutex> lock(impl->mutex);
    // printf("FileSourceManager::getFileSource() b\n");

    // Remove released file sources.
    for (auto it = impl->fileSources.begin(); it != impl->fileSources.end();) {
        // printf("FileSourceManager::getFileSource() c\n");
        it = it->fileSource.expired() ? impl->fileSources.erase(it) : ++it;
    }

    // printf("FileSourceManager::getFileSource() d\n");
    const auto context = reinterpret_cast<uint64_t>(resourceOptions.platformContext());
    // printf("FileSourceManager::getFileSource() e\n");
    std::string baseURL = resourceOptions.tileServerOptions().baseURL();
    // printf("FileSourceManager::getFileSource() f, baseURL: %s\n", baseURL.c_str());
    std::string id = baseURL + '|' + resourceOptions.apiKey() + '|' + resourceOptions.cachePath() + '|' +
                     util::toString(context);
    // printf("FileSourceManager::getFileSource() g, id: %s\n", id.c_str());

    std::shared_ptr<FileSource> fileSource;
    // printf("FileSourceManager::getFileSource() h\n");
    auto fileSourceIt = std::find_if(impl->fileSources.begin(), impl->fileSources.end(), [type, &id](const auto& info) {
        // printf("FileSourceManager::getFileSource() i\n");
        return info.type == type && info.id == id;
    });
    // printf("FileSourceManager::getFileSource() l\n");
    if (fileSourceIt != impl->fileSources.end()) {
        // printf("FileSourceManager::getFileSource() m\n");
        fileSource = fileSourceIt->fileSource.lock();
        // printf("FileSourceManager::getFileSource() n\n");
    }

    // printf("FileSourceManager::getFileSource() o\n");

    if (!fileSource) {
        // printf("FileSourceManager::getFileSource() p\n");
        auto it = impl->fileSourceFactories.find(type);
        // printf("FileSourceManager::getFileSource() q\n");
        if (it != impl->fileSourceFactories.end()) {
            // printf("FileSourceManager::getFileSource() r\n");
            assert(it->second);
            // printf("FileSourceManager::getFileSource() s\n");
            fileSource = it->second(resourceOptions, clientOptions);
            // printf("FileSourceManager::getFileSource() t\n");
            impl->fileSources.emplace_back(type, std::move(id), fileSource);
            // printf("FileSourceManager::getFileSource() u\n");
        }
    }

    // printf("FileSourceManager::getFileSource() v\n");

    return fileSource;
}

void FileSourceManager::registerFileSourceFactory(FileSourceType type, FileSourceFactory&& factory) noexcept {
    assert(factory);
    std::lock_guard<std::recursive_mutex> lock(impl->mutex);
    impl->fileSourceFactories[type] = std::move(factory);
}

FileSourceManager::FileSourceFactory FileSourceManager::unRegisterFileSourceFactory(FileSourceType type) noexcept {
    std::lock_guard<std::recursive_mutex> lock(impl->mutex);
    auto it = impl->fileSourceFactories.find(type);
    FileSourceFactory factory;
    if (it != impl->fileSourceFactories.end()) {
        factory = std::move(it->second);
        impl->fileSourceFactories.erase(it);
    }
    return factory;
}

} // namespace mbgl
