#include <mbgl/util/work_request.hpp>
#include <mbgl/util/work_task.hpp>

#include <stdio.h>
#include <cassert>
#include <utility>

namespace mbgl {

WorkRequest::WorkRequest(Task task_)
    : task(std::move(task_)) {
    assert(task);
}

WorkRequest::~WorkRequest() {
    printf("WorkRequest::~WorkRequest\n");
    task->cancel();
}

} // namespace mbgl
