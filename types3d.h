#pragma once
#include "olcPixelGameEngine.h"
#include <fstream>
#include <sstream>
#include "constants.h"

using namespace std;
using namespace olc;
using namespace utils;

struct intPair
{
	intPair(int a = 0, int b = 0)
		: a(a), b(b)
	{}

	union
	{
		struct { int a, b; };
		struct { int x, y; };
		int n[2]{ 0, 0 };
	};
};

struct mat4x4
{
	float m[4][4] = { 0 }; //[row][col]

	mat4x4 operator*(const mat4x4& a) const
	{
		mat4x4 matrix;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				matrix.m[r][c] = m[r][0] * a.m[0][c] + m[r][1] * a.m[1][c] + m[r][2] * a.m[2][c] + m[r][3] * a.m[3][c];
		return matrix;
	}

	mat4x4& operator*=(const mat4x4 rhs)
	{
		*this = *this * rhs;
		return *this;
	}
};

struct vec2d
{
	union
	{
		struct { float x, y, z; };
		struct { float u, v, w; };
		float n[3] = { 0.0f, 0.0f, 1.0f };
	};

	vec2d()
		: x(0), y(0), z(1)
	{}
	vec2d(float x, float y)
		: x(x), y(y), z(1)
	{}
	vec2d(float x, float y, float z)
		: x(x), y(y), z(z)
	{}

	float length() const
	{
		return sqrtf(x * x + y * y);
	}

	float dot(const vec2d& a) const
	{
		return (x * a.x + y * a.y);
	}

	float angle(const vec2d& a) const
	{
		return acosf( dot(a)/(length() * a.length()) );
	}

	float dist(const vec2d& a) const
	{
		float dx = x - a.x;
		float dy = y - a.y;
		return sqrtf(dx*dx + dy*dy);
	}

	vec2d normalized() const
	{
		return *this / length();
	}

	vec2d lerp(const vec2d& a, float t)
	{
		return {
			x + (a.x-x)*t,
			y + (a.y-y)*t,
			1
		};
	}

	vf2d asVf() const
	{
		return vf2d(x, y);
	}

#pragma region Operator Overloads
	vec2d operator+(const vec2d& rhs) const
	{
		vec2d res = {
			this->x + rhs.x,
			this->y + rhs.y,
			1
		};
		return res;
	}
	vec2d operator-(const vec2d& rhs) const
	{
		vec2d res = {
			this->x - rhs.x,
			this->y - rhs.y,
			1
		};
		return res;
	}
	vec2d operator*(const float rhs) const
	{
		vec2d res = {
			this->x * rhs,
			this->y * rhs,
			1
		};
		return res;
}
	vec2d operator/(const float rhs) const
	{
		vec2d res = {
			this->x / rhs,
			this->y / rhs,
			1
		};
		return res;
	}
	vec2d& operator+=(const vec2d& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	vec2d& operator-=(const vec2d& rhs)
	{
		*this = *this - rhs;
		return *this;
	}
	vec2d& operator*=(const float rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	vec2d& operator/=(const float rhs)
	{
		*this = *this / rhs;
		return *this;
	}
#pragma endregion
};

struct quaternion
{

};

struct vec3d
{
	union
	{
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
		float n[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	};

	vec3d()
		: x(0), y(0), z(0), w(1)
	{}
	
	vec3d(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{}

	vec3d(float x, float y, float z)
		: x(x), y(y), z(z), w(1)
	{}

	float length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	vec3d normalized() const
	{
		return *this / length();
	}

	float angle(const vec3d& a) const
	{
		return acosf( dot(a)/(length() * a.length()) );
	}

	float dot(const vec3d& a) const
	{
		return (x * a.x + y * a.y + z * a.z);
	}

	vec3d cross(const vec3d& a) const
	{
		return {
			n[1] * a.n[2] - n[2] * a.n[1],
			n[2] * a.n[0] - n[0] * a.n[2],
			n[0] * a.n[1] - n[1] * a.n[0],
			1
		};
	}

	vec3d lerp(const vec3d& a, float t)
	{
		return {
			x + (a.x-x)*t,
			y + (a.y-y)*t,
			z + (a.z-z)*t,
			1
		};
	}

	//Returns the vector projected onto the horizontal plane
	vec2d as2d() const
	{
		return { x, z };
	}

#pragma region Operator Overloads
	//Returns the product of the current vector with the specified matrix
	vec3d operator*(const mat4x4& m) const
	{
		return vec3d(
						(x * m.m[0][0]) + (y * m.m[1][0]) + (z * m.m[2][0]) + (m.m[3][0]),
						(x * m.m[0][1]) + (y * m.m[1][1]) + (z * m.m[2][1]) + (m.m[3][1]),
						(x * m.m[0][2]) + (y * m.m[1][2]) + (z * m.m[2][2]) + (m.m[3][2]),
						(x * m.m[0][3]) + (y * m.m[1][3]) + (z * m.m[2][3]) + (m.m[3][3])
					);
	}

	vec3d operator+(const vec3d& rhs) const
	{
		vec3d res = {
			this->x + rhs.x,
			this->y + rhs.y,
			this->z + rhs.z,
			1
		};
		return res;
	}
	vec3d operator-(const vec3d& rhs) const
	{
		vec3d res = {
			this->x - rhs.x,
			this->y - rhs.y,
			this->z - rhs.z,
			1
		};
		return res;
	}
	vec3d operator*(const float rhs) const
	{
		vec3d res = {
			this->x * rhs,
			this->y * rhs,
			this->z * rhs,
			1
		};
		return res;
}
	vec3d operator/(const float rhs) const
	{
		vec3d res = {
			this->x / rhs,
			this->y / rhs,
			this->z / rhs,
			1
		};
		return res;
	}
	vec3d& operator+=(const vec3d& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	vec3d& operator-=(const vec3d& rhs)
	{
		*this = *this - rhs;
		return *this;
	}
	vec3d& operator*=(const float rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	vec3d& operator/=(const float rhs)
	{
		*this = *this / rhs;
		return *this;
	}
#pragma endregion
};
