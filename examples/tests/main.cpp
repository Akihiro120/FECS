#include <iostream>
#include <vector>
#include <cstdlib>

// ——— Always-on test macro ———
#define TEST_CHECK(expr)                                         \
    do                                                           \
    {                                                            \
        if (!(expr))                                             \
        {                                                        \
            std::cerr                                            \
                << "TEST FAILED: " #expr                         \
                << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            std::exit(EXIT_FAILURE);                             \
        }                                                        \
    } while (0)

#include "fecs/registry/registry.h" // Registry
#include "fecs/core/types.h"        // Entity, SPARSE_PAGE_SIZE

using namespace FECS;

// Sample components
struct Position
{
    int x, y;
};
struct Velocity
{
    int dx, dy;
};

void TestEntityLifecycle()
{
    ComponentManager::ClearRegistry();
    Registry registry;
    Entity e1 = registry.CreateEntity();
    Entity e2 = registry.CreateEntity();
    TEST_CHECK(e1 != e2);
    TEST_CHECK(registry.IsEntityAlive(e1) && registry.IsEntityAlive(e2));

    registry.DestroyEntity(e1);
    TEST_CHECK(!registry.IsEntityAlive(e1));

    Entity e3 = registry.CreateEntity();
    TEST_CHECK(registry.IsEntityAlive(e3));

    std::cout << "[OK] TestEntityLifecycle\n";
}

void TestComponentOps()
{
    ComponentManager::ClearRegistry();
    Registry registry;
    registry.Reserve(1);
    registry.GetEntityManager().Reserve(16);

    // Register & attach
    registry.RegisterComponent<Position>();
    Entity e = registry.CreateEntity();

    // initially no component
    auto& posSet = registry.RegisterComponent<Position>();
    TEST_CHECK(!posSet.Has(e));

    registry.Attach<Position>(e, {42, 99});
    TEST_CHECK(posSet.Has(e));

    // Get and verify data
    Position& p = registry.Get<Position>(e);
    TEST_CHECK(p.x == 42 && p.y == 99);

    // Detach and verify
    registry.Detach<Position>(e);
    TEST_CHECK(!posSet.Has(e));

    std::cout << "[OK] TestComponentOps\n";
}

void TestDeleteEntityStripsAll()
{
    ComponentManager::ClearRegistry();
    Registry registry;
    registry.Reserve(2);
    registry.GetEntityManager().Reserve(16);

    registry.RegisterComponent<Position>();
    registry.RegisterComponent<Velocity>();

    Entity e = registry.CreateEntity();
    auto& posSet = registry.RegisterComponent<Position>();
    auto& velSet = registry.RegisterComponent<Velocity>();

    registry.Attach<Position>(e, {1, 2});
    registry.Attach<Velocity>(e, {3, 4});
    TEST_CHECK(posSet.Has(e) && velSet.Has(e));

    registry.DestroyEntity(e);
    TEST_CHECK(!registry.IsEntityAlive(e));
    TEST_CHECK(!posSet.Has(e) && !velSet.Has(e));

    std::cout << "[OK] TestDeleteEntityStripsAll\n";
}

void TestSingleComponentView()
{
    ComponentManager::ClearRegistry();
    Registry registry;
    registry.Reserve(1);
    registry.GetEntityManager().Reserve(16);

    registry.RegisterComponent<Position>();
    Entity a = registry.CreateEntity();
    Entity b = registry.CreateEntity();

    registry.Attach<Position>(a, {5, 5});
    registry.Attach<Position>(b, {6, 6});

    auto view = registry.View<Position>();
    view.Reserve(8);

    std::vector<Entity> seen;
    view.Each([&](Entity e, Position& p)
    {
        seen.push_back(e);
        TEST_CHECK((e == a && p.x == 5) || (e == b && p.x == 6));
    });
    TEST_CHECK(seen.size() == 2);

    std::cout << "[OK] TestSingleComponentView\n";
}

