# Coding Style Guide

Visual Studio's auto format is very nice.

## Formatting
- Everything should be in English.
- Use tabs for indents.
- Opening curly braces should go on a new line.
- Operators should be seperated by spaces.
- Newlines should be used to seperate functionally different code.
- Comment anything which could be unclear.
```cpp
struct Foo
{
    int bar = 9 + 10;
};
```

## Variables
- Local and member variables as well as constants should be named following camel case (ex. bool isPlayerAlive).
- Only use type deduction when it makes the code more readable (ex. auto itr = container.begin()).
- Don't use macros, but if you have to they should be named in all caps with underscores (ex. #define ECS_MAX_COMPONENTS).
- Variables which should not be directly used outside their implementation should be prefixed by "_". This does not apply to class members. (ex. bool _staleCache)
- For any integer type other than int, use an exact-width type (ex. int16_t).

## Functions
- Functions should be named in camel case with a capital first letter (ex. void DestroyEntity()).
- Functions which should not be directly used outside their implementation should be prefixed by "_" (ex. void _GetEntityID()).
- Lambdas should be formatted the same way as functions unless they only have one line.

## Containers
- Classes, Structs, and Enums should be named in camel case with a capital first letter (ex. class PhysicsSystem()).
- Avoid complicated inheritance.
- Don't use classes just as containers for static members, instead use namespaces.
- No friends allowed

```cpp
constexpr float g = -9.81f;

struct Transform
{
    Vector3 position;
	bool _staleCache = true;
};

void Integrate(Transform t)
{
    t.position += g;
}

auto lambda = [g](){ return g + 1; };
auto lambda2 = [g](float num)
{
    if(num % 3 == 0)
        g += num;
    return g;
};
```

## Namespaces
- All functionality related to the engine should be in the engine namespace, it can then be divided further using nested namespaces.
- Namespace names should follow camel case (ex. namespace renderSystem).
- Avoid using-directives for namespaces other than *engine* (ex. using namespace std).
```cpp
namespace engine
{
    void Start();

    namespace ecs
    {
        using Entity = uint32_t;
        Entity CreateEntity();
    }
}

using namespace engine; //This is fine

ecs::Entity e = ecs::CreateEntity();
```
