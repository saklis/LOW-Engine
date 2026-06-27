#include <catch2/catch_test_macros.hpp>
#include <spdlog/sinks/null_sink.h>

#include "log/Log.h"
#include "memory/ComponentPool.h"
#include "ecs/IComponent.h"

// ComponentPool uses _log for error/debug output. Initialize it once to a null
// sink so tests don't crash and produce no noise.
namespace {
    struct LogGuard {
        LogGuard() {
            if (!LowEngine::_log) {
                LowEngine::_log = std::make_shared<spdlog::logger>(
                    "test", std::make_shared<spdlog::sinks::null_sink_mt>());
            }
        }
    };
    static LogGuard logGuard;
}

// ─── Minimal test component ────────────────────────────────────────────────────

namespace {
    struct TestComponent : LowEngine::ECS::IComponent<TestComponent> {
        int Value = 0;

        explicit TestComponent(LowEngine::Memory::Memory* memory)
            : IComponent(memory) {}

        TestComponent(LowEngine::Memory::Memory* memory, TestComponent const* other)
            : IComponent(memory, other), Value(other->Value) {}

        void Initialize() override {}

        void Update(float) override { Value++; }
        void FixedUpdate(float) override { Value++; }

        nlohmann::ordered_json SerializeToJSON() override {
            auto j = IComponent::SerializeToJSON();
            j["Value"] = Value;
            return j;
        }
    };

    struct OtherComponent : LowEngine::ECS::IComponent<OtherComponent> {
        int Tag = 0;

        explicit OtherComponent(LowEngine::Memory::Memory* memory)
            : IComponent(memory) {}

        OtherComponent(LowEngine::Memory::Memory* memory, OtherComponent const* other)
            : IComponent(memory, other), Tag(other->Tag) {}

        void Initialize() override {}
    };
}

using Pool      = LowEngine::Memory::ComponentPool<TestComponent>;
using OtherPool = LowEngine::Memory::ComponentPool<OtherComponent>;

// ─── CreateComponent ──────────────────────────────────────────────────────────

TEST_CASE("ComponentPool - CreateComponent returns non-null", "[pool]") {
    Pool pool;
    TestComponent* comp = pool.CreateComponent(nullptr, 0);
    REQUIRE(comp != nullptr);
}

TEST_CASE("ComponentPool - CreateComponent stores data correctly", "[pool]") {
    Pool pool;
    pool.CreateComponent(nullptr, 0)->Value = 42;

    auto* comp = reinterpret_cast<TestComponent*>(pool.GetComponentPtr(0));
    REQUIRE(comp != nullptr);
    REQUIRE(comp->Value == 42);
}

TEST_CASE("ComponentPool - CreateComponent duplicate returns nullptr", "[pool]") {
    Pool pool;
    pool.CreateComponent(nullptr, 0);
    TestComponent* duplicate = pool.CreateComponent(nullptr, 0);
    REQUIRE(duplicate == nullptr);
}

// ─── GetComponentPtr ──────────────────────────────────────────────────────────

TEST_CASE("ComponentPool - GetComponentPtr returns correct component", "[pool]") {
    Pool pool;
    TestComponent* comp = pool.CreateComponent(nullptr, 5);
    REQUIRE(comp != nullptr);
    comp->Value = 99;

    void* ptr = pool.GetComponentPtr(5);
    REQUIRE(ptr != nullptr);
    REQUIRE(reinterpret_cast<TestComponent*>(ptr)->Value == 99);
}

TEST_CASE("ComponentPool - GetComponentPtr returns nullptr for unknown entity", "[pool]") {
    Pool pool;
    REQUIRE(pool.GetComponentPtr(999) == nullptr);
}

// ─── DestroyComponent ─────────────────────────────────────────────────────────

TEST_CASE("ComponentPool - DestroyComponent removes component", "[pool]") {
    Pool pool;
    pool.CreateComponent(nullptr, 0);
    pool.DestroyComponent(0);
    REQUIRE(pool.GetComponentPtr(0) == nullptr);
}

