#include <../thirdparty/plog/Log.h>
#include <../thirdparty/plog/Appenders/ColorConsoleAppender.h>

// Map log-trace to verbose
#define LOG_TRACE                       LOG(plog::verbose)
#define LOGT                             LOG_TRACE

namespace plog {
    void InitializeLogger();
}