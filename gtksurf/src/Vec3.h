/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */

#ifndef VEC3_H
#define VEC3_H

#include <GL/gl.h>

#include<iostream>
#include<cmath>

template<class T>
class Vec3 {
public:
	Vec3() : x(0), y(0), z(0) {}
	Vec3(const Vec3<T>& src) : x(src.x), y(src.y), z(src.z) {}
	Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

	virtual ~Vec3() {}

	Vec3<T>& operator=(const Vec3& src) {
		x = src.x;
		y = src.y;
		z = src.z;
		return *this;
	}

	Vec3<T> operator*(T l) const { // scaling
		return Vec3<T>(l*x, l*y, l*z);
	}

	Vec3<T> operator/(T l) const { // scaling
		return Vec3<T>(x/l, y/l, z/l);
	}

	T dotProduct(const Vec3<T>& rhs) const {
		return x*rhs.x + y*rhs.y + z*rhs.z;
	}

	Vec3<T> crossProduct(const Vec3<T>& rhs) const {
		return Vec3<T>(y*rhs.z - z*rhs.y,
			       z*rhs.x - x*rhs.z,
			       x*rhs.y - z*rhs.x);
	}

	Vec3<T> operator-() const { // unary minus
		return Vec3<T>(-x, -y, -z);
	}

	Vec3<T> operator+(const Vec3<T>& rhs) const {
		return Vec3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vec3<T> operator-(const Vec3<T>& rhs) const { // binary minus
		return Vec3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	T normSq() const { return x*x + y*y + z*z; }

	T norm() const { return std::sqrt(normSq()); }

	Vec3<T> unit() const {
		T length = norm();
		if(length > 0) {
			return (*this)/length;
		} else {
			Vec3<T> null;
			return null;
		}
	}

	T x, y, z; // public elements!
};

typedef Vec3<GLfloat> GLvector;

#endif //!VEC3_H
