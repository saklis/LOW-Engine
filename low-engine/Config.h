#pragma once

#include <limits>

namespace LowEngine {
    /* * Config class
     * This class contains the configuration settings for the LowEngine.
     */
    class Config {
    public:
        /**
         * @brief Default count of the component in the pool for each scene.
         *
         * This value is used to reserve the Component Pool for each scene.
         * Component Pool will automatically expand during the scene's lifetime.
         */
        inline static const std::size_t DEFAULT_COMPONENT_POOL_SIZE = 1000;

        /**
         * @brief Name of the logger used in the engine.
         */
        inline static const char* LOGGER_NAME = "low_engine_spdlog_logger_name";

        /**
         * @brief Maximum value for size_t.
         *
         * This value is used as "null" value for size_t.
         */
        inline static const unsigned int MAX_SIZE = std::numeric_limits<std::size_t>::max();
    };
}
