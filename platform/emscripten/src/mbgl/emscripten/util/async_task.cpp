#include <mbgl/util/async_task.hpp>

#include <mbgl/util/run_loop.hpp>

#include <emscripten/console.h>
#include <pthread.h>
#include <atomic>
#include <functional>
#include <stdexcept>

// void *AsyncTaskThread(void *arg);

namespace mbgl {
namespace util {

class AsyncTask::Impl {
public:
    explicit Impl(std::function<void()> fn)
        : m_task(std::move(fn)) {}

    ~Impl() {
        
    }

    void maySend() {
        printf("AsyncTask::maySend()\n");
        // int rc = pthread_create(&m_thread, NULL, AsyncTaskThread, static_cast<void*>(this));
        // assert(rc == 0);

        m_task();
    }

    std::function<void()> m_task;

private:
    pthread_t m_thread;
};

AsyncTask::AsyncTask(std::function<void()>&& fn)
    : impl(std::make_unique<Impl>(std::move(fn))) {}

AsyncTask::~AsyncTask() = default;

void AsyncTask::send() {
    printf("AsyncTask::send()\n");
    impl->maySend();
    // m_task();
}

} // namespace util
} // namespace mbgl

// void *AsyncTaskThread(void *arg) {
//     emscripten_console_log("AsyncTaskThread()\n");
//     auto asyncTask = static_cast<mbgl::util::AsyncTask*>(arg);

//     emscripten_console_log("AsyncTaskThread() about to invoke task\n");
//     asyncTask->m_task();
//     emscripten_console_log("AsyncTaskThread() invoked task\n");

//     pthread_exit((void*)0);
// }