#pragma once

#include <string>

namespace LowEngine::Assets {

    class AssetManager {
    public:
        AssetManager() = default;

        void LoadAllFromPath(const std::string& path);
    };

}
