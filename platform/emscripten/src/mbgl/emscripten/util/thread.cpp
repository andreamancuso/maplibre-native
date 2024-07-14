#include <mbgl/util/platform.hpp>
#include <mbgl/platform/thread.hpp>
#include <mbgl/util/logging.hpp>

#include <string>

// #include <pthread.h>
// #include <sched.h>
// #include <sys/resource.h>

namespace mbgl {
namespace platform {

std::string getCurrentThreadName() {
    // printf("platform::getCurrentThreadName()\n");
    char name[32] = "unknown";

    return name;
}

void setCurrentThreadName(const std::string& name) {
    // printf("platform::setCurrentThreadName()\n");
}

void makeThreadLowPriority() {
    // printf("platform::makeThreadLowPriority()\n");
}

void setCurrentThreadPriority(double priority) {
    // printf("platform::setCurrentThreadPriority()\n");
}

void attachThread() {
    // printf("platform::attachThread()\n");
}

void detachThread() {
    // printf("platform::detachThread()\n");
}

} // namespace platform
} // namespace mbgl