TEST_CASE("ComponentPool - DestroyComponent on missing entity does not crash", "[pool]") {
    Pool pool;
    REQUIRE_NOTHROW(pool.DestroyComponent(999));
}

TEST_CASE("ComponentPool - destroying first component keeps last accessible", "[pool]") {
    Pool pool;
    pool.CreateComponent(nullptr, 0)->Value = 10;
    pool.CreateComponent(nullptr, 1)->Value = 20;
    pool.CreateComponent(nullptr, 2)->Value = 30;

    pool.DestroyComponent(0);

    REQUIRE(pool.GetComponentPtr(0) == nullptr);
    REQUIRE(pool.GetComponentPtr(1) != nullptr);
    REQUIRE(pool.GetComponentPtr(2) != nullptr);
    REQUIRE(reinterpret_cast<TestComponent*>(pool.GetComponentPtr(1))->Value == 20);
    REQUIRE(reinterpret_cast<TestComponent*>(pool.GetComponentPtr(2))->Value == 30);
}

// ─── ForEachComponent ─────────────────────────────────────────────────────────

TEST_CASE("ComponentPool - ForEachComponent visits all components", "[pool]") {
    Pool pool;
    pool.CreateComponent(nullptr, 0)->Value = 1;
    pool.CreateComponent(nullptr, 1)->Value = 2;
    pool.CreateComponent(nullptr, 2)->Value = 3;

    int sum = 0;
    pool.ForEachComponent([&](TestComponent& c) { sum += c.Value; });
    REQUIRE(sum == 6);
}

TEST_CASE("ComponentPool - ForEachComponent on empty pool does not crash", "[pool]") {
    Pool pool;
    REQUIRE_NOTHROW(pool.ForEachComponent([](TestComponent&) {}));
}

// ─── Update / FixedUpdate ─────────────────────────────────────────────────────

TEST_CASE("ComponentPool - Update only calls active components", "[pool]") {
    Pool pool;

    TestComponent* active   = pool.CreateComponent(nullptr, 0);
    TestComponent* inactive = pool.CreateComponent(nullptr, 1);

    active->Active   = true;
    inactive->Active = false;

    pool.Update(0.016f);

    REQUIRE(reinterpret_cast<TestComponent*>(pool.GetComponentPtr(0))->Value == 1);
    REQUIRE(reinterpret_cast<TestComponent*>(pool.GetComponentPtr(1))->Value == 0);
}

TEST_CASE("ComponentPool - FixedUpdate only calls active components", "[pool]") {
    Pool pool;

    TestComponent* active   = pool.CreateComponent(nullptr, 0);
    TestComponent* inactive = pool.CreateComponent(nullptr, 1);

    active->Active   = true;
    inactive->Active = false;

    pool.FixedUpdate(0.016f);

    REQUIRE(reinterpret_cast<TestComponent*>(pool.GetComponentPtr(0))->Value == 1);
    REQUIRE(reinterpret_cast<TestComponent*>(pool.GetComponentPtr(1))->Value == 0);
}

// ─── Capacity growth ──────────────────────────────────────────────────────────

TEST_CASE("ComponentPool - grows beyond initial capacity", "[pool]") {
    Pool pool(2); // start with capacity 2

    for (size_t i = 0; i < 5; ++i) {
        TestComponent* comp = pool.CreateComponent(nullptr, i);
        REQUIRE(comp != nullptr);
        comp->Value = static_cast<int>(i);
    }

    // All components must still be reachable after reallocation
    for (size_t i = 0; i < 5; ++i) {
        void* ptr = pool.GetComponentPtr(i);
        REQUIRE(ptr != nullptr);
        REQUIRE(reinterpret_cast<TestComponent*>(ptr)->Value == static_cast<int>(i));
    }
}

// ─── Swap-and-pop correctness (middle removal) ────────────────────────────────

