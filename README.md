# FECS - A Functional Entity Component System

[![Test](https://github.com/Akihiro120/FECS/actions/workflows/test.yml/badge.svg)](https://github.com/Akihiro120/FECS/actions/workflows/test.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

> A modern, manager-based, and data-oriented C++20 Entity Component System (ECS) framework designed for performance and ease of use in real-time applications.

---
## Table of Contents
- [Why FECS?](#why-fecs)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
- [Quick Start](#quick-start)
- [Core Concepts](#core-concepts)
  - [The World](#the-world)
  - [Entities](#entities)
  - [Components](#components)
  - [Systems & Scheduling](#systems--scheduling)
  - [Views & Queries](#views--queries)
  - [Resources](#resources)
- [Examples](#examples)
- [License](#license)

---
## Why FECS
FECS is built on a modular, manager-based architecture that promotes clean, decoupled code. It leverages modern C++20 features and a fluent builder API to provide an expressive and type-safe way to build systems. The design prioritizes data-oriented principles for high performance in demanding applications like games and simulations, while remaining flexible and easy to extend.

---
## Features
- **Modern C++20 Design**: Utilizes modern C++ features for a clean and efficient implementation.
- **Manager-Based Architecture**: Decoupled managers for Entities, Components, Systems, and more.
- **Fluent Builder APIs**: Expressive and type-safe APIs for creating entities and defining systems.
- **Powerful System Scheduling**: Organize systems into ordered sets with support for `Startup`, `Update`, `Fixed`, and `Timed` execution.
- **Data-Oriented**: High-performance component storage using sparse sets.
- **Resource Management**: A dedicated manager for global, non-entity data.

---
## Getting Started

### Prerequisites
- C++20 compatible compiler (GCC, Clang, MSVC)
- CMake >= 3.16

### Building
To build the project and its examples, use the following commands:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---
## Quick Start
The following example demonstrates how to create a simple application with moving entities and a system to update them.

```cpp
#include <FECS/FECS.h>
#include <iostream>

// Define components
struct Position { float x, y; };
struct Velocity { float dx, dy; };

// A system that moves entities
void MoveSystem(FECS::Query<Position, Velocity> query)
{
    query.Each([](Position& pos, Velocity& vel)
    {
        pos.x += vel.dx;
        pos.y += vel.dy;
    });
}

int main()
{
    // Create a world
    FECS::World world;

    // Create an entity with Position and Velocity components
    world.Entities().Create()
        .Attach<Position>({ 0.0f, 0.0f })
        .Attach<Velocity>({ 1.0f, 0.5f })
        .Build();

    // Add a system to update the position
    world.Scheduler()
        .AddSystem()
        .WithQuery<Position, Velocity>()
        .Build(MoveSystem);

    // Run the scheduler for one frame
    world.Scheduler().Run(0.016f); // Pass delta time

    // You can also query manually
    world.View().Query<Position>().Each([](FECS::Entity id, Position& pos)
    {
        std::cout << "Entity " << id << " Position: " << pos.x << ", " << pos.y << std::endl;
    });

    return 0;
}
```

---
## Core Concepts

### The World
The `FECS::World` class is the heart of the ECS. It acts as a central hub, providing access to all the underlying managers:

```cpp
FECS::World world;

auto& entityManager = world.Entities();
auto& componentManager = world.Components();
auto& resourceManager = world.Resources();
auto& scheduleManager = world.Scheduler();
auto& viewManager = world.View();
```

### Entities
You can create entities using the `EntityManager`, accessed via `world.Entities()`. The `EntityBuilder` provides a fluent API for composing entities.

#### Creating Entities
```cpp
// Create a new entity and attach components
world.Entities().Create()
    .Attach<Position>({ 0.0f, 0.0f })
    .Attach<Velocity>({ 1.0f, 1.0f })
    .Build();
```

#### Advanced Entity Composition
The `EntityBuilder` supports a variety of methods for more complex entity creation:

```cpp
struct Player {};

world.Entities().Create()
    // Emplace a component in-place
    .Emplace<Position>(0.0f, 0.0f)
    // Add a tag (a component with no data)
    .Tag<Player>()
    // Conditionally attach components
    .When(isGodMode, [](FECS::EntityBuilder& builder)
    {
        builder.Attach(Invincibility{});
    })
    // Ensure a component exists, adding a default if it doesn't
    .Ensure(Health{100})
    // Modify an existing component
    .Patch<Position>([](Position& pos)
    {
        pos.x = 10.0f;
    })
    // Apply a custom function to the builder
    .Apply([](FECS::EntityBuilder& builder)
    {
        // ... custom logic ...
    })
    // Detach a component
    .Detach<UselessComponent>()
    .Build();
```

### Components
While the `EntityBuilder` is the primary way to work with components, you can also use the `ComponentManager` directly:

```cpp
auto& componentManager = world.Components();
FECS::Entity entity = world.Entities().Create().Build();

// Attach a component
componentManager.Attach<Position>(entity, {0.0f, 0.0f});

// Check if an entity has a component
if (componentManager.Has<Position>(entity))
{
    // Get a component
    Position& pos = componentManager.Get<Position>(entity);
}

// Detach a component
componentManager.Detach<Position>(entity);
```

### Systems & Scheduling
Systems contain the logic of your application. The `ScheduleManager` (`world.Scheduler()`) is used to define systems and their execution properties.

#### System Creation
The `SystemBuilder` provides a fluent API for defining systems:

```cpp
// A system that prints the position of entities
void PrintSystem(FECS::Query<Position> query)
{
    query.Each([](Position& pos)
    {
        std::cout << "Position: " << pos.x << ", " << pos.y << std::endl;
    });
}

world.Scheduler()
    .AddSystem()
    .WithQuery<Position>()
    .Build(PrintSystem);
```

#### System Dependencies
Systems can depend on resources. Use `Read<T>` for read-only access and `Write<T>` for read-write access.

```cpp
struct GameTime { float totalTime; };

void TimeSystem(GameTime& time)
{
    time.totalTime += 0.016f;
}

world.Scheduler()
    .AddSystem()
    .Write<GameTime>()
    .Build(TimeSystem);
```

#### System Execution
You can control when a system runs:

- **`Startup()`**: Runs once before the main loop.
- **`Update()`**: Runs every frame.
- **`Fixed()`**: Runs at a fixed time step.
- **`Timed(interval)`**: Runs at a specific interval.

```cpp
world.Scheduler().SetFixedStep(1.0f / 60.0f);

world.Scheduler().AddSystem().Startup().Build(InitGame);
world.Scheduler().AddSystem().Update().Build(HandleInput);
world.Scheduler().AddSystem().Fixed().Build(PhysicsUpdate);
world.Scheduler().AddSystem().Timed(1.0f).Build(SlowUpdate);
```

#### System Ordering
Systems can be organized into groups to control the execution order.

```cpp
enum SystemGroupID { INPUT = 0, PHYSICS = 1, RENDER = 2 };

world.Scheduler().SetExecutionOrder({
    SystemGroupID::INPUT,
    SystemGroupID::PHYSICS,
    SystemGroupID::RENDER,
});

world.Scheduler().AddSystem()
    .In(SystemGroupID::RENDER)
    .Build(RenderSystem);
```

### Views & Queries
The `ViewManager` (`world.View()`) is the entry point for querying entities.

```cpp
// Get a query for all entities with Position and Velocity
auto query = world.View().Query<Position, Velocity>();

// Iterate over the entities
query.Each([](FECS::Entity id, Position& pos, Velocity& vel)
{
    // ... logic ...
});
```

### Resources
The `ResourceManager` (`world.Resources()`) allows you to store and access global, non-entity data.

```cpp
// Define a resource
struct GameState { float isRunning = true; };

// Add the resource to the world
world.Resources().Add(GameState{});

// Access the resource directly
GameState& state = world.Resources().Get<GameState>();
state.isRunning = false;

// Access the resource in a system
void ControlSystem(GameState& state)
{
    if (/* some condition */)
    {
        state.isRunning = false;
    }
}

world.Scheduler()
    .AddSystem()
    .Write<GameState>()
    .Build(ControlSystem);
```

---
## Examples
The project includes several examples in the `examples` directory:
- **boids**: A classic boids simulation, demonstrating advanced features like system ordering and spatial hashing.
- **scheduling**: A simple example showcasing the system scheduler.
- **benchmark**: A set of benchmarks for various ECS operations.
- **tests**: A collection of tests for various features, and a good place to see the `EntityBuilder` in action.

---
## License
Distributed under the MIT License. See [LICENSE](LICENSE) for details.