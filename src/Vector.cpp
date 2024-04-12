#include <stdexcept>
#include "engine/Vector.h"

////////// Math //////////
float Degrees(float radians)
{
	return radians * (180 / PI);
}
float Radians(float degrees)
{
	return degrees * (PI / 180);
}

////////// Vector 2 //////////

//Constructors
Vector2::Vector2()
{
	x = 0;
	y = 0;
}
Vector2::Vector2(float all)
{
	x = all;
	y = all;
}
Vector2::Vector2(float _x, float _y)
{
	x = _x;
	y = _y;
}
Vector2::Vector2(Vector3 vec3)
{
	x = vec3.x;
	y = vec3.y;
}

//Comparison
bool Vector2::operator==(const Vector2& rhs)
{
	return abs(x - rhs.x) < comparisonPrecision && abs(y - rhs.y) < comparisonPrecision;
}

//Indexing
float& Vector2::operator[](int i)
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	default:
		throw std::out_of_range("Index in Vector2 out of range");
	}
}

//Add
Vector2 Vector2::operator+(const float& add) const
{
	return Vector2(x + add, y + add);
}
Vector2 Vector2::operator+(const Vector2& add) const
{
	return Vector2(x + add.x, y + add.y);
}
Vector2& Vector2::operator+=(const Vector2& add)
{
	x += add.x;
	y += add.y;
	return *this;
}

//Subtract
Vector2 Vector2::operator-(const float& sub) const
{
	return Vector2(x - sub, y - sub);
}
Vector2 Vector2::operator-(const Vector2& sub) const
{
	return Vector2(x - sub.x, y - sub.y);
}
Vector2& Vector2::operator-=(const Vector2& sub)
{
	x -= sub.x;
	y -= sub.y;
	return *this;
}

//Multiply
Vector2 Vector2::operator*(const float& mult) const
{
	return Vector2(x * mult, y * mult);
}
Vector2 Vector2::operator*(const Vector2& mult) const
{
	return Vector2(x * mult.x, y * mult.y);
}
Vector2& Vector2::operator*=(const float& mult)
{
	x *= mult;
	y *= mult;
	return *this;
}
Vector2& Vector2::operator*=(const Vector2& mult)
{
	x *= mult.x;
	y *= mult.y;
	return *this;
}

//Divide
Vector2 Vector2::operator/(const float& div) const
{
	return Vector2(x / div, y / div);
}
Vector2 Vector2::operator/(const Vector2& div) const
{
	return Vector2(x / div.x, y / div.y);
}
Vector2& Vector2::operator/=(const float& div)
{
	x /= div;
	y /= div;
	return *this;
}

float Vector2::Length() const
{
	return sqrt(x * x + y * y);
}
Vector2 Vector2::Normalize()
{
	float length = sqrt(x * x + y * y);
	return Vector2(x / length, y / length);
}
float Vector2::Dot(const Vector2& b) const
{
	return (x * b.x) + (y * b.y);
}
Vector2 Vector2::LeftNormal() const
{
	return Vector2(-y, x).Normalize();
}
Vector2 Vector2::RightNormal() const
{
	return Vector2(y, -x).Normalize();
}

std::string Vector2::ToString() const
{
	char buff[255];
	std::snprintf(buff, sizeof(buff) - 1, "(%f, %f)", x, y);
	return std::string(buff);
}


////////// Vector 3 //////////

//Constructors
Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}
Vector3::Vector3(glm::vec3 vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}
Vector3::Vector3(float all)
{
	x = all;
	y = all;
	z = all;
}
Vector3::Vector3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}
Vector3::Vector3(Vector2 vec2, float _z)
{
	x = vec2.x;
	y = vec2.y;
	z = _z;
}

//Comparison
bool Vector3::operator==(const Vector3& rhs)
{
	return x == rhs.x && y == rhs.y && z == rhs.z;
}
bool Vector3::operator!=(const Vector3& rhs)
{
	return !(x == rhs.x && y == rhs.y && z == rhs.z);
}

//Indexing
float& Vector3::operator[](int i)
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	default:
		throw std::out_of_range("Index in Vector3 out of range");
	}
}
const float& Vector3::operator[](int i) const
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	default:
		throw std::out_of_range("Index in Vector3 out of range");
	}
}

//Addition
Vector3 Vector3::operator+(const float& add) const
{
	return Vector3(x + add, y + add, z + add);
}
Vector3 Vector3::operator+(const Vector3& add) const
{
	return Vector3(x + add.x, y + add.y, z + add.z);
}
Vector3& Vector3::operator+=(const Vector3& add)
{
	x += add.x;
	y += add.y;
	z += add.z;
	return *this;
}

