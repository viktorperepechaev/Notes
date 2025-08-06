[video](https://www.youtube.com/watch?v=JJqRlSTQlh4)
[additional information](https://google.github.io/googletest/)

## Definitions

**Unit test** - is a piece of code that tests a function or a class.

```txt
-------------  Apply inputs (mock data)   ----------------
| unite test| -->-->-->-->-->-->-->-->--> | function/class|
-------------                             ---------------- 
            |                                 |
            <--<--<--<--<--<--<--<--<--<--<--<-
                 Evaluate actual output
```

**Test Suite** - a group of logically related test cases. (Ex: different inputs for a function)
**Fixture** - a class factoring out commoc code for multiple cases. (Ex: Setup or TearDown)
**Mock Classes/Objects** - simplified and lightweight classes/objects to untangle complex dependencies while testing.

Test framework provides:
- Automation of execution (Ex: implementing main())
- Assertions, checks and matchers
- Test organization (Ex: suites, fixtures)
- Reports

Examples of test frameworks:
- Google Test and Mock
- Catch2
- Doctest
- Boost.test

## Basic Unit Test

Let's suppose we have a function:
```cpp
int Fib(int n) {
    if (n <= 1) {
        return n;
    }
    return Fib(n - 1) + Fib(n - 2);
}
```
Our test.cpp would look something like this:
```cpp
#include <gtest/gtest.h>

TEST(FibTest, NonNegativeValues) {  // Suite name: FibTest, Test name: NonNegativeValues
    EXPECT_EQ(Fib(0), 0);
    EXPECT_EQ(Fib(1), 1);
    EXPECT_EQ(Fib(5), 5);
    EXPECT_EQ(Fib(6), 8);
}

    TEST(FibTest, NegativeValues) {  // Suite name: FibTest, Test name: NegativeValues
    EXPECT_LT(Fib(-1), 0);
    EXPECT_LT(Fib(-10), 0);
}
```
Note: Suite name and Test name must be valid C++ identifiers **and** have no underscores inside.
Note: TEST(TestSuiteName, TestName) generates a class named TestSuiteName\_TestName\_Test
Note: Tests from different test suites can have the same individual name.

If we don't write a main function ourselves Google Test will link our programm to gtest\_main which has main.
if we write a main finction then Google Test will link our program to gtest.
Example of the main function:
```cpp
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
```

## Google Test Assertion Macros

### ASSERT vs EXPECT

|      Fatal assertion     |     Nonfatal assertion   |      Verifies      |
| ------------------------ | ------------------------ | ------------------ |
| ASSERT\_TRUE(condition)  | EXPECT\_TRUE(condition)  | condition is true  |
| ASSERT\_FALSE(condition) | EXPECT\_FALSE(condition) | condition is false |

The difference is that after ASSERT fails the test is not continued.
Ex:
```cpp
TEST(lol, aboba) {
    ASSERT_TRUE(false);
    std::cout << "This line won't be executed" << std::endl;
    ...
}
```
But:
```cpp
TEST(lol, aboba) {
    EXPECT_TRUE(false);
    std::cout << "This line will be executed" << std::endl;
    ...
}
```

### Binary Comparison  

| Fatal assertion  | Nonfatal assertion | Verifies |
| ---------------- | ------------------ | -------- |
| ASSERT\_EQ(x, y) | EXPECT\_EQ(x, y)   | x == y   |
| ASSERT\_NE(x, y) | EXPECT\_NE(x, y)   | x != y   |
| ASSERT\_LT(x, y) | EXPECT\_LT(x, y)   | x < y    |
| ASSERT\_LE(x, y) | EXPECT\_LE(x, y)   | x <= y   |
| ASSERT\_GT(x, y) | EXPECT\_GT(x, y)   | x > y    |
| ASSERT\_GE(x, y) | EXPECT\_GE(x, y)   | x >= y   |

Note: The value arguments must be comparable by the assertion’s comparison operator, otherwise a compiler error will result.

### Comparing C-strings

| Fatal assertion               | Nonfatal assertion              | Verifies                     |
| ----------------------------- | ------------------------------- | ---------------------------- |
| ASSERT\_STREQ(str1, str2)     | EXPECT\_STREQ(str1, str2)       | str1 == str2                 |
| ASSERT\_STRNE(str1, str2)     | EXPECT\_STRNE(str1, str2)       | str1 != str2                 |
| ASSERT\_STRCASEEQ(str1, str2) | EXPECT\_STRCASEEQ(str1, str2)   | str1 == str2 (ignoring case) |
| ASSERT\_STRCASENE(str1, str2) | EXPECT\_STRCASENE(str1, str2)   | str1 != str2 (ignoring case) |

Note: to compare std::string objects use EXPECT\_EQ and EXPECT\_NE instead.
Note: To compare a C-string with NULL use EXPECT\_EQ(str, nullptr).

### Comparing Floating Points

| Fatal assertion                    | Nonfatal assertion                   | Verifies                                          |
| ---------------------------------- | ------------------------------------ | ------------------------------------------------- |
| ASSERT\_FLOAT\_EQ(x, y)            | EXPECT\_FLOAT\_EQ(x, y)              | floats: x and y - are _almost_ equal              |
| ASSERT\_DOUBLE\_EQ(x, y)           | EXPECT\_DOUBLE\_EQ(x, y)             | doubles: x and y - are _almost_ equal             |
| ASSERT\_NEAR(x, y, eps)            | EXPECT\_NEAR(x, y, eps)              | the difference between x and y doesn't exceed eps |

Note: by _almost_ equal we mean that the two values are within 4 ULP's from each other

### Predicates

The following assertions enable more complex predicates to be verified while printing a more clear failure message than if EXPECT\_TRUE were used alone.

| Fatal assertion                 | Nonfatal assertion              | Verifies                      |
| ------------------------------- | ------------------------------- | ----------------------------- |
| ASSERT\_PRED1(pred, val1)       | EXPECT\_PRED1(pred, val1)       | pred(val1) returns true       |
| ASSERT\_PRED2(pred, val1, val2) | EXPECT\_PRED2(pred, val1, val2) | pred(val1, val2) return true  |
| ...                             | ...                             | ...                           |
(It goes up to PRED5)

Ex:
``` cpp
bool MyEqual(int a, int b) {
    return std::abs(a - b) < 10;
}

TEST(aboba, lol) {
    int a = 0;
    int b = 1;
    EXPECT_PRED2(MyEqual, a, b);
}
```

We can increase readability even further by modifying the predicat:
```cpp
AssertionResult MyEqual(int a, int b) {
    if (std::abs(a - b) < 10) {
        return AssertionSuccess();
    } else {
        return AssertionFailure()
            << "a: " << a << " and b: " << b << ". The difference is " << std::abs(a - b);
    }
    return std::abs(a - b) < 10;
}
```
Note: AssertionResult is a type defined by Google Test

### Testing Exceptions

| Fatal assertion                           | Nonfatal assertion                        | Verifies                                              |
| ----------------------------------------- | ----------------------------------------- | ----------------------------------------------------- |
| ASSERT\_THROW(statement, exception\_type) | EXPECT\_THROW(statement, exception\_type) | statement throws an exception of type exception\_type |
| ASSERT\_ANY\_THROW(statement)             | EXPECT\_ANY\_THROW(statement)             | statement throws an exception of any type             |
| ASSERT\_NO\_THROW(statement)              | EXPECT\_NO\_THROW(statement)              | statement does not throw any exception                |

Note: the piece of code under test can be a compound statement, ex:
```cpp
EXPECT_NO_THROW({
  int n = 5;
  DoSomething(&n);
});
```
We can also check the message inside of exception:
```cpp
TEST(lol, zonbie) {
    EXPECT_THROW({
        try {
            Divide(228, 0);
        } catch (const DivideByZero& e) {
            EXPECT_STREQ("Divide by zero :(", e.what());
            throw;
        }
    },
    DivideByZero);
}
```

### Death Tests

The following assertions verify that a piece of code causes the process to terminate.

- EXPECT\_DEATH:
    - EXPECT\_DEATH(statement, matcher)
    - ASSERT\_DEATH(statement, matcher)
    Note: EXPECT\_DEATH runs the target statement in a separate child process. The parent process then checks the child's exit status and captures its stderr to validate the outcome.
    Ex:
    ```cpp
    EXPECT_DEATH(DoSomething(42), "My error");
    ```
- EXPECT\_DEATH\_IF\_SUPPORTED:
    - EXPECT\_DEATH\_IF\_SUPPORTED(statement, matcher)
    - ASSERT\_DEATH\_IF\_SUPPORTED(statement, matcher)
    If death tests are supported, behaves the same as EXPECT\_DEATH. Otherwise, verifies nothing.
- EXPECT\_EXIT:
    - EXPECT\_EXIT(statement, predicate, matcher)
    - ASSERT\_EXIT(statement, predicate, matcher)
    Verifies that statement causes the process to terminate with an exit status that satisfies predicate, and produces stderr output that matches matcher.
Ex:
```cpp
void Exit(int exit_code = 0) {
    std::exit(exit_code);
}

void Kill(int exit_code = SIGINT) {
    kill(getpid(), exit_code);
}

TEST(MyDeathTest, Exit) {
    EXPECT_DEATH(Exit(-1), "");  // Both conditions are met => the test passes
}

TEST(MyDeathTest, NormalExit) {
    EXPECT_EXIT(Exit(0), testing::ExitedWithCode(0), "");  // the test passes
}

TEST(MyDeathTest, AbnormalExit) {
    EXPECT_EXIT(Exit(-1), testing::ExitedWithCode(-1), "");  // the test passes
}

TEST(MyDeathTest, KillProccess) {
    EXPECT_EXIT(Kill(SIGKILL), testing::KilledBySignal(SIGKILL), "");  // the test passes
}
```
Note: a c-string is treated as ContainsRegex(s), not Eq(s).
Note: name your test suit \*DeathTest since all test suites with a name ending in "DeathTest" are run before all other tests.

## Test Fixtures

```txt
                     o-----------------------o
o---------o          |    o -> test1 -> o    |          o------------o
| Setup() | -> -> -> | -> | -> test2 -> | -> | -> -> -> | TearDown() |
o---------o          |    o -> test3 -> o    |          o------------o
                     o-----------------------o
```
This way we can put the common code from test1, test2, test3 into Setup() or TearDown().

### Basic usage

```cpp
class MyTestFixture : public ::testing::Test {
  protected:
    void SetUp() override {
        ...
    }

    void TearDown() override {
        ...
    }
};

TEST_F(MyTestFixture, Test1) {
    ...
}
```
As an example, let’s write tests for a FIFO queue class named Queue, which has the following interface:
```cpp
template <typename E>  // E is the element type.
class Queue {
 public:
  Queue();
  void Enqueue(const E& element);
  E* Dequeue();  // Returns NULL if the queue is empty.
  size_t size() const;
  bool IsEmpty() const;
  ...
  private:
    std::vector<E> v_;
};
```
First, define a fixture class. By convention, you should give it the name FooTest where Foo is the class being tested.
```cpp
class QueueTest : public ::testing::Test {
  protected:
    
    void SetUp() override {
        q_.Enqueue(1);
        q_.Enqueue(2);
    }

    void TearDown() override {
        PrintQueue();
        std::cout << "======Test ended=====\n";
    }

    PrintQueue();

    Queue<int> q_;
}
```
And the test will look like this:
```cpp
TEST_F(QueueTest, IsNotEmptyInitially) {
    EXPECT_EQ(q_.size(), 2);
    EXPECT_FALSE(q_.IsEmpty());
}
```

Why q_ is protected and is accessible from TEST\_F? Well, because to run this test Google Test generates a class that looks _approximately_ like this:
```cpp
// This is a simplified view of what gtest generates for you
class QueueTest_IsNotEmptyInitially_Test : public QueueTest {
public:
    // The code you wrote inside TEST_F goes into this method.
    virtual void TestBody();
};

void QueueTest_IsNotEmptyInitially_Test::TestBody() {
    // Since this method is part of a class that inherits from QueueTest,
    // it can access its protected members directly.
    EXPECT_EQ(q_.size(), 2);
    EXPECT_FALSE(q_.IsEmpty());
}
```
Note: GoogleTest uses a different instance of QueueTest for each test. 

You can limit the time some test gets to execute using Test Fixture:
```cpp
class QueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        start_time_ms_ = std::chrono::high_resolution_clock::now();
        q_.Enqueue(1);
        q_.Enqueue(2);
    }

    void TearDown() override {
        auto end_time_ms = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time_ms - start_time_ms_);

        std::cout << "Test took: " << duration.count() << " ms" << std::endl;
        EXPECT_TRUE(duration.count() <= 1000) << "-> The test took too long!";
        PrintQueue();
        std::cout << "=======================Test ended!======================="
                  << std::endl;
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_ms_;
};

TEST_F(QueueTest, TakesTooLong) {
    for (unsigned long i = 0; i < 10000000; i++) {
        q_.Enqueue(1);
        q_.Dequeue();
    }
    EXPECT_EQ(q_.size(), 2);
}
```
You can also create setups and teardowns that execute not for each single test but for the whoule test suite:
```cpp
class FooTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        std::cout << "========Beginning of all tests of the test suite========" << std::endl;
        if (shared_resource_ == nullptr) {
            shared_resource_ =
                new std::string("This is an expensive shared resource!");
        }
    }

    static void TearDownTestSuite() {
        std::cout << "========End of all tests of the test suite========" << std::endl;
        delete shared_resource_;
        shared_resource_ = nullptr;
    }

    void SetUp() override {
        std::cout << "Called before each test." << std::endl;
    }

    void TearDown() override {
        std::cout << "Called after each test." << std::endl;
    }

    // Some expensive resource shared by all tests.
    static std::string* shared_resource_;
};

```

## Manipulating Tests

You can disable a specific test:
```cpp
TEST(lol, DISABLED_aboba) {  // you need to add "DISABLED_" to the test's name
    EXPECT_TRUE(false);
}
```
You can disable a whole test suite:
```cpp
class DISABLED_BasicTests : public ::testing::Test {};

TEST_F(DISABLED_BasicTests, aboba) {
    EXPECT_TRUE(false);
}
```

You can skip a specific test:
```cpp
TEST(lol, aboba) {
    GTEST_SKIP() << "Skipping this test";
    EXPECT_TRUE(false);
}
```
You can skip a whole test suite:
```cpp
class SkipFixture : public ::testing::Test {
  protected:
    void SetUp() override {
        GTEST_SKIP() << "Skipping of all this test suite tests!";
    }
}

TEST_F(SkipFixture, lol1) {
    ...
}

TEST_F(SkipFixture, lol2) {
    ...
}
```

Note: Skipping is a runtime decision and Disabling is a compile time decision

## Filtering Tests: `--gtest_filter`

If you set the `--gtest_filter` flag to a filter string, GoogleTest will only run the tests whose full names (in the form of TestSuiteName.TestName) match the filter.
The format of a filter is a ‘:‘-separated list of wildcard patterns (called the positive patterns) optionally followed by a ‘-’ and another ‘:‘-separated pattern list (called the negative patterns). A test matches the filter if and only if it matches any of the positive patterns but does not match any of the negative patterns.
A pattern may contain '*' (matches any string) or '?' (matches any single character). For convenience, the filter '*-NegativePatterns' can be also written as '-NegativePatterns'.
Ex:
- `./foo_test` Has no flag, and thus runs all its tests
- `./foo_test --gtest_filter=*` Also runs everything, due to the single match-everything * value
- `./foo_test --gtest_filter=FooTest.*` Runs everything in test suite FooTest
- `./foo_test --gtest_filter=*Null*:*Constructor*` Runs any test whose full name contains either "Null" or "Constructor"
- `./foo_test --gtest_filter=-*DeathTest.*` Runs all non-death tests
- `./foo_test --gtest_filter=FooTest.*-FooTest.Bar` Runs everything in test suite FooTest except FooTest.Bar
- `./foo_test --gtest_filter=FooTest.*:BarTest.*-FooTest.Bar:BarTest.Foo` Runs everything in test suite FooTest except FooTest.Bar and everything in test suite BarTest except BarTest.

## Printing user types

Some of the asserts provide descriptions of the containers' insides if the the fails (Ex: EXPECT\_THAT(value, mathcer)). But for this GoogleTest has to know how to print your container/type.
You can "teach" GoogleTest by providing a `PrintTo` function:
```cpp
class Point {
  ...
  friend void PrintTo(const Point& point, std::ostream* os) {  // We can simply define the whole function
                                                               // inside the class because ADL won't find
                                                               // PrintTo in this case
    *os << "(" << point.x << "," << point.y << ")";
  }

  int x;
  int y;
};

// If you can't declare the function in the class it's important that PrintTo()
// is defined in the SAME namespace that defines Point.  C++'s look-up rules
// rely on that.

void PrintTo(const Point& point, std::ostream* os) {
    *os << "(" << point.x << "," << point.y << ")";
}
```

