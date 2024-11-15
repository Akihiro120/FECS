#pragma once
#include "src/FECS.h"
#include "src/debug_utils.h"

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

namespace Unittests {
	 void entity_add_remvove(FECS& ecs) {

		  // test adding and removing entities
		  auto e1 = ecs.add_entity();
		  auto e2 = ecs.add_entity();
		  auto e3 = ecs.add_entity();
		  auto e4 = ecs.add_entity();

		  // debug
		  FECSDebug::debug_entity_id("E1", e1);
		  FECSDebug::debug_entity_id("E2", e2);
		  FECSDebug::debug_entity_id("E3", e3);
		  FECSDebug::debug_entity_id("E4", e4);

		  // remove
		  ecs.remove_entity(e1);
		  ecs.remove_entity(e4);
		  std::cout << "Removed E1" << std::endl;
		  std::cout << "Removed E4" << std::endl;

		  // add
		  e1 = ecs.add_entity();
		  e4 = ecs.add_entity();
		  auto e5 = ecs.add_entity();

		  // debug
		  FECSDebug::debug_entity_id("E1", e1);
		  FECSDebug::debug_entity_id("E2", e2);
		  FECSDebug::debug_entity_id("E3", e3);
		  FECSDebug::debug_entity_id("E4", e4);
		  FECSDebug::debug_entity_id("E5", e5);
	 } 

	 void component_add_remove(FECS& ecs) {
		  auto e1 = ecs.add_entity(); 
		  ecs.add<Position>(e1, Position(0.0f, 0.0f));
		  ecs.add<Velocity>(e1, Velocity(1.0f, 1.0f));

		  FECSDebug::debug_entity_id("E1", e1);
		  // test adding duplicates
		  ecs.add<Position>(e1, Position(0.0f, 0.0f));
	 
		  // debug
		  std::cout << "e1: Position->x: " << ecs.get<Position>(e1)->x << std::endl; 
		  std::cout << "e1: Position->y: " << ecs.get<Position>(e1)->y << std::endl; 
		  std::cout << "e1: Velocity->x: " << ecs.get<Velocity>(e1)->x << std::endl; 
		  std::cout << "e1: Velocity->y: " << ecs.get<Velocity>(e1)->y << std::endl; 

		  // remove component
		  ecs.remove<Position>(e1);
		  std::cout << "Removed Position from E1" << std::endl;
		  
		  // test removing nonexisting components
		  ecs.remove<Position>(e1);

		  // test adding after removing
		  ecs.add<Position>(e1, Position(0.0f, 0.0f));

		  // test removing entity
		  auto e2 = ecs.add_entity(); 
		  ecs.add<Position>(e2, Position(0.0f, 0.0f));

		  ecs.remove_entity(e1);

		  e1 = ecs.add_entity(); 
		  ecs.add<Position>(e1, Position(0.0f, 0.0f));
		  ecs.add<Velocity>(e1, Velocity(1.0f, 1.0f));
		  FECSDebug::debug_entity_id("E1", e1);
	 }

	 void component_querying(FECS& ecs) {
		  auto e1 = ecs.add_entity(); 
		  ecs.add<Position>(e1, Position(1.0f, 1.0f));
		  ecs.add<Velocity>(e1, Velocity(1.0f, 1.0f));

		  auto e2 = ecs.add_entity(); 
		  ecs.add<Position>(e2, Position(0.0f, 0.0f));

		  FECSDebug::debug_entity_id("E1", e1);
		  FECSDebug::debug_entity_id("E2", e2);

		  ecs.query_system<Position, Velocity>([&](EntityID id, Position& position, Velocity& velocity) {
				position.x += velocity.x;
				position.y += velocity.y;

				std::cout << "Queried Entity ID: " << id << std::endl;
		  });
		  // expected result, only 1 entity should be queried
		  
		  // check if the data has been changed
		  // debug
		  std::cout << "e1: Position->x: " << ecs.get<Position>(e1)->x << std::endl; 
		  std::cout << "e1: Position->y: " << ecs.get<Position>(e1)->y << std::endl; 
		  std::cout << "e1: Velocity->x: " << ecs.get<Velocity>(e1)->x << std::endl; 
		  std::cout << "e1: Velocity->y: " << ecs.get<Velocity>(e1)->y << std::endl; 
		  // expected result, position.x and y should be 1 and 1
	 }

	 void removing_entity_in_query(FECS& ecs) {

		  auto e1 = ecs.add_entity(); 
		  ecs.add<Position>(e1, Position(1.0f, 1.0f));
		  ecs.add<Velocity>(e1, Velocity(1.0f, 1.0f));
		  auto e2 = ecs.add_entity(); 
		  ecs.add<Position>(e2, Position(1.0f, 1.0f));
		  ecs.add<Velocity>(e2, Velocity(1.0f, 1.0f));
		  auto e3 = ecs.add_entity(); 
		  ecs.add<Position>(e3, Position(1.0f, 1.0f));
		  ecs.add<Velocity>(e3, Velocity(1.0f, 1.0f));

		  for (int i = 0; i < 2; i++) {
				ecs.query_system<Position, Velocity>([&](EntityID id, Position& position, Velocity& velocity) {
					 std::cout << "Queried Entity ID: " << id << std::endl;
					 
					 if (id == 1) {
						  ecs.remove_entity(id);
					 }
				});
		  }
	 }
}
