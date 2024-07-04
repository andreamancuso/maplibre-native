#include <mbgl/util/async_task.hpp>

#include <mbgl/util/run_loop.hpp>

#include <pthread.h>
#include <atomic>
#include <functional>
#include <stdexcept>

void *AsyncTaskThread(void *arg);

namespace mbgl {
namespace util {

class AsyncTask::Impl {
public:
    explicit Impl(std::function<void()> fn)
        : m_task(std::move(fn)) {}

    ~Impl() {
        
    }

    void maySend() {
        int rc = pthread_create(&m_thread, NULL, AsyncTaskThread, static_cast<void*>(this));
    }

    std::function<void()> m_task;

private:
    pthread_t m_thread;
};

AsyncTask::AsyncTask(std::function<void()>&& fn)
    : impl(std::make_unique<Impl>(std::move(fn))) {}

AsyncTask::~AsyncTask() = default;

void AsyncTask::send() {
    impl->maySend();
}

} // namespace util
} // namespace mbgl

void *AsyncTaskThread(void *arg) {
    auto asyncTask = static_cast<mbgl::util::AsyncTask*>(arg);

    asyncTask->m_task();

    pthread_exit((void*)0);
}