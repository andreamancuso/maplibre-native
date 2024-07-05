#pragma once

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/thread_local.hpp>
#include <mbgl/util/containers.hpp>
#include <mbgl/util/identity.hpp>

#include <pthread.h>
#include <algorithm>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace mbgl {

// Task queues bucketed by tag address
struct Queue {
    std::atomic<std::size_t> runningCount;   /* running tasks */
    std::condition_variable cv;              /* queue empty condition */
    std::mutex lock;                         /* lock */
    std::queue<std::function<void()>> queue; /* pending task queue */
};

class ThreadedSchedulerBase : public Scheduler {
public:
    /// @brief Schedule a generic task not assigned to any particular owner.
    /// The scheduler itself will own the task.
    /// @param fn Task to run
    void schedule(std::function<void()>&& fn) override;

    /// @brief Schedule a task assigned to the given owner `tag`.
    /// @param tag Identifier object to indicate ownership of `fn`
    /// @param fn Task to run
    void schedule(const util::SimpleIdentity tag, std::function<void()>&& fn) override;
    const util::SimpleIdentity uniqueID;
    util::ThreadLocal<ThreadedSchedulerBase> owningThreadPool;
    std::mutex workerMutex;
    bool terminated{false};
    std::atomic<size_t> taskCount{0};
    std::mutex taggedQueueLock;
    // Signal when an item is added to the queue
    std::condition_variable cvAvailable;
    mbgl::unordered_map<util::SimpleIdentity, std::shared_ptr<Queue>> taggedQueue;

protected:
    ThreadedSchedulerBase() = default;
    ~ThreadedSchedulerBase() override;

    void terminate();
    int makeSchedulerThread(pthread_t& thread, size_t index);

    /// @brief Wait until there's nothing pending or in process
    /// Must not be called from a task provided to this scheduler.
    /// @param tag Tag of the owner to identify the collection of tasks to
    // wait for. Not providing a tag waits on tasks owned by the scheduler.
    void waitForEmpty(const util::SimpleIdentity = util::SimpleIdentity::Empty) override;

    /// Returns true if called from a thread managed by the scheduler
    bool thisThreadIsOwned() const { return owningThreadPool.get() == this; }

    
};

/**
 * @brief ThreadScheduler implements Scheduler interface using a lightweight event loop
 *
 * @tparam N number of threads
 *
 * Note: If N == 1 all scheduled tasks are guaranteed to execute consequently;
 * otherwise, some of the scheduled tasks might be executed in parallel.
 */
class ThreadedScheduler : public ThreadedSchedulerBase {
public:
    ThreadedScheduler(std::size_t n)
        : m_threads(n) {
        for (std::size_t i = 0u; i < n; ++i) {
            pthread_t thread;
            makeSchedulerThread(thread, i);
            m_threads.push_back(thread);
        }
    }

    ~ThreadedScheduler() override {
        assert(!thisThreadIsOwned());
        terminate();
        for (auto& thread : m_threads) {
            // todo: what should happen here?

            // assert(std::this_thread::get_id() != thread.get_id());
            // thread.join();
        }
    }

    void runOnRenderThread(const util::SimpleIdentity tag, std::function<void()>&& fn) override {
        printf("ThreadedScheduler::runOnRenderThread()\n");

        std::shared_ptr<RenderQueue> queue;
        {
            std::lock_guard<std::mutex> lock(taggedRenderQueueLock);
            auto it = taggedRenderQueue.find(tag);
            if (it != taggedRenderQueue.end()) {
                queue = it->second;
            } else {
                queue = std::make_shared<RenderQueue>();
                taggedRenderQueue.insert({tag, queue});
            }
        }

        std::lock_guard<std::mutex> lock(queue->mutex);
        queue->queue.push(std::move(fn));
    }

    void runRenderJobs(const util::SimpleIdentity tag, bool closeQueue = false) override {
        printf("ThreadedScheduler::runRenderJobs()\n");

        std::shared_ptr<RenderQueue> queue;
        std::unique_lock<std::mutex> lock(taggedRenderQueueLock);

        {
            auto it = taggedRenderQueue.find(tag);
            if (it != taggedRenderQueue.end()) {
                queue = it->second;
            }

            if (!closeQueue) {
                lock.unlock();
            }
        }

        if (!queue) {
            return;
        }

        std::lock_guard<std::mutex> taskLock(queue->mutex);
        while (queue->queue.size()) {
            auto fn = std::move(queue->queue.front());
            queue->queue.pop();
            if (fn) {
                fn();
            }
        }

        if (closeQueue) {
            // We hold both locks and can safely remove the queue entry
            taggedRenderQueue.erase(tag);
        }
    }

    mapbox::base::WeakPtr<Scheduler> makeWeakPtr() override { return weakFactory.makeWeakPtr(); }

private:
    std::vector<pthread_t> m_threads;

    struct RenderQueue {
        std::queue<std::function<void()>> queue;
        std::mutex mutex;
    };
    mbgl::unordered_map<util::SimpleIdentity, std::shared_ptr<RenderQueue>> taggedRenderQueue;
    std::mutex taggedRenderQueueLock;

    mapbox::base::WeakPtrFactory<Scheduler> weakFactory{this};
    // Do not add members here, see `WeakPtrFactory`
};

class SequencedScheduler : public ThreadedScheduler {
public:
    SequencedScheduler()
        : ThreadedScheduler(1) {}
};

class ParallelScheduler : public ThreadedScheduler {
public:
    ParallelScheduler(std::size_t extra)
        : ThreadedScheduler(1 + extra) {}
};

class ThreadPool : public ParallelScheduler {
public:
    ThreadPool()
        : ParallelScheduler(3) {}
};

} // namespace mbgl
