#include <catch2/catch_test_macros.hpp>
#include <spdlog/sinks/null_sink.h>

#include "log/Log.h"
#include "memory/Memory.h"
#include "ecs/Entity.h"
#include "ecs/IComponent.h"

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

// ─── Test components ──────────────────────────────────────────────────────────

namespace {
    struct TestComp : LowEngine::ECS::IComponent<TestComp> {
        int Value           = 0;
        bool InitCalled     = false;

        explicit TestComp(LowEngine::Memory::Memory* memory)
            : IComponent(memory) {}

        TestComp(LowEngine::Memory::Memory* memory, TestComp const* other)
            : IComponent(memory, other), Value(other->Value) {}

        void Initialize() override { InitCalled = true; }

        nlohmann::ordered_json SerializeToJSON() override {
            auto j = IComponent::SerializeToJSON();
            j["Value"] = Value;
            return j;
        }
    };

    // Depends on TestComp — must be created after TestComp
    struct DependentComp : LowEngine::ECS::IComponent<DependentComp, TestComp> {
        explicit DependentComp(LowEngine::Memory::Memory* memory)
            : IComponent(memory) {}

        DependentComp(LowEngine::Memory::Memory* memory, DependentComp const* other)
            : IComponent(memory, other) {}

        void Initialize() override {}
    };
}

// ─── CreateEntity ─────────────────────────────────────────────────────────────

TEST_CASE("Memory - CreateEntity returns non-null", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("hero");
    REQUIRE(e != nullptr);
}

TEST_CASE("Memory - CreateEntity sets Name and Active", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("hero");
    REQUIRE(e->Name   == "hero");
    REQUIRE(e->Active == true);
}

TEST_CASE("Memory - CreateEntity assigns sequential Ids", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* a = mem.CreateEntity<LowEngine::ECS::Entity>("a");
    auto* b = mem.CreateEntity<LowEngine::ECS::Entity>("b");
    REQUIRE(a->Id == 0);
    REQUIRE(b->Id == 1);
}

// ─── GetEntity / FindEntity ───────────────────────────────────────────────────

TEST_CASE("Memory - GetEntity returns correct entity", "[memory]") {
    LowEngine::Memory::Memory mem;
    mem.CreateEntity<LowEngine::ECS::Entity>("x");
    auto* e = mem.GetEntity<LowEngine::ECS::Entity>(0);
    REQUIRE(e != nullptr);
    REQUIRE(e->Name == "x");
}

TEST_CASE("Memory - GetEntity returns nullptr for out-of-range id", "[memory]") {
    LowEngine::Memory::Memory mem;
    REQUIRE(mem.GetEntity<LowEngine::ECS::Entity>(99) == nullptr);
}

TEST_CASE("Memory - FindEntity finds by name", "[memory]") {
    LowEngine::Memory::Memory mem;
    mem.CreateEntity<LowEngine::ECS::Entity>("target");
    auto* e = mem.FindEntity<LowEngine::ECS::Entity>("target");
    REQUIRE(e != nullptr);
    REQUIRE(e->Name == "target");
}

TEST_CASE("Memory - FindEntity returns nullptr for missing name", "[memory]") {
    LowEngine::Memory::Memory mem;
    REQUIRE(mem.FindEntity<LowEngine::ECS::Entity>("ghost") == nullptr);
}

// ─── DestroyEntity ────────────────────────────────────────────────────────────

TEST_CASE("Memory - DestroyEntity removes entity", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    mem.DestroyEntity(e);
    REQUIRE(mem.GetEntity<LowEngine::ECS::Entity>(0) == nullptr);
}

TEST_CASE("Memory - DestroyEntity removes entity's components", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    mem.CreateComponent<TestComp>(e->Id);
    mem.DestroyEntity(e);
    REQUIRE(mem.GetComponent<TestComp>(0) == nullptr);
}

// ─── CreateComponent ──────────────────────────────────────────────────────────

TEST_CASE("Memory - CreateComponent sets EntityId and Active", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    auto* c = mem.CreateComponent<TestComp>(e->Id);
    REQUIRE(c != nullptr);
    REQUIRE(c->EntityId == e->Id);
    REQUIRE(c->Active   == true);
}

