// Copyright © Ian Good

#include "pch.h"
#include <functional>
#include <vector>
#include <xstring>
#include "../Giters/Giters/FirstOrDefault.h"

using namespace Giters;

struct Widget
{
	std::string Name;
	bool isValid = false;
	bool IsValid() const { return isValid; }
};

class FirstOrDefaultTest : public ::testing::Test {
protected:
	std::vector<int> numbers;
	std::vector<int*> pointers;
	std::vector<Widget> widgets;

	int numberA = 123;
	int numberB = 456;

	void SetUp() override {
		numbers = { 8, 6, 7, 5, 3, 0, 9 };
		pointers = { &numberA, &numberB, nullptr };
		widgets = {
			Widget{"foo", false},
			Widget{"bar", false},
			Widget{"blah", true},
		};
	}
};

// Empty sequence returns default value of the type.
TEST_F(FirstOrDefaultTest, NoPredicateEmpty) {
	{
		std::vector<int> empty;
		int actual = empty | FirstOrDefault();
		int expected = 0;
		EXPECT_EQ(actual, expected);
	}
	{
		std::vector<int*> empty;
		int* actual = empty | FirstOrDefault();
		int* expected = nullptr;
		EXPECT_EQ(actual, expected);
	}
	{
		std::vector<Widget> empty;
		Widget actual = empty | FirstOrDefault();
		Widget expected;
		EXPECT_EQ(actual.Name, expected.Name);
		EXPECT_EQ(actual.isValid, expected.isValid);
	}
}

// Non-empty sequence returns first element.
TEST_F(FirstOrDefaultTest, NoPredicateSome) {
	{
		int actual = numbers | FirstOrDefault();
		int expected = 8;
		EXPECT_EQ(actual, expected);
	}
	{
		int* actual = pointers | FirstOrDefault();
		int* expected = &numberA;
		EXPECT_EQ(actual, expected);
	}
	{
		Widget actual = widgets | FirstOrDefault();
		Widget expected = { "foo", false };
		EXPECT_EQ(actual.Name, expected.Name);
		EXPECT_EQ(actual.isValid, expected.isValid);
	}
}

// Empty sequence returns default value of the type.
TEST_F(FirstOrDefaultTest, WithPredicateEmpty) {
	std::vector<int> empty;
	int actual = empty | FirstOrDefault([](int n) { return true; });
	int expected = 0;
	EXPECT_EQ(actual, expected);
}

// Non-empty sequence returns first element that matches the predicate.
TEST_F(FirstOrDefaultTest, WithPredicateSome) {
	// Predicate can be a lambda.
	{
		// first less than 5
		int actual = numbers | FirstOrDefault([](int n) { return n < 5; });
		int expected = 3;
		EXPECT_EQ(actual, expected);

		// first or default greater than 10
		actual = numbers | FirstOrDefault([](int n) { return n > 10; });
		expected = 0;
		EXPECT_EQ(actual, expected);
	}
	// Predicate can be a functor.
	{
		struct IsOdd
		{
			bool operator()(int n) const { return n % 2 != 0; }
		} myFunctor;

		// first odd
		int actual = numbers | FirstOrDefault(myFunctor);
		int expected = 7;
		EXPECT_EQ(actual, expected);
	}
	// Predicate can be a binder.
	{
		struct IsOdd
		{
			bool Test(int n) const { return n % 2 != 0; }
		} myFunctor;
		auto isOdd = std::bind(&IsOdd::Test, &myFunctor, std::placeholders::_1);

		// first odd
		int actual = numbers | FirstOrDefault(isOdd);
		int expected = 7;
		EXPECT_EQ(actual, expected);
	}
	// Predicate can be a static function.
	{
		struct IsOdd
		{
			static bool Test(int n) { return n % 2 != 0; }
		};

		// first odd
		int actual = numbers | FirstOrDefault(&IsOdd::Test);
		int expected = 7;
		EXPECT_EQ(actual, expected);
	}
	// Predicate can be a member function.
	{
		// first valid widget
		Widget actualWidget = widgets | FirstOrDefault(&Widget::IsValid);
		std::string expectedName = "blah";
		EXPECT_EQ(actualWidget.Name, expectedName);
	}
}

TEST_F(FirstOrDefaultTest, WithStatefulPredicate) {
	// Functor
	{
		struct IsOdd
		{
			bool operator()(int n) {
				++invokeCount;
				return n % 2 != 0;
			}

			int invokeCount = 0;
		};

		IsOdd myFunctor;

		int actual = numbers | FirstOrDefault(myFunctor);
		int expected = 7;
		EXPECT_EQ(actual, expected);

		EXPECT_EQ(myFunctor.invokeCount, 3);

		// repeat with same functor
		actual = numbers | FirstOrDefault(myFunctor);
		expected = 7;
		EXPECT_EQ(actual, expected);

		EXPECT_EQ(myFunctor.invokeCount, 6);
	}
	// Lambda
	{
		int invokeCount = 0;
		auto isOdd = [&invokeCount](int n) {
			++invokeCount;
			return n % 2 != 0;
		};

		int actual = numbers | FirstOrDefault(isOdd);
		int expected = 7;
		EXPECT_EQ(actual, expected);

		EXPECT_EQ(invokeCount, 3);

		// repeat with same lambda
		actual = numbers | FirstOrDefault(isOdd);
		expected = 7;
		EXPECT_EQ(actual, expected);

		EXPECT_EQ(invokeCount, 6);
	}
}
