# Functional Entity Component System (FECS)

A lighweight and flexible Entity Component System (ECS) single header framework built for flexibility and high-performance game development. This framework provides an efficient way to manage entities, components, and systems.

FECS utilises complex data structures like, sparse sets, and bitsets to allow for fast entity querying, and creation.

> [!WARNING]
> This is made to fit the needs of my projects, if something isn't right or needs implementation, create an **Issue**.

### Features
- **Entity Management**
- **Component-Based Design**
- **Flexible System Processing**
- **Efficient Memory Layout**

### Table of Contents
- Wrappers
- Getting Started
- Benchmark
- Usage
    - Entities
    - Components
    - Systems
 
### Wrappers
---
|                                       | Language |
| --------------------------------------| :------: |
| https://github.com/Akihiro120/FECS-rs | Rust     |

### Getting Started
---
Download the (latest release)[https://github.com/Akihiro120/FECS/releases/tag/v1.0.1].

Include the FECS library, ```#include <FECS.h>```

Initialize the FECS context
``` c++
FECS fecs;

// .. insert code here

fecs.terminate();
```

### Usage - Entities
---
Entities are uint32_t IDs.

``` c++
EntityID entity = fecs.add_entity();
```

### Usage - Components
To create a component, you create a class/struct and inherit the **Component** base structure.

```
struct Position : public Component {
    float x;
    float y;

    Position(float x, float y) {
        this->x = x;
        this->y = y;
    }
};
```

You can then attach a component to an entity

``` c++
fecs.add<Position>(entity, Position{0.0f, 0.0f});
```

You can remove with
``` c++
fecs.remove<Position>(entity);
```

An entity can only have one of any component type.

### Usage - Systems
---
For systems, its a unique and functional process. There is no concept of a system class.

To create a system, you have to query components, then use a lambda function to utilize it.

``` c++
ecs.query_system<Position, Velocity>(
    [&](EntityID id, Position& pos, Velocity& vel) {
    // apply function
    pos.x += vel.x;
    pos.y += vel.y;
});
```

This allows for flexible and functional system management and creation.

You must always include the EntityID as a lambda parameter.
