//
// name: vector2D.h
// note: contains a two-dimensional vector class and related vector operations
//
#ifndef vector2D_h
#define vector2D_h
//
#include <iostream>
#include <math.h>


class Vector2D {
	public:
		double x;
		double y;
		inline Vector2D() { x = 0.0; y = 0.0; }
		inline Vector2D(double _x, double _y) { x = _x; y = _y; }	// constrcut a vector
		inline Vector2D(const Vector2D& v) { x = v.x; y = v.y; }	// copy constructor

		inline Vector2D operator+(const Vector2D& v) const { return Vector2D(x + v.x, y + v.y); }	// add
		inline void operator+=(const Vector2D& v) { x += v.x; y += v.y; }							// adds a 2D vector to the current vector.

		inline Vector2D operator-(const Vector2D& v) const { return Vector2D(x - v.x, y - v.y); }	// subtract
		inline void operator-=(const Vector2D& v) { x -= v.x; y -= v.y; }							// subtracts a 2D vector to the current vector.

		inline Vector2D operator*(const double a) const { return Vector2D(x * a, y * a); }	// scalar multiplication
		inline void operator*=(const double& a) { x *= a; y *= a; }							// multiplies the current vector by a sclar
		inline Vector2D operator/(const double a) const { return Vector2D(x / a, y / a); }	// scalar division
		inline void operator/=(const double& a) { x /= a; y /= a; }							// divides the current vector by a sclar

		inline double dot(const Vector2D& v) const { return x * v.x + y * v.y; }	// dot product
		inline double cross(const Vector2D& v) const { return x*v.y - y*v.x; }		// cross product (why do we need it?)

		inline bool operator==(const Vector2D& v) const { return (x == v.x && y == v.y); }	// vector equality, return True if the vectors are equal, false otherwise
		inline bool operator!=(const Vector2D& v) const { return (x != v.x || y != v.y); }	// vector inequality, return True if the vectors are not equal, false otherwise

		inline Vector2D normalize(const Vector2D& v) { double d = sqrt(x*x + y*y); if (d > 0) return Vector2D(x / d, y / d); return v; }	// normalizes the current vector
		inline Vector2D normalize() { double d = sqrt(x*x + y*y); if (d > 0) return Vector2D(x / d, y / d); return Vector2D(x, y); }	// normalizes the current vector
		inline double length() const { return sqrt(x*x + y*y); }	// returns the magnitude of the current vector
		inline double length2() const { return x*x + y*y; }			// returns the squared magnitude of the vector
};
#endif
