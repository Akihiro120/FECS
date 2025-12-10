# Building a Modular Dungeon Crawler

This document outlines the definitive workflow for building a simple, text-based dungeon crawler using a modular, data-oriented, and scalable Entity-Component-System (ECS) architecture. This is a complete, end-to-end example with no hidden logic.

---

## 1. Core Principles & Architecture

### The Rules
1.  **Components**: Contain only per-entity data. (e.g., `Position`, `Stats`).
2.  **Resources**: Contain only global or module-wide data. (e.g., `Map`, `GameLog`).
3.  **Systems**: Contain only logic. They are stateless functions that operate on components and resources.

### Module-Based Structure
The project is organized into self-contained **Modules**. Shared data lives in a `Core` module, while features like `Player` or `Combat` are independent, ensuring they are decoupled and reusable. Every definition lives in its own file.

```
/ 
├── modules/
│   ├── core/
│   ├── dungeon/
│   ├── actor/
│   ├── player/
│   ├── monster/
│   ├── game/
│   └── rendering/
├── main.cpp
└── ... (core ECS library)
```

### The Turn-Based Execution Pipeline
The `main` function defines a series of stages that control the flow of a single game turn.
```cpp
enum class TurnSystemStage {
    PreTurn,        // Cleanup from last turn
    PlayerInput,    // Read player input
    PlayerAction,   // Execute player's intended action
    MonsterAction,  // Execute AI actions
    PostTurn,       // Resolve combat, handle level ups
    Render          // Draw the final world state
};
```

---

## 2. Foundational Modules (`Core`, `Dungeon`, `Actor`)

### The `Core` Data Module
**File: `/modules/core/components/position.h`**
```cpp
#pragma once
namespace Module::Core::Component { struct Position { int x = 0, y = 0; }; }
```
**File: `/modules/core/mod.h`**
```cpp
#pragma once
#include "components/position.h"
namespace Module::Core { inline void Integrate(FECS::World&) {} }
```

### The `Dungeon` Data Module
**File: `/modules/dungeon/resources/map.h`**
```cpp
#pragma once
#include <vector>
namespace Module::Dungeon::Resource { struct Map { int w=0, h=0; std::vector<char> tiles; }; }
```
**File: `/modules/dungeon/mod.h`**
```cpp
#pragma once
#include "resources/map.h"
namespace Module::Dungeon { inline void Integrate(FECS::World&) {} }
```

### The `Actor` Data Module
**File: `/modules/actor/components/stats.h`**
```cpp
#pragma once
namespace Module::Actor::Component { struct Stats { int hp = 10, maxHp = 10, attack = 2; }; }
```
**File: `/modules/actor/mod.h`**
```cpp
#pragma once
#include "components/stats.h"
namespace Module::Actor { inline void Integrate(FECS::World&) {} }
```

---

## 3. Feature Modules (`Player`, `Monster`, `Game`, `Rendering`)

### The `Game` Module
This module handles game-wide state, like whether the game should continue running.
**File: `/modules/game/resources/game_state.h`**
```cpp
#pragma once
namespace Module::Game::Resource { struct GameState { bool isRunning = true; }; }
```
**File: `/modules/game/mod.h`**
```cpp
#pragma once
#include "resources/game_state.h"
namespace Module::Game {
    inline void Integrate(FECS::World& world) {
        world.Resources().Emplace<Resource::GameState>();
    }
}
```

### The `Player` Module
**File: `/modules/player/components/player_tag.h`**
```cpp
#pragma once
namespace Module::Player::Component { struct PlayerTag {}; }
```
**File: `/modules/player/resources/action_request.h`**
```cpp
#pragma once
namespace Module::Player::Resource { struct ActionRequest { enum Type { NONE, MOVE } type = NONE; int x=0, y=0; char key=0; }; }
```
**File: `/modules/player/systems/process_input.h`**
```cpp
#pragma once
#include <iostream>
#include "../resources/action_request.h"
#include "../../game/resources/game_state.h"

namespace Module::Player::System {
    inline void ProcessInput(Resource::ActionRequest& request, Game::Resource::GameState& gameState) {
        std::cout << "Your move (w/a/s/d) or 'q' to quit: ";
        std::cin >> request.key;

        request.x=0; request.y=0;
        request.type = Resource::ActionRequest::MOVE;
        if (request.key == 'w') request.y = -1;
        else if (request.key == 'a') request.x = -1;
        else if (request.key == 's') request.y = 1;
        else if (request.key == 'd') request.x = 1;
        else if (request.key == 'q') gameState.isRunning = false;
        else request.type = Resource::ActionRequest::NONE;
    }
}
```
**File: `/modules/player/systems/execute_move.h`**
```cpp
#pragma once
#include "../resources/action_request.h"
#include "../../core/components/position.h"
#include "../../dungeon/resources/map.h"
#include "../components/player_tag.h"

namespace Module::Player::System {
    inline void ExecuteMove(const Resource::ActionRequest& request, Core::Component::Position& pos, const Dungeon::Resource::Map& map) {
        if (request.type != Resource::ActionRequest::MOVE) return;
        int newX = pos.x + request.x, newY = pos.y + request.y;
        if (map.tiles[newY * map.w + newX] == '.') { pos.x = newX; pos.y = newY; }
    }
}
```
**File: `/modules/player/mod.h`**
```cpp
#pragma once
#include "../../FECS.h"
#include "resources/action_request.h"
#include "systems/process_input.h"
#include "systems/execute_move.h"
#include "components/player_tag.h"

enum class TurnSystemStage;

namespace Module::Player {
    inline void Integrate(FECS::World& world) {
        world.Resources().Emplace<Resource::ActionRequest>();
        world.Scheduler().AddSystem().In(TurnSystemStage::PlayerInput).Build(System::ProcessInput);
        world.Scheduler().AddSystem().In(TurnSystemStage::PlayerAction).With<Component::PlayerTag>().Build(System::ExecuteMove);
    }
}
```