void TestMultiComponentView()
{
    ComponentManager::ClearRegistry();
    Registry registry;
    registry.Reserve(2);
    registry.GetEntityManager().Reserve(16);

    registry.RegisterComponent<Position>();
    registry.RegisterComponent<Velocity>();

    Entity e = registry.CreateEntity();
    registry.Attach<Position>(e, {7, 7});
    registry.Attach<Velocity>(e, {1, 1});

    auto view = registry.View<Position, Velocity>();
    std::vector<Entity> seen;
    view.Each([&](Entity ent, Position& p, Velocity& v)
    {
        seen.push_back(ent);
        TEST_CHECK(p.x == 7 && v.dx == 1);
    });
    TEST_CHECK(seen.size() == 1);
    TEST_CHECK(seen[0] == e);

    std::cout << "[OK] TestMultiComponentView\n";
}

// Test 1: attaching the same component twice updates, not duplicates
void TestDoubleAttachUpdates()
{
    ComponentManager::ClearRegistry();
    Registry r;
    r.Reserve(1);
    r.GetEntityManager().Reserve(4);
    r.RegisterComponent<Position>();

    Entity e = r.CreateEntity();
    r.Attach<Position>(e, {1, 1});
    r.Attach<Position>(e, {2, 2}); // second attach should overwrite

    Position& p = r.Get<Position>(e);
    TEST_CHECK(p.x == 2 && p.y == 2);

    auto view = r.View<Position>();
    std::vector<Entity> seen;
    view.Each([&](Entity eid, Position& pp)
    {
        seen.push_back(eid);
        TEST_CHECK(eid == e && pp.x == 2);
    });
    TEST_CHECK(seen.size() == 1);

    std::cout << "[OK] TestDoubleAttachUpdates\n";
}

// Test 2: detaching a non-existent component is safe (no crash, no effect)
void TestDetachNonexistentNoCrash()
{
    ComponentManager::ClearRegistry();
    Registry r;
    r.Reserve(1);
    r.GetEntityManager().Reserve(4);
    r.RegisterComponent<Position>();

    Entity e = r.CreateEntity();
    // no attach yet
    r.Detach<Position>(e); // should do nothing
    TEST_CHECK(!r.RegisterComponent<Position>().Has(e));

    // attach then detach twice
    r.Attach<Position>(e, {3, 3});
    r.Detach<Position>(e);
    TEST_CHECK(!r.RegisterComponent<Position>().Has(e));
    r.Detach<Position>(e); // second detach still safe
    TEST_CHECK(!r.RegisterComponent<Position>().Has(e));

    std::cout << "[OK] TestDetachNonexistentNoCrash\n";
}

// Test 3: destroying an entity with no components is safe
void TestDestroyEntityNoComponents()
{
    ComponentManager::ClearRegistry();
    Registry r;
    r.Reserve(1);
    r.GetEntityManager().Reserve(4);

    Entity e = r.CreateEntity();
    TEST_CHECK(r.IsEntityAlive(e));
    r.DestroyEntity(e);
    TEST_CHECK(!r.IsEntityAlive(e)); // should simply destroy

    std::cout << "[OK] TestDestroyEntityNoComponents\n";
}

// Test 4: ID reuse does not leak old components
void TestIDReuseNoLeak()
{
    ComponentManager::ClearRegistry();
    Registry r;
    r.Reserve(1);
    r.GetEntityManager().Reserve(4);
    r.RegisterComponent<Position>();

    Entity e1 = r.CreateEntity();
    r.Attach<Position>(e1, {7, 7});
    TEST_CHECK(r.RegisterComponent<Position>().Has(e1));

    r.DestroyEntity(e1);
    Entity e2 = r.CreateEntity();
    // many EntityManagers recycle the same ID:
    TEST_CHECK(e2 != e1);
    TEST_CHECK(GetEntityIndex(e2) == GetEntityIndex(e1));
    TEST_CHECK(GetEntityVersion(e2) != GetEntityVersion(e1));

    // old component must not remain
    TEST_CHECK(!r.RegisterComponent<Position>().Has(e2));

    std::cout << "[OK] TestIDReuseNoLeak\n";
}

