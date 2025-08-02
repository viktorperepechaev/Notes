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
