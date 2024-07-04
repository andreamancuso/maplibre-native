#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/monotonic_timer.hpp>
#include <mbgl/util/run_loop.hpp>
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

        printf("RunLoop::RunLoop(Type type)\n");

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

    if (impl->type == Type::Default) {
        return;
    }

    // Run the loop again to ensure that async
    // close callbacks have been called. Not needed
    // for the default main loop because it is only
    // closed when the application exits.
    impl->async.reset();
    runOnce();

    
}

LOOP_HANDLE RunLoop::getLoopHandle() {
    printf("RunLoop::getLoopHandle()\n");
}

void RunLoop::wake() {
    printf("RunLoop::wake()\n");
}

void RunLoop::run() {
    MBGL_VERIFY_THREAD(tid);

    printf("RunLoop::run()\n");
}

void RunLoop::runOnce() {
    MBGL_VERIFY_THREAD(tid);

    printf("RunLoop::runOnce()\n");
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

void RunLoop::addWatch(int fd, Event event, std::function<void(int, Event)>&& callback) {
    MBGL_VERIFY_THREAD(tid);

    printf("RunLoop::addWatch()\n");
}

void RunLoop::removeWatch(int fd) {
    MBGL_VERIFY_THREAD(tid);

    printf("RunLoop::removeWatch()\n");
}

} // namespace util
} // namespace mbgl
