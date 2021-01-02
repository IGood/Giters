// Copyright © Ian Good

#include <benchmark/benchmark.h>
#include "../Giters/Giters.h"

using namespace Giters;

class NonNullFixture : public benchmark::Fixture {
protected:
	void SetUp(const benchmark::State& state) {
		numbers.clear();
		numbers.reserve(2048);
		pointers.clear();
		pointers.reserve(2048);
		for (int i = -1000; i < 1000; ++i) {
			numbers.push_back(i);
			pointers.push_back((i % 3 == 0) ? nullptr : &numbers.back());
		}
	}

	void TearDown(const benchmark::State& state) {
	}

	void Iterate(const std::vector<int>& collection) {
		for (int n : collection) {
			benchmark::DoNotOptimize(n);
		}
	}

	std::vector<int> numbers;
	std::vector<int*> pointers;
};

static bool IsNotNull(const int* p) { return p != nullptr; }
static int Dereference(const int* p) { return *p; }

BENCHMARK_F(NonNullFixture, TypicalLoopInline)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results;
		results.reserve(pointers.size());
		for (int* p : pointers) {
			if (p != nullptr) {
				results.push_back(*p);
			}
		}
		Iterate(results);
	}
}

BENCHMARK_F(NonNullFixture, TypicalLoopFunctions)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results;
		results.reserve(pointers.size());
		for (int* p : pointers) {
			if (IsNotNull(p)) {
				results.push_back(Dereference(p));
			}
		}
		Iterate(results);
	}
}

BENCHMARK_F(NonNullFixture, GitersNonNullLambda)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = pointers
			| NonNull()
			| ToVector(pointers.size(), [](const int* p) { return *p; });
		Iterate(results);
	}
}

BENCHMARK_F(NonNullFixture, GitersNonNullFunction)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = pointers
			| NonNull()
			| ToVector(pointers.size(), &Dereference);
		Iterate(results);
	}
}

BENCHMARK_F(NonNullFixture, GitersNonNullFunctionLambda)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = pointers
			| NonNull()
			| ToVector(pointers.size(), [](const int* p) {return Dereference(p); });
		Iterate(results);
	}
}

BENCHMARK_F(NonNullFixture, GitersWhereLambda)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = pointers
			| Where([](const int* p) {return p != nullptr; })
			| ToVector(pointers.size(), [](const int* p) { return *p; });
		Iterate(results);
	}
}
