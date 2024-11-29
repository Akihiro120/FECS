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

### Benchmark
---
> [!NOTE]
> 32 is the maximum amount of components allowed per entity

The **Batch** testing is done with the maximum amount allowed by FECS. 
This is test the maximum capabilities of the ECS.

**Benchmark was performed on:**
- 12th Gen i5-12450H @ 4.40 GHz, 12 core
- Intel Alder Lake-P GT1 UHD Graphics
- 16 Gb memory
 
|                                                         | Benchmark Time |
| ------------------------------------------------------- | :------------: |
| Entity Creation - Single                                |  0.003278ms~   |
| Entity Creation - Batch (100)                           |  0.007058ms~   |
| Entity Creation - Batch (1000)                          |  0.040831ms~   |
| Entity Creation - Batch (10000)                         |  0.439732ms~   |
| Entity Destruction - Single                             |  0.001228ms~   |
| Entity Destruction - Batch (100)                        |  0.006346ms~   |
| Entity Destruction - Batch (1000)                       |  0.062376ms~   |
| Entity Destruction - Batch (10000)                      |  0.487587ms~   |
| Component Attachment - Single                           |  0.012816ms~   |
| Component Attachment - Batch (32)                       |  0.410112ms~   |
| Component Removal - Single                              |  0.001702ms~   |
| Component Removal - Batch (32)                          |  0.054464ms~   |
| System Querying - 1 Component, 100 Entities             |  0.019921ms~   |
| System Querying - 3 Systems, 1 Components, 100 Entities |  0.055975ms~   |

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
