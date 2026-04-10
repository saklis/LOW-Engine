#pragma once
#include <cstddef>
#include <functional>

#include "SFML/System/Vector2.hpp"

namespace LowEngine::Utils {
    /**
     * @brief Hash functor for sf::Vector2i, for use as the Hash parameter in unordered containers.
     *
     * std::unordered_map and std::unordered_set require a hash function for their key type.
     * No standard specialization of std::hash exists for sf::Vector2i, so this functor must be
     * supplied explicitly, e.g.:
     *
     *   std::unordered_map<sf::Vector2i, MyValue, LowEngine::Utils::Vector2iHash> myMap;
     *
     * The two component hashes are combined using the "hash_combine" pattern, adapted from
     * Boost (boost/container_hash/hash.hpp). The magic constant 0x9e3779b9 is the 32-bit
     * truncation of the golden ratio (2^32 / phi), which spreads bits well and reduces
     * clustering. The shifts ensure that the combined value is sensitive to input order,
     * so (x, y) and (y, x) produce different hashes.
     *
     * Reference: https://www.boost.org/doc/libs/release/libs/container_hash/doc/html/hash.html#notes_hash_combine
     */
    struct Vector2iHash {
        std::size_t operator () (const sf::Vector2i& i) const {
            auto hx = std::hash<std::size_t>{}(i.x);
            auto hy = std::hash<std::size_t>{}(i.y);
            return hx ^ (hy + 0x9e3779b9 + (hx << 6) + (hx >> 2));
        }
    };
}
