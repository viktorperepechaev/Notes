[Youtube Video](https://www.youtube.com/watch?v=xBAduq0RGes)

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
We can make the whole struct have a template (with addition to templated methods we alreday have with `auto`).
```cpp
template <typename T>
constexpr auto c_cast = [](auto x) {
    return (T)x;
}
```
Turns into:
```cpp
template <typename T>
struct __lambda_9 {
    template <typename U>
    inline auto operator(U x) const {
        return (T)x;
    }
};

template <typename T>
auto c_cast = __lambda_9<T>();
```
This pattern can actually be useful in real life:
```cpp
using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;
using ns = std::chrono::nanoseconds;

struct Time {
    std::variant<ms, us, ns> time;

    auto convert(const auto& converter) {
        return std::visit(converter, time);
    }
};

int main() {
    Time t{ns(3000)};
    std::cout << t.convert(std::chrono::duration_cast<us>).count();  // Error
}
```
We get an error because `std::chrono::duration_cast` actually has 3 template parametres: `template< class ToDuration, class Rep, class Period >` and as a result `std::chrono::duration_cast<us>` isn't even fully declared (usually the last 2 parametres are deduced from the argument but in this scenerio we just want to pass this as some function).
Solution:
```cpp
using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;
using ns = std::chrono::nanoseconds;

struct Time {
    std::variant<ms, us, ns> time;

    auto convert(const auto& converter) {
        return std::visit(converter, time);
    }
};

template <typename T>
constexpr auto duration_cast = [](auto d) {
    return std::chrono::duration_cast<T>(d);
};

int main() {
    Time t{ns(3000)};
    std::cout << t.convert(duration_cast<us>).count();  // Works!
}
```
# C++14 - Init capture
Example:
```cpp
struct Widget {};
auto ptr = std::make_unique<Widget>();

auto f = [ptr = std::move(ptr)] {
    std::cout << ptr.get() << '\n';
};

assert(ptr == nullptr);  // passes
f();
```
The clojure will look like this:
```cpp
struct __lambda_8 {
    __lambda_8(std::unique_ptr<Widget> _ptr)
        : __ptr(std::move(_ptr))
    {}

    inline void operator()() const {
        std::cout << __ptr.get() << '\n';
    }

private:
    std::unique_ptr<Widget> __ptr; 
};

__lambda_8(std::move(ptr));
```
## Init Capture Optiization
Let's look at an example:
```cpp
int main() {
    const std::vector<std::string> vs = {"apple", "orange", "foobar", "lemon"};
    const std::string prefix = "foo";

    auto result = std::find_if(
        vs.begin(), vs.end(),
        [&prefix](const std::string& s) {
            return s == prefix + "bar";
        });

    if (result != vs.end()) {
        std::cout << *result << " -something found!\n";
    }
}
```
This is not very optimal because the concatination of prefix and "bar" will hapen at every iteration of the cycle inside of `std::find_if`.
To optimize this we can precompute prexif + "bar" and store it using init capture:
```cpp
int main() {
    const std::vector<std::string> vs = {"apple", "orange", "foobar", "lemon"};
    const std::string prefix = "foo";

    auto result = std::find_if(
        vs.begin(), vs.end(),
        [&prefix, str = prefix + "bar"](const std::string& s) {
            return s == str;
        });

    if (result != vs.end()) {
        std::cout << *result << " -something found!\n";
    }
}
```
# C++17
- Lambdas can be constexpr:
    ```cpp
    auto f = []() constexpr {
        return sizeof(*void);
    }

    std::array<int, f()> arr = {};
    ```
- CTAD (which we will use with lamdas)

## Lambda Overload Set
```cpp
template <typename... Ts>
struct overload : Ts... {
    using Ts::operator()...;
}

int main() {
    overload f = {
        [](int i){ std::cout << "got int\n"; },
        [](float i){ std::cout << "got float\n"; }
    };

    overload(1);
    overload(2.0f);
    /* It can be handy in the following way:
    std::variant<int, float> v = 0.2f;
    std::visit(f, v);
    */
}
``` 
This code will compile as-is since C++20 because in C++17 we need to add deduction guides to our class.
Note: overload here is an aggregate so initializer list directly initializes base classes.

# C++20
- lambdas can capture structure bindings:
    ```cpp
    struct Widget {
        float x, y;
    };

    auto [x, y] = Widget();
    auto f = [=] {
        std::cout << x << ' ' << y << '\n';
    };
    ```
- lambdas can capture parametre packs:
    ```cpp
    auto foo(auto... args) {
        std::cout << sizeof...(args) << '\n';
    }
    
    template <typename... Args>
    auto delay_invoke_foo(Args... args) {
        return [args...]() -> decltype(auto) {
            return foo(args...);
        }
    }
    ```
- lambdas can now be consteval:
    ```cpp
    auto f = [](int i) consteval {
        return i * i;
    };
    ```
- we got templated lambdas:
    ```cpp
    std::vector<int> data = {1, 2, 3, 4};
    std::erase_if(
        data,
        []<typename T>(T i) {
            return i % 2 == 0;
        }
    );
    ```
- lambdas allowed in unevaluated contexts
- lambdas without captures are now:
    - default-constructible
    - assignable
Now we can have lambdas as data members:
```cpp
class Widget {
    decltype([]{}) foo;
};
```
Or write like this:
```cpp
using WidgetSet = std::set<
    Widget,
    decltype([](Widget& lhs, Widget& rhs) { return lhs < rhs;  } )
    >;

WidgetSet widgets;
```
**Note: Every new lambda expression generates a new clojure type**
Examples:
```cpp
auto f1 = []{};
auto f2 = []{};
// f1 and f2 have different types
```
```cpp
auto f1 = []{};
auto f2 = f1;
// f1 and f2 have same type
```
```cpp
auto f1 = []{};
decltype(f1) f2;
// f1 and f2 have the same type
```
```cpp
using t = decltype([]{});
t f1;
t f2;
// f1 and f2  have the same type
```
```cpp
decltype([]{}) f1;
decltype([]{}) f2;
// f1 and f2 have different types
```
```cpp
template <auto = []{}>
struct X {};

X x1;
X x2;
// x1 and x2 have different types
```
So now we have a unique type generator!

# C++23

## Recursive lambdas
```cpp
int main() {
    auto f = [](this auto&& self, int i) {
        if (i == 0) {
            return 1;
        }
        return i * self(i - 1);
    };

    std::cout << f(5);  // 120
}
```

Now we can create a quite fancy tree traversal:
```cpp
struct Leaf {};
struct Node;
using Tree = std::variant<Leaf, Node*>;
struct Node {
    Tree left, right;
};

template <typename... Ts>
struct overload : Ts... { using Ts::operator()...; };

int CountLeaves(const Tree& tree) {
    return std::visit(overload{
            [] (const Leaf& leaf) { return 1; },
            [] (this const auto& self, const Node* node) -> int {
                    return std::visit(self, node->left) + std::visit(self, node->right);
                }
            }
        , tree);
}
```
