#include "Emitter.h"

#include "nlohmann/json.hpp"
#include "assets/Assets.h"
#include "log/Log.h"

namespace LowEngine::Particles {

    nlohmann::ordered_json Emitter::SerializeToJSON() const {
        nlohmann::ordered_json json;

        json["TextureAlias"] = Assets::GetTextureAlias(TextureId);
        json["Mode"]         = static_cast<int>(Mode);
        json["FrameIndex"]   = FrameIndex;
        json["AnimClipName"] = AnimClipName;

        json["EmissionRate"]  = EmissionRate;
        json["BurstCount"]    = BurstCount;
        json["MaxParticles"]  = MaxParticles;
        json["Loop"]          = Loop;

        json["LifetimeMin"] = LifetimeMin;
        json["LifetimeMax"] = LifetimeMax;

        json["SpeedMin"]     = SpeedMin;
        json["SpeedMax"]     = SpeedMax;
        json["SpreadAngle"]  = SpreadAngle;
        json["Direction"]    = {{"x", Direction.x}, {"y", Direction.y}};
        json["SpawnRadius"]        = SpawnRadius;
        json["RotationSpeedMin"]   = RotationSpeedMin;
        json["RotationSpeedMax"]   = RotationSpeedMax;

        json["ColorStart"] = {{"r", ColorStart.r}, {"g", ColorStart.g}, {"b", ColorStart.b}, {"a", ColorStart.a}};
        json["ColorEnd"]   = {{"r", ColorEnd.r},   {"g", ColorEnd.g},   {"b", ColorEnd.b},   {"a", ColorEnd.a}};
        json["ScaleStart"] = ScaleStart;
        json["ScaleEnd"]   = ScaleEnd;

        json["Gravity"]    = {{"x", Gravity.x}, {"y", Gravity.y}};
        json["DrawOrder"]  = DrawOrder;

        return json;
    }

    bool Emitter::DeserializeFromJSON(const nlohmann::ordered_json& json) {
        if (json.contains("TextureAlias")) {
            const auto alias = json["TextureAlias"].get<std::string>();
            if (!Assets::TextureExists(alias)) {
                _log->error("Emitter deserialization failed: texture '{}' not found.", alias);
                return false;
            }
            TextureId = Assets::GetTextureId(alias);
        } else {
            _log->error("Emitter deserialization failed: missing 'TextureAlias' field.");
            return false;
        }

        const int modeInt = json.value("Mode", static_cast<int>(SpriteMode::FullTexture));
        if (modeInt < 0 || modeInt > static_cast<int>(SpriteMode::AnimationClip)) {
            _log->error("Emitter deserialization failed: invalid Mode value {}.", modeInt);
            return false;
        }
        Mode = static_cast<SpriteMode>(modeInt);
        FrameIndex = json.value("FrameIndex", std::size_t{0});
        AnimClipName = json.value("AnimClipName", std::string{});

        EmissionRate  = json.value("EmissionRate",  10.0f);
        BurstCount    = json.value("BurstCount",    std::size_t{0});
        MaxParticles  = json.value("MaxParticles",  std::size_t{100});
        Loop          = json.value("Loop",          false);

        LifetimeMin = json.value("LifetimeMin", 1.0f);
        LifetimeMax = json.value("LifetimeMax", 2.0f);

        SpeedMin    = json.value("SpeedMin",    50.0f);
        SpeedMax    = json.value("SpeedMax",    100.0f);
        SpreadAngle = json.value("SpreadAngle", 360.0f);
        SpawnRadius       = json.value("SpawnRadius",       0.0f);
        RotationSpeedMin  = json.value("RotationSpeedMin",  0.0f);
        RotationSpeedMax  = json.value("RotationSpeedMax",  0.0f);

        if (json.contains("Direction")) {
            Direction.x = json["Direction"].value("x", 0.0f);
            Direction.y = json["Direction"].value("y", -1.0f);
        }

        if (json.contains("ColorStart")) {
            ColorStart.r = json["ColorStart"].value("r", uint8_t{255});
            ColorStart.g = json["ColorStart"].value("g", uint8_t{255});
            ColorStart.b = json["ColorStart"].value("b", uint8_t{255});
            ColorStart.a = json["ColorStart"].value("a", uint8_t{255});
        }
        if (json.contains("ColorEnd")) {
            ColorEnd.r = json["ColorEnd"].value("r", uint8_t{0});
            ColorEnd.g = json["ColorEnd"].value("g", uint8_t{0});
            ColorEnd.b = json["ColorEnd"].value("b", uint8_t{0});
            ColorEnd.a = json["ColorEnd"].value("a", uint8_t{0});
        }

        ScaleStart = json.value("ScaleStart", 1.0f);
        ScaleEnd   = json.value("ScaleEnd",   0.0f);

        if (json.contains("Gravity")) {
            Gravity.x = json["Gravity"].value("x", 0.0f);
            Gravity.y = json["Gravity"].value("y", 0.0f);
        }

        DrawOrder = json.value("DrawOrder", 0);

        return true;
    }
}