### The `Monster` Module
**File: `/modules/monster/components/monster_tag.h`**
```cpp
#pragma once
namespace Module::Monster::Component { struct MonsterTag {}; }
```
**File: `/modules/monster/mod.h`**
```cpp
#pragma once
#include "components/monster_tag.h"
// For now, this is a data-only module. AI systems would be added here.
namespace Module::Monster { inline void Integrate(FECS::World&) {} }
```

### The `Rendering` Module
**File: `/modules/rendering/systems/render.h`**
```cpp
#pragma once
#include <iostream>
#include <vector>
#include "../../dungeon/resources/map.h"
#include "../../core/components/position.h"
#include "../../player/components/player_tag.h"
#include "../../monster/components/monster_tag.h"
#include "../../actor/components/stats.h"

namespace Module::Rendering::System {
    inline void Render(const Dungeon::Resource::Map& map, const FECS::World& world) {
        system("cls"); // Or "clear"
        std::vector<char> buffer = map.tiles;
        world.Query<const Core::Component::Position, const Monster::Component::MonsterTag>().Each(
            [&](auto& pos, auto&) { buffer[pos.y*map.w+pos.x] = 'G'; });
        world.Query<const Core::Component::Position, const Player::Component::PlayerTag>().Each(
            [&](auto& pos, auto&) { buffer[pos.y*map.w+pos.x] = '@'; });
        
        for (int y=0; y<map.h; ++y) {
            for (int x=0; x<map.w; ++x) { std::cout << buffer[y*map.w+x]; }
            std::cout << std::endl;
        }
        
        const auto* playerStats = world.Query<const Actor::Component::Stats, const Player::Component::PlayerTag>().Get();
        std::cout << "\nPlayer HP: " << playerStats->hp << "/" << playerStats->maxHp << std::endl;
    }
}
```
**File: `/modules/rendering/mod.h`**
```cpp
#pragma once
#include "../../FECS.h"
#include "systems/render.h"

enum class TurnSystemStage;

namespace Module::Rendering {
    inline void Integrate(FECS::World& world) {
        world.Scheduler().AddSystem().In(TurnSystemStage::Render).Build(System::Render);
    }
}
```

---
## 4. Assembling and Running

**File: `/main.cpp`**
```cpp
#include <iostream>
#include "FECS.h"

#include "modules/core/mod.h"
#include "modules/dungeon/mod.h"
#include "modules/actor/mod.h"
#include "modules/game/mod.h"
#include "modules/player/mod.h"
#include "modules/monster/mod.h"
#include "modules/rendering/mod.h"

enum class TurnSystemStage { PlayerInput, PlayerAction, MonsterAction, Render };

void CreateWorld(FECS::World& world) {
    auto& map = world.Resources().Emplace<Module::Dungeon::Resource::Map>();
    map.w = 20; map.h = 10;
    map.tiles.assign(map.w * map.h, '.');
    for(int x=0; x<map.w; ++x) { map.tiles[x] = '#'; map.tiles[(map.h-1)*map.w+x] = '#'; }
    for(int y=0; y<map.h; ++y) { map.tiles[y*map.w] = '#'; map.tiles[y*map.w+map.w-1] = '#'; }

    world.Entities().Create()
        .Emplace<Module::Core::Component::Position>(2, 2)
        .Emplace<Module::Actor::Component::Stats>()
        .Tag<Module::Player::Component::PlayerTag>()
        .Build();

    world.Entities().Create()
        .Emplace<Module::Core::Component::Position>(10, 5)
        .Emplace<Module::Actor::Component::Stats>(5, 5, 1)
        .Tag<Module::Monster::Component::MonsterTag>()
        .Build();
}

int main() {
    FECS::World world;
    
    Module::Core::Integrate(world);
    Module::Dungeon::Integrate(world);
    Module::Actor::Integrate(world);
    Module::Game::Integrate(world);
    Module::Player::Integrate(world);
    Module::Monster::Integrate(world);
    Module::Rendering::Integrate(world);

    world.Scheduler().SetExecutionOrder({
        TurnSystemStage::PlayerInput, TurnSystemStage::PlayerAction,
        TurnSystemStage::MonsterAction, TurnSystemStage::Render
    });
    
    CreateWorld(world);
    
    // The main loop is now driven entirely by the GameState resource.
    // The ProcessInput system will set isRunning to false when the player quits.
    while(world.Resources().Get<Module::Game::Resource::GameState>().isRunning) {
        world.Scheduler().Run(0.0f);
    }
    
    std::cout << "Thanks for playing!\n";
    return 0;
}
```

This final structure provides a complete, fully decoupled, and scalable foundation for a game. This is the primary strength of a data-oriented, modular ECS design.
