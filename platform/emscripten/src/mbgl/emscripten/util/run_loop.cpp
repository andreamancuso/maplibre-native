#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/monotonic_timer.hpp>
#include <mbgl/emscripten/util/run_loop.hpp>
#include <mbgl/util/thread_local.hpp>

#include <cassert>
#include <functional>
#include <unordered_map>
#include <stdexcept>

namespace mbgl {
namespace util {

RunLoop* RunLoop::Get() {
    assert(static_cast<RunLoop*>(Scheduler::GetCurrent()));
    return static_cast<RunLoop*>(Scheduler::GetCurrent());
}

class RunLoop::Impl {
public:
    RunLoop::Type type;
    std::unique_ptr<AsyncTask> async;
};

RunLoop::RunLoop(Type type)
    : impl(std::make_unique<Impl>()) {
    switch (type) {
        case Type::New:
            
            break;
        case Type::Default:
            
            break;
    }

    impl->type = type;

    Scheduler::SetCurrent(this);

    impl->async = std::make_unique<AsyncTask>(std::bind(&RunLoop::process, this));
}

RunLoop::~RunLoop() {
    Scheduler::SetCurrent(nullptr);
}

LOOP_HANDLE RunLoop::getLoopHandle() {
    throw std::runtime_error("Should not be used in emscripten.");

    return nullptr;
}

void RunLoop::wake() {
    printf("RunLoop::wake()\n");

    impl->async->send();
}

void RunLoop::run() {
    MBGL_VERIFY_THREAD(tid);

    printf("RunLoop::run()\n");

    wake();

    if (impl->type == Type::Default) {
        // QCoreApplication::instance()->exec();
    } else {
        // impl->loop->exec();
    }
}

void RunLoop::runOnce() {
    MBGL_VERIFY_THREAD(tid);

    printf("RunLoop::runOnce()\n");

    wake();

    if (impl->type == Type::Default) {
        // QCoreApplication::instance()->processEvents();
    } else {
        // impl->loop->processEvents();
    }
}

void RunLoop::stop() {
    printf("RunLoop::stop()\n");
}

void RunLoop::waitForEmpty([[maybe_unused]] const mbgl::util::SimpleIdentity tag) {
    printf("RunLoop::waitForEmpty()\n");

    while (true) {
        std::size_t remaining;
        {
            std::lock_guard<std::mutex> lock(mutex);
            remaining = defaultQueue.size() + highPriorityQueue.size();
        }

        if (remaining == 0) {
            return;
        }

        runOnce();
    }
}

} // namespace util
} // namespace mbgl
