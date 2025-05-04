
#include "SoundComponent.h"

namespace LowEngine::ECS {
    void SoundComponent::SetSound(const std::string& soundAlias) {
        auto& newSound = Assets::GetSound(soundAlias);
        Sound.setBuffer(newSound);
    }

    void SoundComponent::Play() {
        Sound.play();
    }
}
