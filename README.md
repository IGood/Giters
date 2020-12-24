# Giters - _LINQ style iterators for C++_

The goal is to write C++ code similar to LINQ in C#.<br>
Which is to say, transforming and filtering elements in a sequence using a fluent syntax.

C# accomplishes this via [extension methods](https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/classes-and-structs/extension-methods).<br>
C++ doesn't have this language feature but we can achieve similar results by overloading a binary operator.<br>
Giters uses the pipe (`|`) operator because we are conceptually "piping" elements through different operations.

```csharp
// C# Example
var squaredEvens = myNumbers
    .Where(n => n % 2 == 0)
    .Select(n => n * n)
    .ToList();
```

```cpp
// C++ Example
auto squaredEvens = myNumbers
    | Where([](int n) { return n % 2 == 0; })
    | Select([](int n) { return n * n; })
    | ToVector();

// Alternatively, we could use function pointers instead of lambdas:
auto squaredEvens = myNumbers | Where(&IsEven) | Select(&Square) | ToVector();
```

----

## Implementation Concepts & Range-Based `for` Loops

From [cppreference.com](https://en.cppreference.com/w/cpp/language/range-for), a **range-based for loop** looks like this...

```cpp
// Syntax
for (range_declaration : range_expression) loop_statement

// Example
for (const Widget& w : GetWidgets()) {
    w.PrintName();
}
```

...and expands to something like this...

```cpp
// Example
{
    auto&& __range = GetWidgets();
    auto __begin = std::begin(__range);
    auto __end = std::end(__range);
    for (; __begin != __end; ++__begin)
    {
        const Widget& w = *__begin;
        w.PrintName();
    }
}
```

This informative because it provides us with all the details necessary to implement a solution that allows code like `B` instead of code like `A`...

```cpp
std::vector<Widget*> widgets = GetWidgets();

// A (typical code)
for (Widget* w : widgets) {
    if (w != nullptr) {
        w->Refresh();
    }
}

// B (using Giters)
for (Widget* w : widgets | NonNull()) {
    w->Refresh();
}
```

We see that we need to implement the following pieces:
1. Some `operator|` that returns a "___range_" object
1. Some "___range_" object that can be passed to `std::begin()` and `std::end()`
1. Some "___begin_" object (an _iterator_) that implements...
   1. `operator!=` to compare against some "___end_" object
   1. `operator*` to get the current value of the iterator
   1. `operator++` to advance the iterator

That doesn't sound so hard 😀<br>
Here is pseudo-code that illustrates the basic outline for implementing a "non-null" filter (members & function implementations have been omitted for brevity):

```cpp
// a "token" type used with operator| (see below)
struct NonNull { };

// operator that accepts any "range" object on the left & our `NonNull` token on the right
template <typename TSource>
auto operator|(TSource& source, NonNull) {
    return NonNullRange_t<TSource>(source);
}

// the "__range" type that can be passed to `std::begin()` and `std::end()`
template <typename TSource>
struct NonNullRange_t {
    Iter_t begin() { return Iter_t(source); } // creates an iterator that skips over null values in the source range
    SourceEnd_t end() { return std::end(source); } // returns the "__end" object that we compare against to check termination
    TSource& source; // reference to the original/input/source range

    // the "iterator" type that implements operators to...
    //  - check termination
    //  - get the current value
    //  - advance to the next non-null value
    struct Iter_t {
        Iter_t(TSource& source); // initialize this iterator by advancing to the first non-null value in the range
        bool operator!=(const SourceEnd_t& end); // returns whether this iterator has reached the end of the range
        auto operator*(); // returns the current value of this iterator
        void operator++(); // advance to the next non-null value in the range
    };
};
```