TEST_CASE("Memory - CreateComponent calls Initialize", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    auto* c = mem.CreateComponent<TestComp>(e->Id);
    REQUIRE(c->InitCalled == true);
}

TEST_CASE("Memory - CreateComponent duplicate returns nullptr", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    mem.CreateComponent<TestComp>(e->Id);
    REQUIRE(mem.CreateComponent<TestComp>(e->Id) == nullptr);
}

TEST_CASE("Memory - CreateComponent with missing dependency returns nullptr", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    // DependentComp requires TestComp — not added yet
    REQUIRE(mem.CreateComponent<DependentComp>(e->Id) == nullptr);
}

TEST_CASE("Memory - CreateComponent with satisfied dependency succeeds", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    mem.CreateComponent<TestComp>(e->Id);
    REQUIRE(mem.CreateComponent<DependentComp>(e->Id) != nullptr);
}

// ─── GetComponent ─────────────────────────────────────────────────────────────

TEST_CASE("Memory - GetComponent returns correct component", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    mem.CreateComponent<TestComp>(e->Id)->Value = 77;
    auto* c = mem.GetComponent<TestComp>(e->Id);
    REQUIRE(c != nullptr);
    REQUIRE(c->Value == 77);
}

TEST_CASE("Memory - GetComponent returns nullptr for missing component", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    REQUIRE(mem.GetComponent<TestComp>(e->Id) == nullptr);
}

// ─── DestroyComponent ─────────────────────────────────────────────────────────

TEST_CASE("Memory - DestroyComponent removes component", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    mem.CreateComponent<TestComp>(e->Id);
    mem.DestroyComponent<TestComp>(e->Id);
    REQUIRE(mem.GetComponent<TestComp>(e->Id) == nullptr);
}

// ─── IsComponentSafeToDestroy ─────────────────────────────────────────────────

TEST_CASE("Memory - IsComponentSafeToDestroy is false when depended on", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    mem.CreateComponent<TestComp>(e->Id);
    mem.CreateComponent<DependentComp>(e->Id);
    REQUIRE(mem.IsComponentSafeToDestroy<TestComp>(e->Id) == false);
}

TEST_CASE("Memory - IsComponentSafeToDestroy is true when not depended on", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    mem.CreateComponent<TestComp>(e->Id);
    REQUIRE(mem.IsComponentSafeToDestroy<TestComp>(e->Id) == true);
}

// ─── Copy constructor ─────────────────────────────────────────────────────────

TEST_CASE("Memory - copy clones entities", "[memory]") {
    LowEngine::Memory::Memory original;
    original.CreateEntity<LowEngine::ECS::Entity>("hero");

    LowEngine::Memory::Memory copy(original);
    auto* e = copy.GetEntity<LowEngine::ECS::Entity>(0);
    REQUIRE(e != nullptr);
    REQUIRE(e->Name == "hero");
}

TEST_CASE("Memory - copy clones component data", "[memory]") {
    LowEngine::Memory::Memory original;
    auto* e = original.CreateEntity<LowEngine::ECS::Entity>("e");
    original.CreateComponent<TestComp>(e->Id)->Value = 55;

    LowEngine::Memory::Memory copy(original);
    auto* c = copy.GetComponent<TestComp>(0);
    REQUIRE(c != nullptr);
    REQUIRE(c->Value == 55);
}

TEST_CASE("Memory - copy is independent from original", "[memory]") {
    LowEngine::Memory::Memory original;
    auto* e = original.CreateEntity<LowEngine::ECS::Entity>("e");
    original.CreateComponent<TestComp>(e->Id)->Value = 1;

    LowEngine::Memory::Memory copy(original);
    original.GetComponent<TestComp>(0)->Value = 99;

    REQUIRE(copy.GetComponent<TestComp>(0)->Value == 1);
}

// ─── Destroy ──────────────────────────────────────────────────────────────────

TEST_CASE("Memory - Destroy clears all entities and components", "[memory]") {
    LowEngine::Memory::Memory mem;
    auto* e = mem.CreateEntity<LowEngine::ECS::Entity>("e");
    mem.CreateComponent<TestComp>(e->Id);
    mem.Destroy();
    REQUIRE(mem.GetEntity<LowEngine::ECS::Entity>(0)  == nullptr);
    REQUIRE(mem.GetComponent<TestComp>(0)             == nullptr);
}