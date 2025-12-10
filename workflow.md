# FECS Workflow: A Data-Oriented Approach for Game Development

This document outlines a recommended workflow for using `fecs`, an Entity-Component-System (ECS) library, with a focus on Data-Oriented Design (DOD). We'll use a simple game as a practical example.

## Core Concepts of ECS and Data-Oriented Design

In a traditional Object-Oriented approach, you might create a `Player` class that contains both data (position, health, etc.) and methods (move, takeDamage, etc.).

In ECS and DOD, we decouple data from behavior:

*   **Entities:** Simple identifiers. An entity is not a "thing"; it's just a unique ID. You can think of it as a key in a database table.
*   **Components:** Pure data. These are structs or classes that hold the state of your game objects. They contain no logic. Examples: `Position`, `Velocity`, `Health`, `PlayerControllable`.
*   **Resources:** Global, unique data that systems can access. Unlike components, resources are not tied to specific entities. Examples: `DeltaTime`, `InputState`, `GameSettings`. They are typically accessed via `World::Resources().Get<T>()`.
*   **Systems:** Pure logic. These are functions that operate on entities that have a specific set of components. A `MovementSystem` would operate on all entities that have both a `Position` and a `Velocity` component.
*   **World:** The container for all entities, components, and systems. It manages memory and executes the systems.

This approach leads to better performance due to cache-friendly memory layouts and promotes highly reusable, decoupled code.

---

## Workflow Example: A Simple 2D Game

Let's imagine a simple game where a player-controlled character moves, and enemies patrol back and forth.

### Step 1: Define Your State (Components)

First, think about the *data* that defines your game objects. Create simple structs or classes for each piece of state.

**File: `src/Components.h`**
```cpp
#pragma once

// Represents a 2D position in the world.
struct Position {
    float x = 0.0f;
    float y = 0.0f;
};

// Represents a 2D velocity.
struct Velocity {
    float dx = 0.0f;
    float dy = 0.0f;
};

// Represents the health of an entity.
struct Health {
    int current = 100;
    int max = 100;
};

// A "tag" component to identify the player entity. It has no data.
struct PlayerTag {};

// A component for enemy-specific data, like patrol points.
struct EnemyAI {
    float patrol_start_x;
    float patrol_end_x;
    float speed = 50.0f;
};
```

**Tip:** Keep components as Plain Old Data (POD). They should not have methods that implement game logic.

### Step 2: Define Your Behavior (Systems)

Systems are free-floating functions that contain the game logic. They query the `World` for entities that have the components they care about.

**File: `src/Systems.h`**
```cpp
#pragma once
#include "Components.h"
#include <iostream>

// A system that updates the position of any entity with a Position and Velocity.
void movement_system(FECS::Query<Position&, const Velocity&> query, const float& delta_time) {
    query.each([&](Position& pos, const Velocity& vel) {
        pos.x += vel.dx * delta_time;
        pos.y += vel.dy * delta_time;
    });
}

// A system that controls enemy movement.
void enemy_ai_system(FECS::Query<Position&, Velocity&, const EnemyAI&> query) {
    query.each([&](Position& pos, Velocity& vel, const EnemyAI& ai) {
        if (pos.x <= ai.patrol_start_x) {
            vel.dx = ai.speed; // Move right
        } else if (pos.x >= ai.patrol_end_x) {
            vel.dx = -ai.speed; // Move left
        }
    });
}

// A system that "renders" the position of entities.
void render_system(FECS::Query<const Position&> query) {
    // In a real game, you would call your graphics API here.
    // We will just print to the console.
    std::cout << "--- Render Frame ---" << std::endl;
    query.each([&](const Position& pos) {
        std::cout << "Entity rendered at (" << pos.x << ", " << pos.y << ")" << std::endl;
    });
    std::cout << "--------------------" << std::endl;
}
```

**Tip:** Systems should be stateless. All the state they operate on should come from the components they query. The `FECS::Query` object is how you get that data from the `World`.

### Step 3: Assemble the World and Create Entities

In your main application file, you will create a `FECS::World` object, define your entities and their components, and schedule your systems.

**File: `src/main.cpp`**
```cpp
#include "FECS/FECS.h"
#include "Components.h"
#include "Systems.h"
#include <thread>
#include <chrono>

int main() {
    // 1. Create the World
    FECS::World world;

    // 2. Components are implicitly registered when first added to an entity.
    //    No explicit registration calls are typically needed.

    // 3. Create Entities and add Components
    
    // Create the Player entity
    world.Entities().Create()
        .set<Position>({ .x = 100.0f, .y = 50.0f })
        .set<Velocity>({ .dx = 0.0f, .dy = 0.0f })
        .set<Health>({ .current = 100, .max = 100 });

    // Create an Enemy entity
    world.Entities().Create()
        .set<Position>({ .x = 0.0f, .y = 100.0f })
        .set<Velocity>({ .dx = 50.0f, .dy = 0.0f }) // Start moving right
        .set<EnemyAI>({ .patrol_start_x = 0.0f, .patrol_end_x = 200.0f });

    // 4. Build and Schedule the Systems
    // Use the SystemBuilder from the ScheduleManager to add systems
    world.Scheduler().AddSystem()
        .WithQuery<Position&, Velocity&, const EnemyAI&>()
        .Build(enemy_ai_system);

    world.Scheduler().AddSystem()
        .WithQuery<Position&, const Velocity&>()
        .Read<float>() // To read the delta_time resource
        .Build(movement_system);

    world.Scheduler().AddSystem()
        .WithQuery<const Position&>()
        .Build(render_system);

    // 5. The Game Loop
    float delta_time = 1.0f / 60.0f; // Assume 60 FPS for simplicity
    // Resources are global, unique data that systems can access.
    // Here, delta_time is registered as a resource.
    world.Resources().Set<float>(delta_time);

    for (int i = 0; i < 10; ++i) {
        // In a real game, you would calculate delta_time each frame and update the resource.
        // E.g., world.Resources().Set<float>(calculated_delta_time);
        // User input handling would go here, modifying components (e.g., Player's Velocity).

        world.Scheduler().Run(world.Resources().Get<float>()); // Execute all scheduled systems in order via the ScheduleManager

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
```

