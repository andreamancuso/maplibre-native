#include <mbgl/util/timer.hpp>

#include <mbgl/util/string.hpp>

#include <stdexcept>

namespace mbgl {
namespace util {

class Timer::Impl {
public:
    Impl() {
        printf("Timer::Impl::Impl()\n");
        // auto* loop = reinterpret_cast<uv_loop_t*>(RunLoop::getLoopHandle());
        // if (uv_timer_init(loop, timer) != 0) {
        //     throw std::runtime_error("Failed to initialize timer.");
        // }

        // handle()->data = this;
        // uv_unref(handle());
    }

    ~Impl() {
        // uv_close(handle(), [](uv_handle_t* h) { delete reinterpret_cast<uv_timer_t*>(h); });
    }

    void start(uint64_t timeout, uint64_t repeat, std::function<void()>&& cb_) {
        printf("Timer::start()\n");
        cb = std::move(cb_);

        // Update the event loop’s concept of “now”.
        // This resolves test failures on some systems, where only the following sequence of tests fails:
        //     Thread.InvokeBeforeChildStarts:Thread.DeleteBeforeChildStarts:Timer.Basic
        // This implies that "you have callbacks that block the event loop for ... on the order of a millisecond
        // or more," so this may be masking some unexpected interaction between successive `RunLoop` instances.
        // uv_update_time(reinterpret_cast<uv_loop_t*>(RunLoop::getLoopHandle()));

        // if (uv_timer_start(timer, timerCallback, timeout, repeat) != 0) {
        //     throw std::runtime_error("Failed to start timer.");
        // }
    }

    void stop() {
        printf("Timer::stop()\n");
        cb = nullptr;
        // if (uv_timer_stop(timer) != 0) {
        //     throw std::runtime_error("Failed to stop timer.");
        // }
    }

private:
    // static void timerCallback(uv_timer_t* handle) { reinterpret_cast<Impl*>(handle->data)->cb(); }

    // uv_handle_t* handle() { return reinterpret_cast<uv_handle_t*>(timer); }

    // uv_timer_t* timer;

    std::function<void()> cb;
};

Timer::Timer()
    : impl(std::make_unique<Impl>()) {}

Timer::~Timer() = default;

void Timer::start(Duration timeout, Duration repeat, std::function<void()>&& cb) {
    impl->start(static_cast<uint64_t>(std::chrono::duration_cast<Milliseconds>(timeout).count()),
                static_cast<uint64_t>(std::chrono::duration_cast<Milliseconds>(repeat).count()),
                std::move(cb));
}

void Timer::stop() {
    impl->stop();
}

} // namespace util
} // namespace mbgl