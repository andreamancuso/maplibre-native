#include <mbgl/util/thread_pool.hpp>

#include <mbgl/platform/settings.hpp>
#include <mbgl/platform/thread.hpp>
#include <mbgl/util/monotonic_timer.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>

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

std::thread ThreadedSchedulerBase::makeSchedulerThread(size_t index) {
    return std::thread([this, index] {
        
    });
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
