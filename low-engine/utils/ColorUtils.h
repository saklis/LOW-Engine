#pragma once

#include "SFML/Graphics/Color.hpp"

namespace LowEngine::Utils {
    /**
     * @brief Linearly interpolates between two colors.
     *
     * Each channel (r, g, b, a) is interpolated independently.
     * @param a Color at t = 0.
     * @param b Color at t = 1.
     * @param t Interpolation factor, expected in the range [0, 1].
     * @return Interpolated color.
     */
    inline sf::Color LerpColor(sf::Color a, sf::Color b, float t) {
        return sf::Color(
            static_cast<uint8_t>(a.r + (b.r - a.r) * t),
            static_cast<uint8_t>(a.g + (b.g - a.g) * t),
            static_cast<uint8_t>(a.b + (b.b - a.b) * t),
            static_cast<uint8_t>(a.a + (b.a - a.a) * t)
        );
    }
}