### Step 4: Project Organization for Scalability

As your project grows, a clear and consistent organization scheme becomes crucial. This section outlines a scalable approach for both your file system and your C++ namespaces.

#### 1. Directory Structure: Group by Feature/Module

Instead of generic folders like `components/` and `systems/`, we recommend organizing your code by game features or modules. This keeps related code together and makes the project easier to navigate and maintain.

A recommended structure for a growing project:

```
src/
├── modules/
│   ├── Core/
│   │   ├── Components/
│   │   │   ├── Position.h
│   │   │   └── Velocity.h
│   │   ├── Systems/
│   │   │   └── MovementSystem.h
│   │   └── Resources/
│   │       └── Time.h
│   │
│   ├── Physics/
│   │   ├── Components/
│   │   │   └── Collider.h
│   │   └── Systems/
│   │       └── CollisionSystem.h
│   │
│   └── Rendering/
│       ├── Components/
│       │   └── Sprite.h
│       └── Systems/
│           └── RenderSystem.h
│
└── main.cpp
```

*   **`modules/`**: Contains the core logic of your game, broken down into self-contained modules (`Core`, `Physics`, `Rendering`, etc.).
*   **`modules/[ModuleName]/Components/`**: Contains all components specific to that module.
*   **`modules/[ModuleName]/Systems/`**: Contains all systems specific to that module.
*   **`modules/[ModuleName]/Resources/`**: Contains module-specific, world-global data.

This structure promotes modularity and makes it clear where to find code related to a specific piece of functionality.

#### 2. Namespacing Strategy: Mirror the Directory Structure

Use extensive namespacing to prevent name collisions and provide context. The namespace structure should directly mirror the directory structure.

**Syntax:** `ModuleName::Category::TypeName`

**Example: `Position.h`**
```cpp
// Located at: src/modules/Core/Components/Position.h

#pragma once

namespace Core::Component {

struct Position {
    float x = 0.0f;
    float y = 0.0f;
};

} // namespace Core::Component
```

**Example: `RenderSystem.h`**
```cpp
// Located at: src/modules/Rendering/Systems/RenderSystem.h

#pragma once
#include "Core/Components/Position.h"   // Note the include path
#include "Rendering/Components/Sprite.h"

namespace FECS { class World; } // Forward declaration

namespace Rendering::System {

void render_system(FECS::World& world);

} // namespace Rendering::System
```

**Example: `main.cpp`**
```cpp
#include "modules/Core/Components/Position.h"
#include "modules/Core/Components/Velocity.h"
#include "modules/Rendering/Components/Sprite.h"
#include "modules/Core/Systems/MovementSystem.h"
#include "modules/Rendering/Systems/RenderSystem.h"

// Using aliases for conciseness
namespace CoreComp = Core::Component;
namespace RenderSys = Rendering::System;

int main() {
    FECS::World world;

    // Components are implicitly registered when first added to an entity.
    // No explicit registration calls are typically needed.

    // Schedule systems using the SystemBuilder
    world.Scheduler().AddSystem()
        .WithQuery<CoreComp::Position&, const CoreComp::Velocity&>()
        .Build(Core::System::movement_system);

    // Assuming RenderSystem.h defines render_system(FECS::Query<const Rendering::Component::Sprite&>)
    world.Scheduler().AddSystem()
        .WithQuery<const Rendering::Component::Sprite&>()
        .Build(RenderSys::render_system);
    
    // ... create entities and run the world
}
```

#### Key Takeaways

*   **State vs. Logic:** Be disciplined. If you find yourself adding a function to a component struct, ask yourself: "Can this be a system instead?" The answer is almost always yes. The only exception might be simple helper functions that don't implement any game logic (e.g., a `Position::distance_to(other)` function).
*   **The World is King:** Pass the `World` object (or queries from it) to your systems. Avoid global state. The ECS `World` is your single source of truth for all game state.
*   **Modularity is Key:** By combining a feature-based directory structure with a parallel namespacing strategy, you create a codebase that is organized, scalable, and easy for new developers to understand.

This workflow helps you build games that are easier to reason about, debug, and extend, all while leveraging the high-performance, cache-friendly benefits of data-oriented design.
