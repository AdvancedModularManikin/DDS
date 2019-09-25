#include "BaseLogger.h"

void plog::InitializeLogger() {
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);
}
