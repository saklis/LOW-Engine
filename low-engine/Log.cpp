#include "Log.h"

namespace LowEngine {
    // Define the static shared pointer. Instance created in Game's constructor
    std::shared_ptr<spdlog::logger> _log = nullptr;
}
