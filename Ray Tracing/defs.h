#ifndef _DEFS_H_
#define _DEFS_H_
#include <cmath>

class Scene;

typedef struct Vector3f
{
	union
	{
		float x;
		float r;
	};
	union
	{
		float y;
		float g;
	};
	union
	{
		float z;
		float b;
	};

	Vector3f() : x(0), y(0), z(0) {}

	Vector3f(const float x, const float y, const float z)
		: x(x), y(y), z(z) {}

	void operator+=(Vector3f v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	Vector3f operator+(Vector3f v) const
	{
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	void operator-=(Vector3f v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	Vector3f operator-(Vector3f v) const
	{
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	void operator*=(const float v)
	{
		x *= v;
		y *= v;
		z *= v;
	}

	Vector3f operator*(const float v) const
	{
		return Vector3f(x*v, y*v, z*v);
	}

	Vector3f operator/(const float v) const
	{
		return Vector3f(x / v, y / v, z / v);
	}


	float operator*(const Vector3f v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	
	Vector3f operator%(const Vector3f v) const
	{
		return Vector3f(y*v.z - z * v.y,
			z*v.x - x * v.z,
			x*v.y - y * v.x);
	}

	void operator%=(const Vector3f v)
	{
		*this = *this % v;
	}

	float magnitude() const
	{
		return sqrt(x*x + y * y + z * z);
	}

	float sqrMagnitude() const
	{
		return x * x + y * y + z * z;
	}

	void normalise()
	{
		float mag = magnitude();

		if (mag > 0)
		{
			x /= mag;
			y /= mag;
			z /= mag;
		}

	}

	Vector3f normalized() const
	{
		Vector3f v = (*this);

		v.normalise();

		return v;
	}

	void reverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	bool operator==(const Vector3f v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator!=(const Vector3f v) const
	{
		return !(*this == v);
	}

	Vector3f componentwise(Vector3f v) const
	{
		return Vector3f(x*v.x, y*v.y, z*v.z);
	}

	void componentwiseUpdate(Vector3f v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}

	static float determinant3x3(Vector3f x1,Vector3f x2,Vector3f x3)
	{
		return (x1.x * (x2.y * x3.z - x2.z * x3.y)) -
			(x1.y * (x2.x * x3.z - x3.x * x2.z)) +
			(x1.z * (x2.x * x3.y - x3.x * x2.y));
	} 
	
	bool isZero()
	{
		return x == 0 && y== 0 && z==0;
	}

} Vector3f;


/* Structure to hold return value from ray intersection routine.  */

typedef struct ReturnVal
{
	bool isValid;
	
	Vector3f intersectPoint;
	Vector3f normal;
	Vector3f incomingDir;
	float t;
	int matIndex;

} ReturnVal;

//
// The global variable through which you can access the scene data
//
extern Scene* pScene;


#endif
