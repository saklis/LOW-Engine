#include <catch2/catch_test_macros.hpp>
#include <unordered_map>

#include "utils/ColorUtils.h"
#include "utils/TypeHash.h"
#include "utils/TypeName.h"

using namespace LowEngine::Utils;

// ─── LerpColor ────────────────────────────────────────────────────────────────

TEST_CASE("LerpColor - t=0 returns first color", "[utils][color]") {
    sf::Color result = LerpColor(sf::Color::Black, sf::Color::White, 0.0f);
    REQUIRE(result == sf::Color::Black);
}

TEST_CASE("LerpColor - t=1 returns second color", "[utils][color]") {
    sf::Color result = LerpColor(sf::Color::Black, sf::Color::White, 1.0f);
    REQUIRE(result == sf::Color::White);
}

TEST_CASE("LerpColor - t=0.5 returns midpoint", "[utils][color]") {
    sf::Color result = LerpColor(sf::Color::Black, sf::Color::White, 0.5f);
    // uint8_t truncation: (0 + 255 * 0.5) = 127
    REQUIRE(result == sf::Color(127, 127, 127, 255));
}

TEST_CASE("LerpColor - same color returns same color", "[utils][color]") {
    sf::Color red = sf::Color::Red;
    REQUIRE(LerpColor(red, red, 0.5f) == red);
}

TEST_CASE("LerpColor - alpha is interpolated", "[utils][color]") {
    sf::Color transparent(0, 0, 0, 0);
    sf::Color opaque(0, 0, 0, 255);
    sf::Color result = LerpColor(transparent, opaque, 0.5f);
    REQUIRE(result.a == 127);
}

// ─── Vector2iHash ─────────────────────────────────────────────────────────────

TEST_CASE("Vector2iHash - same vector produces same hash", "[utils][hash]") {
    Vector2iHash hasher;
    sf::Vector2i v(3, 7);
    REQUIRE(hasher(v) == hasher(v));
}

TEST_CASE("Vector2iHash - (x,y) and (y,x) produce different hashes", "[utils][hash]") {
    Vector2iHash hasher;
    REQUIRE(hasher({1, 2}) != hasher({2, 1}));
}

TEST_CASE("Vector2iHash - usable as unordered_map key", "[utils][hash]") {
    std::unordered_map<sf::Vector2i, int, Vector2iHash> map;
    map[{0, 0}] = 1;
    map[{1, 0}] = 2;
    map[{0, 1}] = 3;

    REQUIRE(map[{0, 0}] == 1);
    REQUIRE(map[{1, 0}] == 2);
    REQUIRE(map[{0, 1}] == 3);
}

// ─── TypeName ─────────────────────────────────────────────────────────────────

TEST_CASE("TypeName - returns non-empty string for primitive types", "[utils][typename]") {
    REQUIRE_FALSE(TypeName<int>().empty());
    REQUIRE_FALSE(TypeName<float>().empty());
}

TEST_CASE("TypeName - different types produce different names", "[utils][typename]") {
    REQUIRE(TypeName<int>() != TypeName<float>());
}

TEST_CASE("GetCleanTypeName - returns std::string", "[utils][typename]") {
    std::string name = GetCleanTypeName<int>();
    REQUIRE_FALSE(name.empty());
}