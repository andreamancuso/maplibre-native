#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/thread_local.hpp>
#ifdef __EMSCRIPTEN__
#include <mbgl/emscripten/util/thread_pool.hpp>
#else
#include <mbgl/util/thread_pool.hpp>
#endif
namespace mbgl {

std::function<void()> Scheduler::bindOnce(std::function<void()> fn) {
    assert(fn);
    return [scheduler = makeWeakPtr(), scheduled = std::move(fn)]() mutable {
        if (!scheduled) return; // Repeated call.
        auto schedulerGuard = scheduler.lock();
        if (scheduler) scheduler->schedule(std::move(scheduled));
    };
}

static auto& current() {
    static util::ThreadLocal<Scheduler> scheduler;
    return scheduler;
};

void Scheduler::SetCurrent(Scheduler* scheduler) {
    current().set(scheduler);
}

Scheduler* Scheduler::GetCurrent() {
    return current().get();
}

// static
std::shared_ptr<Scheduler> Scheduler::GetBackground() {
    printf("Scheduler::GetBackground() a\n");
    static std::weak_ptr<Scheduler> weak;
    printf("Scheduler::GetBackground() b\n");
    static std::mutex mtx;
    printf("Scheduler::GetBackground() c\n");
    std::lock_guard<std::mutex> lock(mtx);
    printf("Scheduler::GetBackground() d\n");
    std::shared_ptr<Scheduler> scheduler = weak.lock();
    printf("Scheduler::GetBackground() e\n");
    if (!scheduler) {
        printf("Scheduler::GetBackground() f\n");
        weak = scheduler = std::make_shared<ThreadPool>();
        printf("Scheduler::GetBackground() g\n");
    }

    printf("Scheduler::GetBackground() h\n");

    return scheduler;
}

// static
std::shared_ptr<Scheduler> Scheduler::GetSequenced() {
    const std::size_t kSchedulersCount = 10;
    static std::vector<std::weak_ptr<Scheduler>> weaks(kSchedulersCount);
    static std::mutex mtx;
    static std::size_t lastUsedIndex = 0u;

    std::lock_guard<std::mutex> lock(mtx);

    if (++lastUsedIndex == kSchedulersCount) lastUsedIndex = 0u;

    std::shared_ptr<Scheduler> result;
    for (std::size_t i = 0; i < kSchedulersCount; ++i) {
        auto& weak = weaks[i];
        if (auto scheduler = weak.lock()) {
            if (lastUsedIndex == i) result = scheduler;
            continue;
        }
        result = std::make_shared<SequencedScheduler>();
        weak = result;
        lastUsedIndex = i;
        break;
    }

    return result;
}

} // namespace mbgl
