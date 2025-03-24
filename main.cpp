#include <cassert>
#include "src/benchmark.h"
#include "src/fecs.h"
#include "src/sparse_set.h"

int main() {
	SparseSet<int> set;

	// sparse set insert
	int sample = 1000;
	{
		Benchmark timer;
		
		for (int i = 0; i < sample; i++) {
			set.insert(i, 32 * i);
		}
		std::cout << "Sparse Set Insert (" + std::to_string(sample) + "), ";
		// 0.024ms avg
	}

	// sparse set get
	{
		Benchmark timer;
		for (int i = 0; i < sample; i++) {
			set.get(i);
		}
		std::cout << "Sparse Set Get (" + std::to_string(sample) + "), ";
		// 0.00033ms avg
	}

	// fecs entity create
	FECS fecs;
	{
		Benchmark timer;
		for (int i = 0; i < sample; i++) {
			fecs.create_entity();
		}

		std::cout << "FECS Entity Create (" + std::to_string(sample) + "), ";
		// 0.03ms avg
	}
	
	// fecs component insert
	{
		Benchmark timer;
		for (int i = 0; i < sample; i++) {
			fecs.attach<int>(i, 32 * i);
		}

		std::cout << "FECS Component Attach (" + std::to_string(sample) + "), ";
		// 0.04ms avg
	}
	
	// fecs get
	{
		Benchmark timer;
		for (int i = 0; i < sample; i++) {
			fecs.get<int>(i);
		}

		std::cout << "FECS Component Get (" + std::to_string(sample) + "), ";
		// 0.007ms avg
	}
	
	// fecs query
	{
		Benchmark timer;
		fecs.query<int>([](Entity id, int& i) {

		});

		std::cout << "FECS Query (1 Component, 1000 Entities)";
		// 0.0076ms avg
	}
}
