// Copyright © Ian Good

#include <benchmark/benchmark.h>
#include "../Giters/Giters.h"

using namespace Giters;

class NonNullRefFixture : public benchmark::Fixture {
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
		benchmark::DoNotOptimize(collection);

		if (collection.size() != 1333) {
			throw "ohshit";
		}

		int p = 0;
		for (int n : collection) {
			while (pointers[p] == nullptr) {
				++p;
			}

			if (n != *pointers[p]) {
				throw "ohshit";
			}

			++p;
		}
	}

	std::vector<int> numbers;
	std::vector<int*> pointers;
};

static bool IsNotNull(const int* p) { return p != nullptr; }
static int& Dereference(int* p) { return *p; }

BENCHMARK_F(NonNullRefFixture, TypicalLoopInline)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results;
		results.reserve(pointers.size());
		for (int* p : pointers) {
			if (p != nullptr) {
				results.push_back((*p) *= 2);
			}
		}
		Iterate(results);
	}
}

BENCHMARK_F(NonNullRefFixture, TypicalLoopFunctions)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results;
		results.reserve(pointers.size());
		for (int* p : pointers) {
			if (IsNotNull(p)) {
				int& r = Dereference(p);
				results.push_back(r *= 2);
			}
		}
		Iterate(results);
	}
}

BENCHMARK_F(NonNullRefFixture, NonNullSelectLambda)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = pointers
			| NonNull()
			| Select([](int* p) -> int& { return *p; })
			| ToVector(pointers.size(), [](int& r) { return r *= 2; });
		Iterate(results);
	}
}

BENCHMARK_F(NonNullRefFixture, NonNullSelectFunc)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = pointers
			| NonNull()
			| Select(&Dereference)
			| ToVector(pointers.size(), [](int& r) { return r *= 2; });
		Iterate(results);
	}
}

BENCHMARK_F(NonNullRefFixture, NonNullSelectFuncLambda)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = pointers
			| NonNull()
			| Select([](int* p) -> int& {return Dereference(p); })
			| ToVector(pointers.size(), [](int& r) { return r *= 2; });
		Iterate(results);
	}
}

BENCHMARK_F(NonNullRefFixture, WhereSelectLambda)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = pointers
			| Where([](const int* p) {return p != nullptr; })
			| Select([](int* p) -> int& { return *p; })
			| ToVector(pointers.size(), [](int& r) { return r *= 2; });
		Iterate(results);
	}
}

BENCHMARK_F(NonNullRefFixture, NonNullRef)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = pointers
			| NonNullRef()
			| ToVector(pointers.size(), [](int& r) { return r *= 2; });
		Iterate(results);
	}
}
