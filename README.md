FECS - A Lightweight Entity Component System

FECS is a lightweight, header-only Entity Component System (ECS) written in modern c++. It leverages a custom sparse set data structure to acheieve near constant-time insertion, removal and retrieval of components. FECS has been designed with clarity and efficiency in mind, suitable for small-to-medium projects and for educational purposes.

--

### Features
- **Entity Management**
    Simple Interface for creating and removing entities with unique IDs.
- **Component Storage via Sparse Set**
    Utilises a fixed-sized sparse set for O(1) access and update, minimising memory overhead.
- **Flexible Component Attachment**
    Attach and Detach components to/from entities via a templated API, using `std::bitset` to track component signatures
- **Query System**
    Query entities that match a given set of components types with an elegant template-based parameter pack.
- **Modern C++ Techniques**
    Uses templates, assertions, and RAII for robust maintainable code.

--

### Getting Started
#### Prerequisites
- A C++ 17 compliant compiler.
- Basic understanding of C++ templates and the ECS design pattern.

#### Installation
FECS is header-only. Simply clone the repository and include the relevant headers in your project:
``` bash
git clone https://github.com/Akihiro120/FECS.git
```

Then, in your C++ source file:
``` bash
#include "fecs.h"
```

-- 
### Usage
#### Preprocessors
You can define and change the MAX size for the Sparse, and the MAX number of components for FECS.
``` c++
#define MAX_SPARSE_SIZE 2000
#define MAX_COMPONENTS 100
```
#### Creating an Entity
``` c++
FECS ecs;
Entity entity = ecs.create_entity();
```
#### Attaching a Component
Define a component type (e.g. a struct) and attach it to an entity:
``` c++
struct Position {
    float x, y;
};

ecs.attach<Position>(entity, {10.0f, 20.0f});
```
#### Detaching a Component
``` c++
ecs.detach<Position>(entity);
```
#### Accessing a Component
``` c++
Position* pos = ecs.get<Position>(entity);
if (pos) {
    // use *pos
}
```
#### Querying Entities
Iterate over entities with specific components using the query interface:
``` c++
ecs.query<Position, Velocity>([](Entity e, Position& pos, Velocity& vel) {
    // process entity e and its Position and Velocity Component
});
```

--
### Contributing
Contributions to FECS are most welcome. If you have ideas for improvement or have identified any issues, please open an issue or submit a pull request. When contributing, please maintain the current academic tone and clarity of documentation.

--
### License
This project is licensed under the MIT Licence – see the LICENCE file for details.
