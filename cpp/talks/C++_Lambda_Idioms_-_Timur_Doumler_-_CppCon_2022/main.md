## Unary plus trick
```cpp
int main() {
    auto* fptr = [](int i){ return i * i;  };
}
```
This won't compile. Error - something like: `cannot deduce type for auto* from __lambda_1`.
But you can do this:
```cpp
int main() {
    auto* fptr = +[](int i){ return i * i;  }; // (or static_cast<int(*)(int> instead of +)
}
```
It works because the compiler:
- sees unary plus
- it is not implemented for the "generated" clojure, but the generated structe has a conversion function to a pointer to function. For which the unary plus is well known
- As a result we explicitely tell the compiler to cast lambda to a function pointer!

## Lambda captures
```cpp
int i = 0;
int j = 0;
auto f = [=] {  // You can ommit argument list, e.g. `()` if you don't need arguments
    return i == j;
};
```
This code will turn into *approximately* this:
```cpp
struct __lambda_2 {
    __lambda_2(int i, int j)
        : __i(i), __j(j)
        {}
    inline bool operator()() const {  // We're overloading operator() here
        return __i == __j;
    }
  private:
    int __i;
    int __j;
};
```
The standard guarantees that we will have these 2 additional memebers.

Let's add some references:
```cpp
int i = 0;
int j = 0;
auto f = [&] {
    return i == j;
};
```
This code will turn into *approximately* this:
```cpp
struct __lambda_2 {
    __lambda_2(int& i, int& j)
        : __i(i), __j(j)
        {}
    inline bool operator()() const { 
        return __i == __j;
    }
  private:
    int& __i;
    int& __j;
};
```
This time it's not said that we well have any additional data members. For example, we could have 1 pointer! It's made so to allow more room for compiler to optimize things.
Also by capturing by reference we allow lambda to modify i and j.

Let's look at lambdas inside structs/classes:
```cpp
struct X {
    void printAsync() {
        callAsync([this] {
                std::cout << i << '\n';
        });
    }
  private:
    int i = 42;
};
```
Turns into:
```cpp
struct X {
    void printAsync() {
        struct __lambda_3 {
            __lambda_3(X* _this)
                : __this(_this)
                {}
            
            void operator()() const {
                std::cout << __this->i << '\n';
            }

          private:
            X* __this;
        };

        callAsync(__lambda_3(this));
    }
  private:
    int i = 42;
}
```

## Lambda capture gothas

```cpp
int main() {
    static int i = 42;

    auto f = [=]{ ++i; };
    f();

    std::cout << i;
}
```
output: 43

Because lambda can capture only local variables. `i` is a static variable so you don't capture it by value here, you simply access it inside the lambda and modify it.
So, you can rewrite the code as:
```cpp
int main() {
    static int i = 42;

    auto f = []{ ++i; };  // Removed = from capture list here
    f();

    std::cout << i;
}
```
The same logic applies to global variables.

