// Copyright © Ian Good

#include "pch.h"
#include <vector>
#include "../Giters/Giters.h"

using namespace Giters;

class NonNullTest : public ::testing::Test {
protected:
	std::vector<int*> pointers = { nullptr, &numberA, nullptr, nullptr, &numberB, &numberC };
	int numberA = 123;
	int numberB = 456;
	int numberC = 789;
};

TEST_F(NonNullTest, Empty) {
	int matches = 0;

	std::vector<int*> empty;
	auto nonNulls = empty | NonNull();
	for (int* p : nonNulls) {
		++matches;
	}

	int expected = 0;
	EXPECT_EQ(matches, expected);
}

TEST_F(NonNullTest, Some) {
	std::vector<int*> actual;
	auto nonNulls = pointers | NonNull();
	for (int* p : nonNulls) {
		actual.push_back(p);
	}

	std::vector<int*> expected = { &numberA, &numberB, &numberC };
	EXPECT_EQ(actual, expected);
}

TEST_F(NonNullTest, Chained) {
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
		| NonNull()
		| ToVector([](const std::string* name) { return *name; });

	std::vector<std::string> expected = { "blah" };
	EXPECT_EQ(actual, expected);
}
