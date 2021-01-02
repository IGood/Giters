// Copyright © Ian Good

#include "pch.h"
#include <vector>
#include "../Giters/Giters.h"

using namespace Giters;

class NonNullRefTest : public ::testing::Test {
protected:
	std::vector<int*> pointers = { nullptr, &numberA, nullptr, nullptr, &numberB, &numberC };
	int numberA = 123;
	int numberB = 456;
	int numberC = 789;
};

TEST_F(NonNullRefTest, Empty) {
	int matches = 0;

	std::vector<int*> empty;
	auto references = empty | NonNullRef();
	for (int& r : references) {
		++matches;
	}

	int expected = 0;
	EXPECT_EQ(matches, expected);
}

TEST_F(NonNullRefTest, Some) {
	std::vector<int> actual;
	auto references = pointers | NonNullRef();
	for (int& r : references) {
		actual.push_back(r *= 2);
	}

	EXPECT_EQ(numberA, 123 * 2);
	EXPECT_EQ(numberB, 456 * 2);
	EXPECT_EQ(numberC, 789 * 2);

	std::vector<int> expected = { numberA, numberB, numberC };
	EXPECT_EQ(actual, expected);
}

TEST_F(NonNullRefTest, WhereSelect) {
	std::vector<int> numbers;
	numbers.reserve(2048);

	pointers.clear();
	pointers.reserve(2048);

	int expectedValid = 0;
	for (int i = -1000; i < 1000; ++i) {
		numbers.push_back(i);

		if (i % 3 == 0) {
			pointers.push_back(nullptr);
		}
		else {
			++expectedValid;
			pointers.push_back(&numbers.back());
		}
	}

	std::vector<int> results;

	auto verifyResults = [&]() {
		EXPECT_EQ(results.size(), expectedValid);

		int p = 0;
		for (int i = 0; i < results.size(); ++i) {
			int actual = results[i];

			ASSERT_LT(p, pointers.size());
			while (pointers[p] == nullptr) {
				++p;
			}

			ASSERT_LT(p, pointers.size());
			int expected = *pointers[p++];
			EXPECT_EQ(actual, expected);
		}
	};

	auto qA = pointers
		| Where([](const int* p) { return p != nullptr; })
		| Select([](int* p) -> int& { return *p; });
	results = qA | ToVector(pointers.size(), [](int& r) { return r *= 2; });
	verifyResults();

	auto qB = pointers | NonNullRef();
	results = qB | ToVector(pointers.size(), [](int& r) { return r *= 2; });
	verifyResults();
}

TEST_F(NonNullRefTest, Chained) {
	struct Widget {
		std::string name;
		bool isEnabled;
	};

	std::vector<Widget> widgets = {
		{"foo", false},
		{"bar", false},
		{"blah", true},
	};

	auto actual = widgets
		| Select([](const Widget& w) { return w.isEnabled ? &w.name : nullptr; })
		| NonNullRef()
		| ToVector([](const std::string& name) { return name; });

	std::vector<std::string> expected = { "blah" };
	EXPECT_EQ(actual, expected);
}
