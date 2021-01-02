// Copyright © Ian Good

#include <celero/Celero.h>
#include "../Giters/Giters.h"

using namespace Giters;

class NonNullRefFixture : public celero::TestFixture
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
		celero::DoNotOptimizeAway(collection);

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

constexpr uint64_t kSamples = 30;
constexpr uint64_t kIterations = 10'000;

BASELINE_F(CollectNonNullRef, LoopInline, NonNullRefFixture, kSamples, kIterations) {
	std::vector<int> results;
	results.reserve(pointers.size());
	for (int* p : pointers) {
		if (p != nullptr) {
			results.push_back((*p) *= 2);
		}
	}
	iterate(results);
}

BENCHMARK_F(CollectNonNullRef, LoopFunc, NonNullRefFixture, kSamples, kIterations) {
	std::vector<int> results;
	results.reserve(pointers.size());
	for (int* p : pointers) {
		if (IsNotNull(p)) {
			int& r = Dereference(p);
			results.push_back(r *= 2);
		}
	}
	iterate(results);
}

BENCHMARK_F(CollectNonNullRef, NonNullSelectLambda, NonNullRefFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| NonNull()
		| Select([](int* p) -> int& { return *p; })
		| ToVector(pointers.size(), [](int& r) { return r *= 2; });
	iterate(results);
}

BENCHMARK_F(CollectNonNullRef, NonNullSelectFunc, NonNullRefFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| NonNull()
		| Select(&Dereference)
		| ToVector(pointers.size(), [](int& r) { return r *= 2; });
	iterate(results);
}

BENCHMARK_F(CollectNonNullRef, NonNullSelectFuncLambda, NonNullRefFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| NonNull()
		| Select([](int* p) -> int& { return Dereference(p); })
		| ToVector(pointers.size(), [](int& r) { return r *= 2; });
	iterate(results);
}

BENCHMARK_F(CollectNonNullRef, WhereSelectLambdas, NonNullRefFixture, kSamples, kIterations) {
	//*
	std::vector<int> results = pointers
		| Where([](const int* p) { return p != nullptr; })
		| Select([](int* p) -> int& { return *p; })
		| ToVector(pointers.size(), [](int& r) { return r *= 2; });
	/*/
	// TODO: Figure out why this is broken in Release builds.
	//	Somethings are getting optimized away?
	auto q = pointers
		| Where([](const int* p) { return p != nullptr; })
		| Select([](int* p) -> int& { return *p; });
	std::vector<int> results = q | ToVector(pointers.size(), [](int& r) { return r *= 2; });
	//*/
	iterate(results);
}

BENCHMARK_F(CollectNonNullRef, WhereSelectFuncs, NonNullRefFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| Where(&IsNotNull)
		| Select(&Dereference)
		| ToVector(pointers.size(), [](int& r) { return r *= 2; });
	iterate(results);
}

BENCHMARK_F(CollectNonNullRef, WhereSelectFuncLambdas, NonNullRefFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| Where([](const int* p) { return IsNotNull(p); })
		| Select([](int* p) -> int& { return Dereference(p); })
		| ToVector(pointers.size(), [](int& r) -> int { return r *= 2; });
	iterate(results);
}

BENCHMARK_F(CollectNonNullRef, NonNullRefLambda, NonNullRefFixture, kSamples, kIterations) {
	std::vector<int> results = pointers
		| NonNullRef()
		| ToVector(pointers.size(), [](int& r) -> int { return r *= 2; });
	iterate(results);
}
