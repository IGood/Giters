// Copyright © Ian Good

#include <celero/Celero.h>
#include "../Giters/Giters.h"

using namespace Giters;

class NonNullFixture : public celero::TestFixture
{
public:
	void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override {
		numbers.clear();
		numbers.reserve(2048);
		pointers.clear();
		pointers.reserve(2048);
		for (int i = -1000; i < 1000; ++i) {
			numbers.push_back(i);
			pointers.push_back((i % 3 == 0) ? nullptr : &numbers.back());
		}
	}

	void iterate(const std::vector<int>& collection) {
		for (int n : collection) {
			celero::DoNotOptimizeAway(n);
		}
	}

	std::vector<int> numbers;
	std::vector<int*> pointers;
};

static bool IsNotNull(const int* p) { return p != nullptr; }
static int Dereference(const int* p) { return *p; }

constexpr uint64_t kSamples = 30;
constexpr uint64_t kIterations = 10'000;

BASELINE_F(CollectNonNull, LoopInline, NonNullFixture, kSamples, kIterations) {
	std::vector<int> results;
	results.reserve(pointers.size());
	for (int* p : pointers) {
		if (p != nullptr) {
			results.push_back(*p);
		}
	}
	iterate(results);
}

BENCHMARK_F(CollectNonNull, LoopFunc, NonNullFixture, kSamples, kIterations) {
	std::vector<int> results;
	results.reserve(pointers.size());
	for (int* p : pointers) {
		if (IsNotNull(p)) {
			results.push_back(Dereference(p));
		}
	}
	iterate(results);
}

BENCHMARK_F(CollectNonNull, NonNullLambda, NonNullFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| NonNull()
		| ToVector(pointers.size(), [](const int* p) { return *p; });
	iterate(results);
}

BENCHMARK_F(CollectNonNull, NonNullFunc, NonNullFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| NonNull()
		| ToVector(pointers.size(), &Dereference);
	iterate(results);
}

BENCHMARK_F(CollectNonNull, NonNullFuncLambda, NonNullFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| NonNull()
		| ToVector(pointers.size(), [](const int* p) {return Dereference(p); });
	iterate(results);
}

BENCHMARK_F(CollectNonNull, WhereLambda, NonNullFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| Where([](const int* p) {return p != nullptr; })
		| ToVector(pointers.size(), [](const int* p) { return *p; });
	iterate(results);
}

BENCHMARK_F(CollectNonNull, WhereFuncs, NonNullFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| Where(&IsNotNull)
		| ToVector(pointers.size(), &Dereference);
	iterate(results);
}

BENCHMARK_F(CollectNonNull, WhereFuncLambdas, NonNullFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| Where([](const int* p) {return IsNotNull(p); })
		| ToVector(pointers.size(), [](const int* p) {return Dereference(p); });
	iterate(results);
}
