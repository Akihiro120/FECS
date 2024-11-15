#include "src/FECS.h"
#include "unittests.h"
#include <chrono>
#include <thread>

struct Timer {
	 std::chrono::high_resolution_clock::time_point start, end;
	 std::chrono::duration<float> duration;

	 Timer() {
		  start = std::chrono::high_resolution_clock::now();
	 }

	 ~Timer() {
		  end = std::chrono::high_resolution_clock::now();
		  duration = end - start;

		  float ms = duration.count() * 1000.0f;
		  std::cout << "Process took" << ms << "ms" << std::endl;
	 }
};

int main() {
    // initialize fecs
    FECS ecs;
	 
	 int sample = 100;
	 for (int i = 0; i < sample; i++) {
		  auto e1 = ecs.add_entity();
		  ecs.add<Position>(e1, Position(0.0f, 0.0f));
	 }


	 Timer timer;

	 ecs.query_system<Position>([&](EntityID id, Position& position) {
	 });

	 ecs.query_system<Position>([&](EntityID id, Position& position) {
	 });

	 ecs.query_system<Position>([&](EntityID id, Position& position) {
	 });

	 ecs.terminate();

    return 0;
}