// Test 5: attaching at a page-boundary index still works
void TestPageBoundaryAttach()
{
    ComponentManager::ClearRegistry();
    Registry r;
    r.Reserve(1);
    // reserve enough entities so IDs cross SPARSE_PAGE_SIZE
    r.GetEntityManager().Reserve(SPARSE_PAGE_SIZE + 2);
    r.RegisterComponent<Position>();

    std::vector<Entity> temp;
    temp.reserve(SPARSE_PAGE_SIZE + 1);
    for (int i = 0; i < SPARSE_PAGE_SIZE; ++i)
        temp.push_back(r.CreateEntity());

    Entity eb = r.CreateEntity(); // should have index == SPARSE_PAGE_SIZE
    r.Attach<Position>(eb, {9, 9});

    auto view = r.View<Position>();
    std::vector<Entity> seen;
    view.Each([&](Entity e, Position& p)
    {
        seen.push_back(e);
        TEST_CHECK(e == eb && p.x == 9);
    });
    TEST_CHECK(seen.size() == 1);

    std::cout << "[OK] TestPageBoundaryAttach\n";
}

// Test 6: dynamic view updates on interleaved attach/detach
void TestDynamicViewUpdate()
{
    ComponentManager::ClearRegistry();
    Registry r;
    r.Reserve(1);
    r.GetEntityManager().Reserve(8);
    r.RegisterComponent<Position>();

    Entity e1 = r.CreateEntity();
    Entity e2 = r.CreateEntity();
    Entity e3 = r.CreateEntity();

    // attach to e1 and e2
    r.Attach<Position>(e1, {1, 1});
    r.Attach<Position>(e2, {2, 2});

    auto view = r.View<Position>();
    // first pass sees e1,e2
    std::vector<Entity> seen1;
    view.Each([&](Entity e, Position&)
    {
        seen1.push_back(e);
    });
    TEST_CHECK(seen1.size() == 2);

    // detach e1, view should rebuild and see only e2
    r.Detach<Position>(e1);
    std::vector<Entity> seen2;
    view.Each([&](Entity e, Position&)
    {
        seen2.push_back(e);
    });
    TEST_CHECK(seen2.size() == 1 && seen2[0] == e2);

    // attach e3, view should rebuild again and see e2,e3
    r.Attach<Position>(e3, {3, 3});
    std::vector<Entity> seen3;
    view.Each([&](Entity e, Position&)
    {
        seen3.push_back(e);
    });
    TEST_CHECK(seen3.size() == 2);
    TEST_CHECK((seen3[0] == e2 && seen3[1] == e3) ||
               (seen3[0] == e3 && seen3[1] == e2));

    std::cout << "[OK] TestDynamicViewUpdate\n";
}

// A third component!
struct Health
{
    int hp;
};

// 1) Has() returns false if never attached
void TestHasOnMissing()
{
    ComponentManager::ClearRegistry();
    Registry r;
    r.Reserve(1);
    r.GetEntityManager().Reserve(4);
    r.RegisterComponent<Position>();

    Entity e = r.CreateEntity();
    auto& posSet = r.RegisterComponent<Position>();
    TEST_CHECK(!posSet.Has(e)); // never attached

    r.Attach<Position>(e, {1, 1});
    TEST_CHECK(posSet.Has(e));
    r.Detach<Position>(e);
    TEST_CHECK(!posSet.Has(e)); // now removed

    std::cout << "[OK] TestHasOnMissing\n";
}

