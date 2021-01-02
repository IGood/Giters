// Copyright © Ian Good

#include "pch.h"
#include <vector>
#include "../Giters/Giters/Where.h"

using Giters::Where;

class WhereTest : public ::testing::Test {
protected:
	std::vector<int> numbers = { -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8 };
};

TEST_F(WhereTest, Empty) {
	int matches = 0;

	std::vector<int> empty;
	auto everything = empty | Where([](int n) { return true; });
	for (int n : everything) {
		++matches;
	}

	int expected = 0;
	EXPECT_EQ(matches, expected);
}

TEST_F(WhereTest, NoMatches) {
	int matches = 0;

	auto gt10 = numbers | Where([](int n) { return n > 10; });
	for (int n : gt10) {
		++matches;
	}

	int expected = 0;
	EXPECT_EQ(matches, expected);
}

TEST_F(WhereTest, SomeMatches) {
	std::vector<int> actualEvens;
	auto evens = numbers | Where([](int n) { return n % 2 == 0; });
	for (int n : evens) {
		actualEvens.push_back(n);
	}

	std::vector<int> expectedEvens = { -2, 0, 2, 4, 6, 8 };
	EXPECT_EQ(actualEvens, expectedEvens);
}

TEST_F(WhereTest, LambdaShenanigans) {
	auto someWhereLambda = [](int n) {
		return n % 2 == 0;
	};

	auto someWhere = Where(std::move(someWhereLambda));

	std::vector<int> actualEvens;
	auto evens = numbers | std::move(someWhere);
	for (int n : evens) {
		actualEvens.push_back(n);
	}

	std::vector<int> expectedEvens = { -2, 0, 2, 4, 6, 8 };
	EXPECT_EQ(actualEvens, expectedEvens);
}
