#include <emscripten.h>
#include <emscripten/console.h>
#include <pthread.h>
#include <tuple>

#include <mbgl/emscripten/util/thread_pool.hpp>

#include <mbgl/platform/settings.hpp>
#include <mbgl/platform/thread.hpp>
#include <mbgl/util/monotonic_timer.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>

void *ThreadMain(void *arg) {
    auto threadScheduler = static_cast<mbgl::ThreadedScheduler*>(arg);

    auto& settings = mbgl::platform::Settings::getInstance();
    auto value = settings.get(mbgl::platform::EXPERIMENTAL_THREAD_PRIORITY_WORKER);

    if (auto* priority = value.getDouble()) {
        mbgl::platform::setCurrentThreadPriority(*priority);
    }

    // mbgl::platform::setCurrentThreadName("Worker " + mbgl::util::toString(index + 1));
    mbgl::platform::setCurrentThreadName("Worker ???");
    mbgl::platform::attachThread();

    threadScheduler->owningThreadPool.set(threadScheduler);

    while (true) {
        std::unique_lock<std::mutex> conditionLock(threadScheduler->workerMutex);
        if (!threadScheduler->terminated && threadScheduler->taskCount == 0) {
            threadScheduler->cvAvailable.wait(conditionLock);
        }

        if (threadScheduler->terminated) {
            // pthread_exit((void*)0);
            mbgl::platform::detachThread();
            break;
        }

        // Let other threads run
        conditionLock.unlock();

        std::vector<std::shared_ptr<mbgl::Queue>> pending;
        {
            // 1. Gather buckets for us to visit this iteration
            std::lock_guard<std::mutex> lock(threadScheduler->taggedQueueLock);
            for (const auto& [tag, queue] : threadScheduler->taggedQueue) {
                pending.push_back(queue);
            }
        }

        // 2. Visit a task from each
        for (auto& q : pending) {
            std::function<void()> tasklet;
            {
                std::lock_guard<std::mutex> lock(q->lock);
                if (q->queue.size()) {
                    q->runningCount++;
                    tasklet = std::move(q->queue.front());
                    q->queue.pop();
                }
                if (!tasklet) continue;
            }

            assert(threadScheduler->taskCount > 0);
            threadScheduler->taskCount--;

            try {
                tasklet();
                tasklet = {}; // destroy the function and release its captures before unblocking `waitForEmpty`

                if (!--q->runningCount) {
                    std::lock_guard<std::mutex> lock(q->lock);
                    if (q->queue.empty()) {
                        q->cv.notify_all();
                    }
                }
            } catch (...) {
                std::lock_guard<std::mutex> lock(q->lock);
                if (threadScheduler->handler) {
                    threadScheduler->handler(std::current_exception());
                }

                tasklet = {};

                if (!--q->runningCount && q->queue.empty()) {
                    q->cv.notify_all();
                }

                if (threadScheduler->handler) {
                    continue;
                }
                throw;
            }
        }
    }
}

namespace mbgl {

ThreadedSchedulerBase::~ThreadedSchedulerBase() = default;

void ThreadedSchedulerBase::terminate() {
    {
        std::lock_guard<std::mutex> lock(workerMutex);
        terminated = true;
    }

    // Wake up all threads so that they shut down
    cvAvailable.notify_all();
}

int ThreadedSchedulerBase::makeSchedulerThread(pthread_t& thread, size_t index) {
    auto lambda = []() {
        emscripten_console_log("inside ThreadMain inside lambda\n");
    };

    return pthread_create(&thread, NULL, ThreadMain, static_cast<void*>(this));
}

void ThreadedSchedulerBase::schedule(std::function<void()>&& fn) {
    schedule(uniqueID, std::move(fn));
}

void ThreadedSchedulerBase::schedule(const util::SimpleIdentity tag, std::function<void()>&& fn) {
    assert(fn);
    if (!fn) return;

    std::shared_ptr<Queue> q;
    {
        std::lock_guard<std::mutex> lock(taggedQueueLock);
        auto it = taggedQueue.find(tag);
        if (it == taggedQueue.end()) {
            q = std::make_shared<Queue>();
            taggedQueue.insert({tag, q});
        } else {
            q = it->second;
        }
    }

    {
        std::lock_guard<std::mutex> lock(q->lock);
        q->queue.push(std::move(fn));
        taskCount++;
    }

    cvAvailable.notify_one();
}

void ThreadedSchedulerBase::waitForEmpty(const util::SimpleIdentity tag) {
    // Must not be called from a thread in our pool, or we would deadlock
    assert(!thisThreadIsOwned());
    if (!thisThreadIsOwned()) {
        const auto tagToFind = tag.isEmpty() ? uniqueID : tag;

        std::shared_ptr<Queue> q;
        {
            std::lock_guard<std::mutex> lock(taggedQueueLock);
            auto it = taggedQueue.find(tagToFind);
            if (it == taggedQueue.end()) {
                return;
            }
            q = it->second;
        }

        std::unique_lock<std::mutex> queueLock(q->lock);
        while (q->queue.size() + q->runningCount) {
            q->cv.wait(queueLock);
        }

        // After waiting for the queue to empty, go ahead and erase it from the map.
        {
            std::lock_guard<std::mutex> lock(taggedQueueLock);
            taggedQueue.erase(tagToFind);
        }
    }
}

} // namespace mbgl
