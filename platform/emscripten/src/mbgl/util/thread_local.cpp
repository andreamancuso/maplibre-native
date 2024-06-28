#include <mbgl/util/thread_local.hpp>
#include <mbgl/util/logging.hpp>

#include <cassert>
#include <cstdlib>

namespace mbgl {
namespace util {
namespace impl {

ThreadLocalBase::ThreadLocalBase() {
    
}

ThreadLocalBase::~ThreadLocalBase() {
    
}

void* ThreadLocalBase::get() const {
    
}

void ThreadLocalBase::set(void* ptr) {
    
}

} // namespace impl
} // namespace util
} // namespace mbgl
