#pragma once
#include <string>
#include <cmath>
#include <glm/glm.hpp>
const double PI = 3.14159265;

///Radians to degrees
float Degrees(float radians);

///Degrees to radians
float Radians(float degrees);

constexpr float comparisonPrecision = 0.0001f;

class Vector3;

///Two floats in one
class Vector2
{
public:
	//Constructors
	Vector2();
	Vector2(float all);
	Vector2(float _x, float _y);
	Vector2(Vector3 vec3);

	//Indexing
	float& operator[](int i);

	//Comparison
	bool operator==(const Vector2& rhs);

	//Add
	Vector2 operator+(const float& add);
	Vector2 operator+(const Vector2& add);
	Vector2& operator+=(const Vector2& add);
	//Subtract
	Vector2 operator-(const float& sub);
	Vector2 operator-(const Vector2& sub);
	Vector2& operator-=(const Vector2& sub);
	//Multiply
	// TODO: Replace use of "float" as templated type
	Vector2 operator*(const float& mult);
	Vector2 operator*(const Vector2& mult);
	Vector2& operator*=(const float& mult);
	Vector2& operator*=(const Vector2& mult);
	//Divide
	Vector2 operator/(const float& div);
	Vector2 operator/(const Vector2& div);
	Vector2& operator/=(const float& div);

	// TODO: Add templated return type
	float Length();
	Vector2 Normalize();
	Vector2 LeftNormal();
	Vector2 RightNormal();
	float Dot(const Vector2& b);

	std::string ToString();

	// TODO: Replace "float" as "double"
	float x, y;
};

///Three floats in one
class Vector3
{
public:
	//Constructors
	Vector3();
	Vector3(glm::vec3 vec);
	Vector3(float all);
	Vector3(float _x, float _y, float _z);
	Vector3(Vector2 vec2, float _z = 0);

	//Indexing
	float& operator[](int i);
	const float& operator[](int i) const;

	//Comparison
	bool operator==(const Vector3& rhs);
	bool operator!=(const Vector3& rhs);

	//Add
	Vector3 operator+(const float& add);
	Vector3 operator+(const Vector3& add);
	Vector3& operator+=(const Vector3& add);
	//Subtract
	Vector3 operator-(const float& sub);
	Vector3 operator-(const Vector3& sub);
	Vector3& operator-=(const Vector3& sub);
	//Multiply
	Vector3 operator*(const float& mult);
	Vector3 operator*(const Vector3& mult);
	Vector3& operator*=(const float& mult);
	//Divide
	Vector3 operator/(const float& div);
	Vector3 operator/(const Vector3& div);
	Vector3& operator/=(const float& div);

	float Length();
	Vector3 Normalize();
	Vector3 Pow(float power);
	float Dot(Vector3 b);
	Vector3 Cross(Vector3 b);

	//Comvert to a glm vec3
	glm::vec3 ToGlm();

	std::string ToString();

	float x, y, z;
};

///Four floats in one
class Vector4
{
public:
	//Constructors
	Vector4();
	Vector4(float all);
	Vector4(float _x, float _y, float _z, float _w);

	//Indexing
	float& operator[](int i);
	const float& operator[](int i) const;

	//Comparison
	bool operator==(const Vector4& rhs);
	bool operator!=(const Vector4& rhs);

	//Add
	Vector4 operator+(const float& add);
	Vector4 operator+(const Vector4& add);
	Vector4& operator+=(const Vector4& add);
	//Subtract
	Vector4 operator-(const float& sub);
	Vector4 operator-(const Vector4& sub);
	Vector4& operator-=(const Vector4& sub);
	//Multiply
	Vector4 operator*(const float& mult);
	Vector4 operator*(const Vector4& mult);
	Vector4& operator*=(const float& mult);
	//Divide
	Vector4 operator/(const float& div);
	Vector4 operator/(const Vector4& div);
	Vector4& operator/=(const float& div);

	std::string ToString();

	float x, y, z, w;
};
