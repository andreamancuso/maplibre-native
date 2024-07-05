#include <mbgl/util/thread_local.hpp>
#include <mbgl/util/logging.hpp>

#include <cassert>
#include <cstdlib>

namespace mbgl {
namespace util {
namespace impl {

ThreadLocalBase::ThreadLocalBase() {
    printf("ThreadLocalBase::ThreadLocalBase()\n");
}

ThreadLocalBase::~ThreadLocalBase() {
    
}

void* ThreadLocalBase::get() const {
    printf("ThreadLocalBase::get()\n");
}

void ThreadLocalBase::set(void* ptr) {
    printf("ThreadLocalBase::set()\n");
}

} // namespace impl
} // namespace util
} // namespace mbgl
