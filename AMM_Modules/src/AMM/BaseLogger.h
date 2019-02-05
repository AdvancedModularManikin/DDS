#include <../thirdparty/plog/Log.h>
#include <../thirdparty/plog/Appenders/ColorConsoleAppender.h>

// Map log-trace to verbose
#define LOG_TRACE                       LOG_DEBUG
#define LOGT                             LOG_DEBUG

namespace plog {
    void InitializeLogger();
}