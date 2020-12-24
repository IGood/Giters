// Copyright © Ian Good

#include <benchmark/benchmark.h>
#include "../Giters/Giters.h"

using namespace Giters;

class ChainingFixture : public benchmark::Fixture {
protected:
	void SetUp(const benchmark::State& state) {
		numbers.clear();
		for (int i = -1000; i < 1000; ++i) {
			numbers.push_back(i);
			numbers.push_back(i * 2);
		}
	}

	void TearDown(const benchmark::State& state) {
	}

	std::vector<int> numbers;
};

static bool IsEven(int n) { return n % 2 == 0; }
static int Square(int n) { return n * n; }
static bool GT100(int n) { return n > 100; }
static int Negate(int n) { return -n; }

BENCHMARK_F(ChainingFixture, TypicalLoopInline)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results;
		for (int n : numbers) {
			if (n % 2 == 0) {				// even
				int sq = n * n;				// square
				if (sq > 100) {				// > 100
					results.push_back(-n);	// negate
				}
			}
		}
	}
}

BENCHMARK_F(ChainingFixture, TypicalLoopFunctions)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results;
		for (int n : numbers) {
			if (IsEven(n)) {						// even
				int sq = Square(n);					// square
				if (GT100(sq)) {					// > 100
					results.push_back(Negate(n));	// negate
				}
			}
		}
	}
}

BENCHMARK_F(ChainingFixture, GitersLambdas)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = numbers
			| Where([](int n) { return n % 2 == 0; })	// even
			| Select([](int n) { return n * n; })		// square
			| Where([](int n) { return n > 100; })		// >100
			| ToVector([](int n) { return -n; });		// negate
	}
}

BENCHMARK_F(ChainingFixture, GitersFunctions1)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = numbers
			| Where(&IsEven)	// even
			| Select(&Square)	// square
			| Where(&GT100)		// >100
			| ToVector(&Negate);// negate
	}
}

BENCHMARK_F(ChainingFixture, GitersFunctions2)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = numbers
			| Where(IsEven)		// even
			| Select(Square)	// square
			| Where(GT100)		// >100
			| ToVector(Negate);	// negate
	}
}

BENCHMARK_F(ChainingFixture, GitersFunctionLambdas)(benchmark::State& st) {
	for (auto _ : st) {
		std::vector<int> results = numbers
			| Where([](int n) { return IsEven(n); })	// even
			| Select([](int n) { return Square(n); })	// square
			| Where([](int n) { return GT100(n); })		// >100
			| ToVector([](int n) { return Negate(n); });// negate
	}
}
