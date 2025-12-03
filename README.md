# FECS - A Fast Entity Component System

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
  - [Systems & Scheduling](#systems--scheduling)
  - [Queries](#queries)
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
- **Fluent System Builder API**: Expressive and type-safe API for defining systems and their scheduling.
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
void MoveSystem(FECS::Query<Position&, const Velocity&> query) {
    query.Each([](Position& pos, const Velocity& vel) {
        pos.x += vel.dx;
        pos.y += vel.dy;
    });
}

int main() {
    // Create a world
    FECS::World world;

    // Create an entity with Position and Velocity components
    world.Entities().Create()
        .Attach<Position>({ 0.0f, 0.0f })
        .Attach<Velocity>({ 1.0f, 0.5f });

    // Add a system to update the position
    world.Scheduler().AddSystem()
        .Update()
        .Build(MoveSystem);

    // Run the scheduler for one frame
    world.Scheduler().RunStartup();
    world.Scheduler().Run(0.016f); // Pass delta time

    // You can also query manually
    auto query = world.Query<const Position&>();
    query.Each([](const Position& pos){
        std::cout << "Position: " << pos.x << ", " << pos.y << std::endl;
    });

    return 0;
}
```

---
## Core Concepts

### The World
The `FECS::World` class is the heart of the ECS. It acts as a central hub, providing access to all the underlying managers for entities, components, resources, and scheduling.

### Entities
You can create entities using the `EntityManager`, accessed via `world.Entities()`.

```cpp
// Create a new entity and attach components
world.Entities().Create()
    .Attach<Position>({ 0.0f, 0.0f })
    .Attach<Velocity>({ 1.0f, 1.0f });
```

### Systems & Scheduling
Systems contain the logic of your application. The `ScheduleManager` (`world.Scheduler()`) is used to define systems, their execution type, and their order.

```cpp
// A system function
void MySystem(FECS::Query<MyComponent&> query) {
    // ... logic ...
}

// Add the system to the scheduler
world.Scheduler().AddSystem()
    .Update() // Or .Startup(), .Fixed(), .Timed()
    .Build(MySystem);
```
Systems are built using a fluent `SystemBuilder` API. You can specify dependencies on resources and other parts of the world.

```cpp
// A system that needs access to a global resource
void MySystemWithResource(MyResource& resource, FECS::Query<MyComponent&> query) {
    // ... logic ...
}

world.Scheduler().AddSystem()
    .Update()
    .Write<MyResource>() // Request write access to the resource
    .Build(MySystemWithResource);

```

### Queries
Queries are the primary way to access and iterate over entities with a specific set of components. You can create queries manually or have them passed to your systems.

**1. Manual Queries:**
Use `world.Query<...>()` to get a query object that you can iterate over.

```cpp
auto query = world.Query<Position&, const Velocity&>();
query.Each([](Position& pos, const Velocity& vel) {
    // ... logic ...
});
```

**2. System Queries:**
When building a system, you can specify a query that will be automatically passed as an argument to your system function.

```cpp
void MySystem(FECS::Query<Position&> query) {
    query.Each([](Position& pos) { /* ... */ });
}

world.Scheduler().AddSystem()
    .Update()
    // No .WithQuery<>() is needed, it's inferred from the function signature!
    .Build(MySystem);
```

### Resources
The `ResourceManager` (`world.Resources()`) allows you to store and access global, non-entity data in a type-safe manner.

```cpp
// Define a resource
struct GameState { bool isRunning = true; };

// Add the resource to the world
world.Resources().Add(GameState{});

// Access the resource in a system
void ControlSystem(GameState& state) {
    if (/* some condition */) {
        state.isRunning = false;
    }
}

world.Scheduler().AddSystem().Update().Build(ControlSystem);
```

---
## Examples
The project includes several examples in the `examples` directory. The `boids` example is a great starting point for understanding how to use FECS in a real-world application, demonstrating advanced features like system ordering and spatial hashing.

---
## License
Distributed under the MIT License. See [LICENSE](LICENSE) for details.
