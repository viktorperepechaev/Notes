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
auto f = [=] {
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