// 2) Triple‐component view
void TestTripleComponentView()
{
    ComponentManager::ClearRegistry();
    Registry r;
    r.Reserve(3);
    r.GetEntityManager().Reserve(10);
    r.RegisterComponent<Position>();
    r.RegisterComponent<Velocity>();
    r.RegisterComponent<Health>();

    Entity e1 = r.CreateEntity();
    Entity e2 = r.CreateEntity();

    r.Attach<Position>(e1, {1, 1});
    r.Attach<Velocity>(e1, {1, 0});
    r.Attach<Health>(e1, {99});

    r.Attach<Position>(e2, {2, 2}); // no Velocity or Health

    auto view = r.View<Position, Velocity, Health>();
    std::vector<Entity> seen;
    view.Each([&](Entity e, Position&, Velocity&, Health& h)
    {
        seen.push_back(e);
        TEST_CHECK(e == e1);
        TEST_CHECK(h.hp == 99);
    });
    TEST_CHECK(seen.size() == 1);

    std::cout << "[OK] TestTripleComponentView\n";
}

// 3) Dynamic attach/detach invalidates cache
void TestDynamicAttachDetachView()
{
    ComponentManager::ClearRegistry();
    Registry r;
    r.Reserve(1);
    r.GetEntityManager().Reserve(8);
    r.RegisterComponent<Position>();

    Entity e1 = r.CreateEntity();
    Entity e2 = r.CreateEntity();

    r.Attach<Position>(e1, {1, 1});
    auto view = r.View<Position>();
    std::vector<Entity> seen1;
    view.Each([&](Entity e, Position&)
    { seen1.push_back(e); });
    TEST_CHECK(seen1.size() == 1 && seen1[0] == e1);

    // attach new entity, view must rebuild
    r.Attach<Position>(e2, {2, 2});
    std::vector<Entity> seen2;
    view.Each([&](Entity e, Position&)
    { seen2.push_back(e); });
    TEST_CHECK(seen2.size() == 2);

    // detach first, view must rebuild again
    r.Detach<Position>(e1);
    std::vector<Entity> seen3;
    view.Each([&](Entity e, Position&)
    { seen3.push_back(e); });
    TEST_CHECK(seen3.size() == 1 && seen3[0] == e2);

    std::cout << "[OK] TestDynamicAttachDetachView\n";
}

// 6) Large multi‐page stress
void TestSparseMultiPageStress()
{
    ComponentManager::ClearRegistry();
    Registry r;
    r.Reserve(1);
    const int N = SPARSE_PAGE_SIZE * 3 + 7;
    r.GetEntityManager().Reserve(N + 2);
    r.RegisterComponent<Position>();

    std::vector<Entity> es;
    es.reserve(N);
    for (int i = 0; i < N; ++i)
    {
        es.push_back(r.CreateEntity());
        r.Attach<Position>(es.back(), {i, i});
    }

    // verify all present via view
    auto view = r.View<Position>();
    int cnt = 0;
    view.Each([&](Entity e, Position& p)
    {
        TEST_CHECK(es[cnt] == e);
        TEST_CHECK(p.x == cnt && p.y == cnt);
        ++cnt;
    });
    TEST_CHECK(cnt == N);

    std::cout << "[OK] TestSparseMultiPageStress\n";
}

int main()
{
    TestEntityLifecycle();
    TestComponentOps();
    TestDeleteEntityStripsAll();
    TestSingleComponentView();
    TestMultiComponentView();
    std::cout << "✅ All simple ECS tests passed!\n";

    TestDoubleAttachUpdates();
    TestDetachNonexistentNoCrash();
    TestDestroyEntityNoComponents();
    TestIDReuseNoLeak();
    TestPageBoundaryAttach();
    TestDynamicViewUpdate();
    std::cout << "✅ All edge-case ECS tests passed!\n";

    TestHasOnMissing();
    TestTripleComponentView();
    TestDynamicAttachDetachView();
    TestSparseMultiPageStress();
    std::cout << "✅ All additional edge-case ECS tests passed!\n";

    return 0;
}
