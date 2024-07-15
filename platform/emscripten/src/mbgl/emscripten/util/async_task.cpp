#include <mbgl/util/async_task.hpp>

#include <mbgl/util/run_loop.hpp>

#include <emscripten/console.h>
#include <pthread.h>
#include <atomic>
#include <functional>
#include <stdexcept>

void *AsyncTaskThread(void *arg) {
    auto asyncTask = static_cast<mbgl::util::AsyncTask*>(arg);

    emscripten_console_log("AsyncTaskThread() about to invoke task\n");
    asyncTask->m_task();
    emscripten_console_log("AsyncTaskThread() invoked task\n");

    return 0;
}

namespace mbgl {
namespace util {

AsyncTask::AsyncTask(std::function<void()>&& fn)
    : m_task(std::move(fn)) {}

AsyncTask::~AsyncTask() = default;

void AsyncTask::send() {
    m_task();
    // int rc = pthread_create(&m_thread, NULL, AsyncTaskThread, static_cast<void*>(this));
    // assert(rc == 0);


}

} // namespace util
} // namespace mbgl