You also don't capture variables (even local ones) if they are not odr-used by the lambda:
```cpp
int main() {
    constexpr int i = 42;

    auto f = []{ std::cout << i << '\n';  };  // OK: i isn't odr-used
    f();
}
```
**But**:
```cpp
int main() {
    constexpr int i = 42;

    auto f = []{ std::cout << &i << '\n';  };  // Error: i is odr-used but not captured. You can, for example add 
                                               // & to capture list
    f();
}
```
**And**
```cpp
int main() {
    const int i = 42;

    auto f = []{ std::cout << i << '\n';  };  // OK, because... 
    f();
}
```
Because `i` is of integral typeand is marked const and is initialized with a literal value, therefore it's fully known at compiler time, therefore compiler implicitely marks it as constexpr. For example, the following won't compile (because float is not integral):
```cpp
int main() {
    const float i = 42.0f;

    auto f = []{ std::cout << i << '\n';  };  // Error
    f();
}
```
# Immediately Invoked Function Expressions (IIFE)
Simple example:
```cpp
int main() {
    []{ std::cout << "Hello world!\n"; }();  // We simply call the struct's operator() immediately
}
```
It's useful when we want to initialize a const variable using some complex logic. Example:
```cpp
int main() {
    const Foo foo = [&] {
        if (hasDatabase) {
            return getFooFromDatabase();
        } else {
            return getFooFromElsewhere();
        }
    }();
}
```
Immediately invoked lambdas work great with move-semantics:
```cpp
int main() {
    std::vector<Foo> foos;

    foos.emplace_back([&]{ 
        if (hasDatabase) {
            return getFooFromDatabase();
        } else {
            return getFooFromElsewhere();
        }
    }());  // Immediately call lambda here
}
```
The `()` operator might be easily overlooked, so you can also use std::invoke (since C++17) to increase code readability:
```cpp
int main() {
    std::vector<Foo> foos;

    foos.emplace_back(std::invoke([&]{ 
        if (hasDatabase) {
            return getFooFromDatabase();
        } else {
            return getFooFromElsewhere();
        }
    }));
}
```
# Call-once Lambda
Let's look at this struct:
```cpp
struct X {
    X() {
        std::cout << "called once!\n";
    }
}
```
We want it to behave so that the following code:
```cpp
int main() {
    X x1;
    X x2;
    X x3;
}
```
would ouput: called once!
But now it will print "called once!" 3 times.
The wanted effect can be achieved using a static variable:
```cpp
struct X {
    X() {
        auto static _ = []{std::cout << "called once!\n"; return 228; }();  // The value that's returned doesn't matter. We just have to return something
    }
}

```
This works because since C++11 we have a guarantee that a static variable is initialized only once + the initialization is also thread-safe.
Note: there is a bit of a runtime overhead because the compiler will put an if-check in X::X() in order not to re-initialize.
# Generic Lambdas
Example:
```cpp
std::map<int, std::string> httpErrors = {
    {400, "Bad Request"},
    {401, "Unauthorised"},
    {403, "Forbidden"},
    {404, "Not Found"}
};

std::for_each(httpErrors.begin(), httpErrors.end(),
        [](const auto& item) {
            std::cout << item.first << ':' << item.second << '\n';
        }
    );
```
What's happening?
```cpp
[](auto i){
    std::cout << i << '\n';
};
```
Will turn into something like this:
```cpp
struct __lambda_6 {  // Note: the struct itself is not templated!
    template <typename T>
    void operator()(T i) const {
        std::cout << i << '\n';
    }
    // Part (*) - start
    temaplate <typename T>
    using __func_type = void(*)(T i);

    temaplte <typename T>
    inline operator __func_type<T>() const noexcept {
        return &__invoke<T>;
    }

  private:
    template <typename T>
    static void __invoke(T i) {
        std::cout << i << '\n';
    }
    // Part (*) - end
};
```
Note: I should also mention that Part (\*) is generated only if the capture list is empty and it doesn't matter if the lambda is generic or not!

You can write this:
```cpp
void legacy_call(int(*f)(int)) {
    std::cout << f(7) << '\n';
}

int main() {
    legacy_call([](auto i){
        return i * i;
    });
}
```
But now the unary plus thick doesn't work:
```cpp
int main() {
    auto* fptr = +[](auto i) {  // Error: can't deduce template argument
        return i * i;
    };
}
```

Generic lambdas also support perfect-forwarding:
```cpp
std::vector<std::string> v;
auto f = [&v](auto&& item) {  // forwarding reference
    v.push_back(std::forward<decltype(item)>(item));
}
```
It's understandable to work because the lambda will look like this:
```cpp
struct __lambda_7 {
    __lambda_7(const std::vector<std::string>& v)
        : __v(v) {}

    template <typename T>
    void operator()(T&& item) const {
        __v.push_back(std::forward<decltype(item)>(item));
    }

  private:
    std::vector<std::string>& __v;
}
```
Lambdas also support variadic templates:
```cpp
auto f = [](auto&&... args) {
    (std::cout << ... << args);  // Fold-expressions since C++17
};

f(42, "hello", 1.5);
```

We can pass lambdas into other lambdas:
```cpp
auto twice = [](auto&& f) {
    return [=]{ f(); f(); };
}

auto print_hihi = twice([]{ std::cout << "hi"; });
print_hihi();
```

# Variable template lambda
