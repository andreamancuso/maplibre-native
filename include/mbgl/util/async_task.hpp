#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <functional>

namespace mbgl {
namespace util {

class AsyncTask : private util::noncopyable {
public:
    AsyncTask(std::function<void()>&&);
    ~AsyncTask();

    void send();

    std::function<void()> m_task;

private:
    pthread_t m_thread;
};

} // namespace util
} // namespace mbgl
