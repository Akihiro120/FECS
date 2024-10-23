#include "src/FECS.h"

struct Velocity : public Component {
    float x;
    float y;

    Velocity(float x, float y) {
        this->x = x;
        this->y = y;
    }
};

struct Position : public Component {
    float x;
    float y;

    Position(float x, float y) {
        this->x = x;
        this->y = y;
    }
};

int main() {
    // initialize fecs
    FECS ecs;

    // entities
    EntityID entity = ecs.add_entity();

    // add testing
    ecs.add<Position>(entity, Position{0.0f, 0.0f});
    ecs.add<Velocity>(entity, Velocity{1.0f, 1.0f});

    // systems
    for (int i = 0; i < 100; i++) {
        ecs.query_system<Position, Velocity>([&](Position& pos, Velocity& vel) {
            // query position, and velocity entity
            // update the position
            pos.x += vel.x;
            pos.y += vel.y;
        });
    }

    // get the components
    std::cout << ecs.get<Position>(entity)->x << std::endl;
    std::cout << ecs.get<Position>(entity)->y << std::endl;
        
    // terminate fecs
    ecs.terminate();

    return 0;
}
