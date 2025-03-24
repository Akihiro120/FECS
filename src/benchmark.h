#pragma once
#include <chrono>
#include <iostream>

struct Benchmark {
	 std::chrono::high_resolution_clock::time_point start, end;
	 std::chrono::duration<float> duration;

	 Benchmark() {
		  start = std::chrono::high_resolution_clock::now();
	 }

	 ~Benchmark() {
		  end = std::chrono::high_resolution_clock::now();
		  duration = end - start;

		  float ms = duration.count() * 1000.0f;
		  std::cout << "Process took: " << ms << "ms" << std::endl;
	 }
};