//Subtraction
Vector3 Vector3::operator-(const float& sub) const
{
	return Vector3(x - sub, y - sub, z - sub);
}
Vector3 Vector3::operator-(const Vector3& sub) const
{
	return Vector3(x - sub.x, y - sub.y, z - sub.z);
}
Vector3& Vector3::operator-=(const Vector3& sub)
{
	x -= sub.x;
	y -= sub.y;
	z -= sub.z;
	return *this;
}

//Multiplication
Vector3 Vector3::operator*(const float& mult) const
{
	return Vector3(x * mult, y * mult, z * mult);
}
Vector3 Vector3::operator*(const Vector3& mult) const
{
	return Vector3(x * mult.x, y * mult.y, z * mult.z);
}
Vector3& Vector3::operator*=(const float& mult)
{
	x *= mult;
	y *= mult;
	z *= mult;
	return *this;
}

//Division
Vector3 Vector3::operator/(const float& div) const
{
	return Vector3(x / div, y / div, z / div);
}
Vector3 Vector3::operator/(const Vector3& div) const
{
	return Vector3(x / div.x, y / div.y, z / div.z);
}
Vector3& Vector3::operator/=(const float& div)
{
	x /= div;
	y /= div;
	z /= div;
	return *this;
}

float Vector3::Length() const
{
	return sqrt(x * x + y * y + z * z);
}
Vector3 Vector3::Pow(float power)
{
	return Vector3(pow(x, power), pow(y, power), pow(z, power));
}
Vector3 Vector3::Normalize()
{
	float length = sqrt(x * x + y * y + z * z);
	return Vector3(x / length, y / length, z / length);
}
float Vector3::Dot(Vector3 b) const
{
	return (x * b.x) + (y * b.y) + (z * b.z);
}
Vector3 Vector3::Cross(Vector3 b) const
{
	return Vector3((y * b.z) - (z * b.y), (z * b.x) - (x * b.z), (x * b.y) - (y * b.x));
}

glm::vec3 Vector3::ToGlm() const
{
	return glm::vec3(x, y, z);
}

std::string Vector3::ToString() const
{
	char buff[255];
	std::snprintf(buff, sizeof(buff) - 1, "(%f, %f, %f)", x, y, z);
	return std::string(buff);
}

//////////// Vector 4 //////////////

//Constructors
Vector4::Vector4()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}
Vector4::Vector4(float all)
{
	x = all;
	y = all;
	z = all;
	w = all;
}
Vector4::Vector4(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

//Comparison
bool Vector4::operator==(const Vector4& rhs)
{
	return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}
bool Vector4::operator!=(const Vector4& rhs)
{
	return !(x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
}

//Indexing
float& Vector4::operator[](int i)
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		throw std::out_of_range("Index in Vector4 out of range");
	}
}
const float& Vector4::operator[](int i) const
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		throw std::out_of_range("Index in Vector4 out of range");
	}
}

//Addition
Vector4 Vector4::operator+(const float& add)
{
	return Vector4(x + add, y + add, z + add, w + add);
}
Vector4 Vector4::operator+(const Vector4& add)
{
	return Vector4(x + add.x, y + add.y, z + add.z, w + add.w);
}
Vector4& Vector4::operator+=(const Vector4& add)
{
	x += add.x;
	y += add.y;
	z += add.z;
	w += add.w;
	return *this;
}

//Subtraction
Vector4 Vector4::operator-(const float& sub)
{
	return Vector4(x - sub, y - sub, z - sub, w - sub);
}
Vector4 Vector4::operator-(const Vector4& sub)
{
	return Vector4(x - sub.x, y - sub.y, z - sub.z, w - sub.w);
}
Vector4& Vector4::operator-=(const Vector4& sub)
{
	x -= sub.x;
	y -= sub.y;
	z -= sub.z;
	w -= sub.w;
	return *this;
}

//Multiplication
Vector4 Vector4::operator*(const float& mult)
{
	return Vector4(x * mult, y * mult, z * mult, w * mult);
}
Vector4 Vector4::operator*(const Vector4& mult)
{
	return Vector4(x * mult.x, y * mult.y, z * mult.z, w * mult.w);
}
Vector4& Vector4::operator*=(const float& mult)
{
	x *= mult;
	y *= mult;
	z *= mult;
	w *= mult;
	return *this;
}

//Division
Vector4 Vector4::operator/(const float& div)
{
	return Vector4(x / div, y / div, z / div, w / div);
}
Vector4 Vector4::operator/(const Vector4& div)
{
	return Vector4(x / div.x, y / div.y, z / div.z, w / div.w);
}
Vector4& Vector4::operator/=(const float& div)
{
	x /= div;
	y /= div;
	z /= div;
	w /= div;
	return *this;
}

std::string Vector4::ToString()
{
	char buff[255];
	std::snprintf(buff, sizeof(buff) - 1, "(%f, %f, %f, %f)", x, y, z, w);
	return std::string(buff);
}