TEST_CASE("ComponentPool - destroying middle component preserves all others", "[pool]") {
    Pool pool;
    pool.CreateComponent(nullptr, 0)->Value = 10;
    pool.CreateComponent(nullptr, 1)->Value = 20;
    pool.CreateComponent(nullptr, 2)->Value = 30;

    pool.DestroyComponent(1); // remove middle — entity 2 swaps into slot 1

    REQUIRE(pool.GetComponentPtr(1) == nullptr);
    REQUIRE(reinterpret_cast<TestComponent*>(pool.GetComponentPtr(0))->Value == 10);
    REQUIRE(reinterpret_cast<TestComponent*>(pool.GetComponentPtr(2))->Value == 30);
}

// ─── ForEachComponent visit count ─────────────────────────────────────────────

TEST_CASE("ComponentPool - ForEachComponent visits each component exactly once", "[pool]") {
    Pool pool;
    pool.CreateComponent(nullptr, 0)->Value = 2;
    pool.CreateComponent(nullptr, 1)->Value = 3;
    pool.CreateComponent(nullptr, 2)->Value = 5;

    int count = 0;
    int sum   = 0;
    pool.ForEachComponent([&](TestComponent& c) { count++; sum += c.Value; });

    REQUIRE(count == 3);
    REQUIRE(sum == 10); // 2+3+5 — wrong if any component is visited more than once
}

// ─── Clone ────────────────────────────────────────────────────────────────────

TEST_CASE("ComponentPool - Clone copies all components", "[pool]") {
    Pool pool;
    pool.CreateComponent(nullptr, 0)->Value = 11;
    pool.CreateComponent(nullptr, 1)->Value = 22;

    auto cloned = pool.Clone(nullptr);

    REQUIRE(cloned->GetComponentPtr(0) != nullptr);
    REQUIRE(cloned->GetComponentPtr(1) != nullptr);
    REQUIRE(reinterpret_cast<TestComponent*>(cloned->GetComponentPtr(0))->Value == 11);
    REQUIRE(reinterpret_cast<TestComponent*>(cloned->GetComponentPtr(1))->Value == 22);
}

TEST_CASE("ComponentPool - Clone is independent from original", "[pool]") {
    Pool pool;
    pool.CreateComponent(nullptr, 0)->Value = 5;

    auto cloned = pool.Clone(nullptr);

    // mutate original
    reinterpret_cast<TestComponent*>(pool.GetComponentPtr(0))->Value = 99;

    // clone must be unaffected
    REQUIRE(reinterpret_cast<TestComponent*>(cloned->GetComponentPtr(0))->Value == 5);
}

// ─── SerializeToJSON ──────────────────────────────────────────────────────────

TEST_CASE("ComponentPool - SerializeToJSON produces array with correct count", "[pool]") {
    Pool pool;
    pool.CreateComponent(nullptr, 0)->Value = 1;
    pool.CreateComponent(nullptr, 1)->Value = 2;

    auto json = pool.SerializeToJSON();
    REQUIRE(json.is_array());
    REQUIRE(json.size() == 2);
}

TEST_CASE("ComponentPool - SerializeToJSON includes component values", "[pool]") {
    Pool pool;
    TestComponent* comp = pool.CreateComponent(nullptr, 0);
    comp->Value  = 42;
    comp->Active = true;

    auto json = pool.SerializeToJSON();
    REQUIRE(json[0]["Value"]  == 42);
    REQUIRE(json[0]["Active"] == true);
}

// ─── Multiple pool types with overlapping entity ids ──────────────────────────

TEST_CASE("ComponentPool - separate pools with same entity ids are independent", "[pool]") {
    Pool      poolA;
    OtherPool poolB;

    poolA.CreateComponent(nullptr, 0)->Value = 100;
    poolB.CreateComponent(nullptr, 0)->Tag   = 200;

    REQUIRE(reinterpret_cast<TestComponent*>(poolA.GetComponentPtr(0))->Value  == 100);
    REQUIRE(reinterpret_cast<OtherComponent*>(poolB.GetComponentPtr(0))->Tag   == 200);

    poolA.DestroyComponent(0);
    REQUIRE(poolA.GetComponentPtr(0) == nullptr);
    REQUIRE(poolB.GetComponentPtr(0) != nullptr); // poolB unaffected
}