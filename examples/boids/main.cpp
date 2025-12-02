#include <FECS/FECS.h>
#include "Boids.h"
#include "Rendering.h"
#include "Grid.h"

enum SystemGroupID
{
    DEFAULT = 0,
    RENDER = 1,
    PHYSICS = 2,
};

auto main() -> int
{
    FECS::World world;
    world.Resources().Add<SpatialGrid>({});

    world.Scheduler().SetFixedStep(1.0f / 60.0f);
    world.Scheduler().SetExecutionOrder(
        {
            SystemGroupID::DEFAULT,
            SystemGroupID::PHYSICS,
            SystemGroupID::RENDER,
        });

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::RENDER)
        .Startup()
        .Build(PrepareRaylib);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::RENDER)
        .Update()
        .WithQuery<RenderComponent, PositionComponent, VelocityComponent>()
        .Build(Render);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::DEFAULT)
        .Startup()
        .Write<FECS::World>()
        .Build(SpawnBoid);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::PHYSICS)
        .Fixed()
        .Write<SpatialGrid>()
        .Write<FECS::World>()
        .WithQuery<PositionComponent, VelocityComponent>()
        .Build(ResolveForces);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::PHYSICS)
        .Fixed()
        .WithQuery<PositionComponent, VelocityComponent>()
        .Build(ResolveVelocities);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::PHYSICS)
        .Fixed()
        .WithQuery<PositionComponent, VelocityComponent>()
        .Build(ResolveWallCollision);

    world.Scheduler()
        .AddSystem()
        .In(SystemGroupID::PHYSICS)
        .Fixed()
        .Write<SpatialGrid>()
        .WithQuery<PositionComponent>()
        .Build(SpatialGridSystem);

    world.Scheduler().RunStartup();
    while (!WindowShouldClose())
    {
        world.Scheduler().Run(GetFrameTime());
    }

    CloseWindow();

    return 0;
}
