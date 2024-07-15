#include <mbgl/util/logging.hpp>
#include <mbgl/util/enum.hpp>

#include <emscripten/console.h>

namespace mbgl {

void Log::platformRecord(EventSeverity severity, const std::string &msg) {
    printf("Log::platformRecord()\n");

    auto s = Enum<EventSeverity>::toString(severity);
    std::string m;

    m += "[";
    m += s;
    m += "] ";
    m += msg;
    m += "\n";

    char arr[m.length() + 1]; 

	strcpy(arr, m.c_str());

    emscripten_console_log(arr);
}

} // namespace mbgl
