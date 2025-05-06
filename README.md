# FECS - A Functional Entity Component System

> A functional and lightweight C++ ECS framework.
> Optimized to *"consume 100% of your memory"* and usage in real-time games.

---
## Table of Contents
- [Why FECS?](#why-fecs)
- [Features](#features)
- [Installation](#installation)
- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
- [License](#license)

---
## Why FECS
Most ECS libraries trade off binary size for convenience.  
**FECS** flips that trade-off: by packing data densely and avoiding hidden allocations, it delivers **raw speed** at the cost of larger binaries—perfect when you need unlimited entities and components in a real-time engine.

---
## Features
- **Header-Only**: Drop all files into your include path.
- **Zero Hidden Allocations**: Every component pool is pre-allocated and determined at compile time.
- **Unlimited Entities and Components**: Indexed by 32-bit IDs + versioning.
- **Control**: Full control over what gets allocated, and how much.
- **Memory Efficiency**: Densely packed data + effective caching.

---
## Installation
Download FECS via Clone
``` bash
git clone https://github.com/Akihiro120/FECS.git
```

FECS is a `Single Header` library so you only have to add it to your `Vendor/Includes` directory.

To build example projects, simply run 
e.g. via `Ninja`

``` bash
mkdir build
cd build

cmake -G Ninja ..
ninja
```

---
## Prerequisites
- C++17 compatible compiler (GCC >= 7, Clang >= 6, MSVC >= 2017)
- CMake >= 3.25 (optional, if you want to build `Example` projects)

---
## Quick Start
``` cpp
#include <fecs/fecs.hpp>

struct Position { float x, y; };
struct Velocity { float dx, dy; };

int main()
{
    FECS::Registry registry;

    // Create 1000 entities with Position + Velocity
    for (int i = 0; i < 1000; i++)
    {
        FECS::Entity e = registry.CreateEntity();
        registry.Attach<Position>(e, Position{0.0f, 0.0f});
        registry.Attach<Velocity>(e, Velocity{1.0f, 0.5f});
    }

    // Simple system
    FECS::View<Position, Velocity> sys = registry.View<Position, Velocity>();
    sys.Each([&](Entity e, Position& pos, Velocity& vel){
        pos.x += vel.dx;
        pos.y += vel.dy;
    });
}
```

### Usage
1. **Define** your Components structures.
2. **Create** a `FECS::Registry` instance.
3. **Spawn** entities with `.CreateEntity()`, then `.Attach<Component>(...)`.
4. **Process** groups of components via `.View<...>().Each([&](...){})`.

For *full details*, see the [API Reference](#api-reference).

---
## API Reference
All public classes and functions are documented in [docs/api.md](docs/api.md). Highlights include:
- `FECS::Registry` - The Core Manager
- `registry.CreateEntity()`, `registry.DestroyEntity(entity)`
- `registry.Attach<T>(...)`, `registry.Detach<T>(entity)`
- `registry.View<Ts...>()`, `.Each(...)`

---
## License
Distributed under the MIT License. See LICENSE for details.
