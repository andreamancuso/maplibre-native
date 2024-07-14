#include <mbgl/actor/scheduler.hpp>
#include <mbgl/platform/settings.hpp>
#include <mbgl/util/enum.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/traits.hpp>

#include <mutex>
#include <cstdio>
#include <cstdarg>
#include <exception>
#include <sstream>

namespace mbgl {

namespace {

std::unique_ptr<Log::Observer> currentObserver;
constexpr auto SeverityCount = underlying_type(EventSeverity::SeverityCount);
std::atomic<bool> useThread[SeverityCount] = {true, true, true, false};
std::mutex mutex;

} // namespace

class Log::Impl {
public:
    Impl() {}

    void record(EventSeverity severity, Event event, int64_t code, const std::string& msg) try {
        printf("Log::Impl::record() a\n");
        if (useThread[underlying_type(severity)]) {
            printf("Log::Impl::record() b\n");
            auto threadName = platform::getCurrentThreadName();
            printf("Log::Impl::record() c\n");
            scheduler->schedule([=]() { 
                printf("Log::Impl::record() e\n");
                Log::record(severity, event, code, msg, threadName); 
                printf("Log::Impl::record() f\n");
            });
            printf("Log::Impl::record() d\n");
        } else {
            Log::record(severity, event, code, msg, {});
        }
    } catch (...) {
        // ignore exceptions during logging
        // What would we do, log them?
#if !defined(NDEBUG)
        [[maybe_unused]] auto ex = std::current_exception();
        assert(!"unhandled exception while logging");
#endif
    }

private:
    const std::shared_ptr<Scheduler> scheduler;
};

Log::Log()
    : impl(std::make_unique<Impl>()) {}

Log::~Log() = default;

Log* Log::get() noexcept {
    static Log instance;
    return &instance;
}

void Log::useLogThread(bool enable) noexcept {
    useLogThread(enable, EventSeverity::Debug);
    useLogThread(enable, EventSeverity::Info);
    useLogThread(enable, EventSeverity::Warning);
    useLogThread(enable, EventSeverity::Error);
}

void Log::useLogThread(bool enable, EventSeverity severity) noexcept {
    const auto index = underlying_type(severity);
    if (index < SeverityCount) {
        useThread[index] = enable;
    }
}

void Log::setObserver(std::unique_ptr<Observer> observer) {
    std::lock_guard<std::mutex> lock(mutex);
    currentObserver = std::move(observer);
}

std::unique_ptr<Log::Observer> Log::removeObserver() {
    std::lock_guard<std::mutex> lock(mutex);
    std::unique_ptr<Observer> observer;
    std::swap(observer, currentObserver);
    return observer;
}

void Log::record(EventSeverity severity, Event event, const std::string& msg) noexcept {
    get()->impl->record(severity, event, -1, msg);
}

void Log::record(EventSeverity severity, Event event, int64_t code, const std::string& msg) noexcept {
    get()->impl->record(severity, event, code, msg);
}

void Log::record(EventSeverity severity,
                 Event event,
                 int64_t code,
                 const std::string& msg,
                 const std::optional<std::string>& threadName) {
    std::lock_guard<std::mutex> lock(mutex);
    if (currentObserver && severity != EventSeverity::Debug && currentObserver->onRecord(severity, event, code, msg)) {
        return;
    }

    std::stringstream logStream;

    logStream << "{" << threadName.value_or(platform::getCurrentThreadName()) << "}";
    logStream << "[" << Enum<Event>::toString(event) << "]";

    if (code >= 0) {
        logStream << "(" << code << ")";
    }

    if (!msg.empty()) {
        logStream << ": " << msg;
    }

    platformRecord(severity, logStream.str());
}

} // namespace mbgl
