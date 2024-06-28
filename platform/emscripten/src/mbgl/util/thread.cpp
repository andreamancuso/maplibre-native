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
    char name[32] = "unknown";

    return name;
}

void setCurrentThreadName(const std::string& name) {

}

void makeThreadLowPriority() {

}

void setCurrentThreadPriority(double priority) {
    
}

void attachThread() {}

void detachThread() {}

} // namespace platform
} // namespace mbgl
