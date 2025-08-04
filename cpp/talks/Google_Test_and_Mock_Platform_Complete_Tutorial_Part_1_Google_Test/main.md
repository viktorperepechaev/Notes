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
