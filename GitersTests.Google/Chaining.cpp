// Copyright © Ian Good

#include "pch.h"
#include <functional>
#include <vector>
#include "../Giters/Giters.h"

using namespace Giters;

struct MyFunctor
{
	static int nextId;
	static int destroyedCount;
	static int copiedCount;
	static int movedCount;
	int id = ++nextId;

	MyFunctor() = default;

	MyFunctor(const MyFunctor& other) {
		++copiedCount;
	}

	MyFunctor(MyFunctor&& other) noexcept {
		++movedCount;
	}

	~MyFunctor() {
		++destroyedCount;
	}

	MyFunctor& operator=(const MyFunctor& other) {
		++copiedCount;
	}

	MyFunctor& operator=(MyFunctor&& other) noexcept {
		++movedCount;
	}

	bool IsEven(int n) {
		++numIsEvenInvokes;
		return n % 2 == 0;
	}

	int Square(int n) {
		++numSquareInvokes;
		return n * n;
	}

	bool GT30(int n) {
		++numGT30Invokes;
		return n > 30;
	}

	int numIsEvenInvokes = 0;
	int numSquareInvokes = 0;
	int numGT30Invokes = 0;
};

int MyFunctor::nextId = 0;
int MyFunctor::destroyedCount = 0;
int MyFunctor::copiedCount = 0;
int MyFunctor::movedCount = 0;

TEST(Chaining, PredicatesThings) {
	int numbers[] = { -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8 };

	MyFunctor mf;

	int actual = numbers
		| Where([&](int n) { return mf.IsEven(n); })
		| Select([&](int n) { return mf.Square(n); })
		| FirstOrDefault([&](int n) { return mf.GT30(n); });
	int expected = 36;
	EXPECT_EQ(actual, expected);

	EXPECT_EQ(MyFunctor::destroyedCount, 0);
	EXPECT_EQ(MyFunctor::copiedCount, 0);
	EXPECT_EQ(MyFunctor::movedCount, 0);

	EXPECT_EQ(mf.id, expected = 1);
	EXPECT_EQ(mf.numIsEvenInvokes, expected = 10);
	EXPECT_EQ(mf.numSquareInvokes, expected = 5);
	EXPECT_EQ(mf.numGT30Invokes, expected = 5);
}

TEST(Chaining, ReuseRangeObject) {
	int numbers[] = { -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8 };

	int numInspected = 0;
	int numKept = 0;
	auto range = numbers
		| Where([&](int n) { ++numInspected; return n > 0; })
		| Select([&](int n) { ++numKept; return n * 2; });

	EXPECT_EQ(numInspected, 0);
	EXPECT_EQ(numKept, 0);

	for (int n : range) {}
	EXPECT_EQ(numInspected, 12);
	EXPECT_EQ(numKept, 8);

	range | Consume();
	EXPECT_EQ(numInspected, 24);
	EXPECT_EQ(numKept, 16);

	range | ForEach([](int n) {/* ... */});
	EXPECT_EQ(numInspected, 36);
	EXPECT_EQ(numKept, 24);

	auto keepers = range | ToVector();

	EXPECT_EQ(numInspected, 48);
	EXPECT_EQ(numKept, 32);

	int first = range | FirstOrDefault();

	EXPECT_EQ(numInspected, 53);
	EXPECT_EQ(numKept, 33);
	EXPECT_EQ(first, 2);
}
