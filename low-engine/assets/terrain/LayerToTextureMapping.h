#pragma once
#include <string>
#include <vector>
#include <utility>

#include "Layer.h"

namespace LowEngine::Terrain {
    class LayerToTextureMapping {
    public:
        LayerType Type;
        size_t TextureId = 0;
        std::vector<std::vector<std::string>> AnimationClipNames;

    public:
        LayerToTextureMapping(LayerType type, size_t textureId, std::vector<std::vector<std::string>> animationClipNames = {})
            : Type(type),
              TextureId(textureId),
              AnimationClipNames(std::move(animationClipNames)) {
        }
    };
}
