#pragma once

#include <string>

namespace LowEditor {
    class Config {
    public:
        inline static const std::string DEFAULT_NEW_PROJECT_NAME = "New Project";
        inline static const std::string PROJECT_DIRECTORY = "projects";

        inline static const std::string TEMPORARY_SCENE_SUFFIX = " (TEMPORARY)";
    };
}