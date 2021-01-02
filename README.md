# Giters - _LINQ style iterators for C++_

The goal is to write C++ code similar to [LINQ](https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/concepts/linq/) in C# (using [method syntax](https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/concepts/linq/query-syntax-and-method-syntax-in-linq)).<br>
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

## Giters

This library implements several projections, filters, & other utilities.<br>
Projections generally take a **selector** as a parameter - a function that takes an element as the input value & returns some new value.<br>
Filters generally take a **predicate** as a parameter - a function that takes an element & returns a value indicating whether that element should be included in the result range.

- `Consume` - iterates the entire range
- `FirstOrDefault` - gets the first element in the range; returns a default-constructed object if the range is empty
- `ForEach` - executes a function for each element; consumes the range
- `NonNull` - keeps elements where `element != nullptr` & yields pointers
- `NonNullRef` - keeps elements where `element != nullptr` & yields references
- `Select` - projects each element into a new value
- `ToVector` - creates a new `std::vector<>` containing the elements in the range
- `Visit` - executes a function for each element
- `Where` - keeps elements where the predicate returns `true`

Creating an iterable range object using Giters does not perform any iterations itself.<br>
The evaluation of the range is said to be **lazy** & the range must be _consumed_ in some way to process the results.

For example, this code creates a range object, `foo`, that can be iterated, but no elements have been inspected or transformed yet:

```cpp
int myNumbers[] = { -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8 };
int numInspected = 0;
int numTransformed = 0;
auto foo = myNumbers
    | Where([&numInspected](int n) { ++numInspected; return n > 0; })
    | Select([&numTransformed](int n) { ++numTransformed; return n * 2; });
// `numInspected` and `numTransformed` are both zero because `foo` has not been consumed.
```

The object `foo` can be consumed in various ways & the Giter operations will be performed ***each time*** as necessary.

```cpp
//                                                  numInspected =  0, numTransformed =  0
for (int n : foo) { }                            // numInspected = 12, numTransformed =  8
foo | Consume();                                 // numInspected = 24, numTransformed = 16
foo | ForEach([](int n) { /* todo: print n */ });// numInspected = 36, numTransformed = 24
std::vector<int> keepers = foo | ToVector();     // numInspected = 48, numTransformed = 32, keepers = { 2, 4, 6, 8, 10, 12, 14, 16 }
int first = foo | FirstOrDefault();              // numInspected = 53, numTransformed = 33, first = 2
```

In the last line above, `numInspected` changes from 48 to 53 & `numTransformed` from 32 to 33 because `FirstOrDefault` stops iterating after it finds its first match (in this case, the first number greater than zero).

----

## Example

```cpp
// Giters
std::vector<std::string> names = widgets
    | NonNull()
    | Visit([](const Widget* w) { LogWidgetState(*w); })
    | Where([](const Widget* w) { return w->IsEnabled(); })
    | Select[](const Widget* w) { return w->GetName(); })
    | ToVector();

// Typical C++
std::vector<std::string> names;
for (const Widget* w : widgets) {
    if (w != nullptr) {
        LogWidgetState(*w);
        if (w->IsEnabled()) {
            names.push_back(w->GetName());
        }
    }
}
```

----

## Implementation Concepts & Range-Based `for` Loops

From [cppreference.com](https://en.cppreference.com/w/cpp/language/range-for), a **range-based for loop** looks like this...

```cpp
for (range_declaration : range_expression) loop_statement
```

So our code like this...

```cpp
for (const Widget& w : GetWidgets()) {
    w.PrintName();
}
```

...expands to something like this...

```cpp
{
    auto&& __range = GetWidgets();
    auto __begin = std::begin(__range);
    auto __end = std::end(__range);
    for (; __begin != __end; ++__begin) {
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
Here is pseudo-code that illustrates the basic outline for implementing a "non-null to ref" Giter that yields references to elements that are not null (members & function implementations have been omitted for brevity):

```cpp
// Goal: Transform a range of pointers into a range of references (iff a pointer is not null).
// Example: If a pointer is not null, then double the value.
for (int* p : myPointers) { if (p != nullptr) { (*p) *= 2; } } // hand-written style
for (int& r : myPointers | NonNullRef()) { r *= 2; } // Giters style

// Giters Implementation:
// a "token" type used with `operator|` (see below)
struct NonNullRef { };

// operator that accepts any "range" object on the left & our `NonNullRef` token on the right
template <typename TSource>
auto operator|(TSource& source, NonNullRef) {
    return NonNullRefRange_t<TSource>(source);
}

// the "__range" type that can be passed to `std::begin()` and `std::end()`
template <typename TSource>
struct NonNullRefRange_t {
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
        auto& operator*(); // returns the current value of this iterator - dereferences a pointer & returns a reference
        void operator++(); // advance to the next non-null value in the range
    };
};
```
