#pragma once

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

inline std::random_device rd;
inline std::mt19937 generator(rd());

inline static int Random(const int Min, const int Max)                      // Generate a random, uniformly distributed integer
{
    std::uniform_int_distribution<int> distribution(Min, Max);
    return distribution(generator);
}
inline static float Random(const float Min, const float Max)                // Generate a random, uniformly distributed float
{
    std::uniform_real_distribution<float> distribution(Min, Max);
    return distribution(generator);
}
inline static double Random(const double Min, const double Max)             // Generate a random, uniformly distributed double
{
    std::uniform_real_distribution<double> distribution(Min, Max);
    return distribution(generator);
}
/*
inline static Vector2 Random(const Vector2 Min, const Vector2 Max)          // Generate a random, uniformly distributed Vector2
{
    return { Random(Min.X, Max.X), Random(Min.Y, Max.Y) };
}
inline static Vector3 Random(const Vector3 Min, const Vector3 Max)          // Generate a random, uniformly distributed Vector3
{
    return { Random(Min.X, Max.X), Random(Min.Y, Max.Y), Random(Min.Z, Max.Z) };
}
inline static FTransform Random(const FTransform Min, const FTransform Max) // Generate a random, uniformly distributed FTransform
{
    return { Random(Min.GetPosition(), Max.GetPosition()), Random(Min.GetRotation(), Max.GetRotation()), Random(Min.GetScale(), Max.GetScale()) };
}
inline static FMomentum Random(const FMomentum Min, const FMomentum Max)    // Generate a random, uniformly distributed FMomentum
{
    return { Random(Min.GetPosition(), Max.GetPosition()), Random(Min.GetVelocity(), Max.GetVelocity()), Random(Min.GetAcceleration(), Max.GetAcceleration()) };
}
*/
template<typename T>
inline static T Random(const T Min, const T Max)                            // Generate a random, uniformly distributed templated type
{
    std::uniform_real_distribution<T> distribution(Min, Max);
    return distribution(generator);
}
template<typename T>
inline static std::vector<T> Random(const std::vector<T>& Vector)           // Generate a random, uniformly distributed templated vector type
{
    std::vector<T> ReturnVector;

    int i = 0;
    for (T Operand : Vector)
    {
        ReturnVector.push_back(Random(-Operand[i], Operand[i]));
        i++;
    }

    return ReturnVector;
}