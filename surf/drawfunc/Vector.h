/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2000 Johannes Gutenberg-Universitaet Mainz
 *   Authors: Stephan Endrass, Hans Huelf, Ruediger Oertel,
 *            Kai Schneider, Ralf Schmitt, Johannes Beigel
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */





#ifndef VECTOR_H
#define VECTOR_H


//  class Clip;

// ----------------------------------------------------------------------------
// -------------- vectorial class ---------------------------------------------
// ----------------------------------------------------------------------------

class Vector
{

public:
	double x;
	double y;
	double z;

	// ----------------- konstructors -------------------------

	Vector()                                : x(0.0),   y(0.0),   z(0.0)   {}
	Vector(double v1, double v2, double v3) : x(v1),    y(v2),    z(v3)    {}
	Vector(const Vector& Vec)               : x(Vec.x), y(Vec.y), z(Vec.z) {}

	// ----------------- reflexive operators ------------------------------------
	
	Vector& operator*=( const double scal )
		{ x *= scal; y *= scal; z *= scal; return *this; }
	Vector& operator/=( const double scal )
		{ x /= scal; y /= scal; z /= scal; return *this; }
	Vector& operator+=( const Vector& Vec2 )
		{ x += Vec2.x; y += Vec2.y; z += Vec2.z; return *this; }
	Vector& operator-=(const Vector& Vec2)
		{ x -= Vec2.x; y -= Vec2.y; z -= Vec2.z; return *this; }
	Vector& operator=(const double val)
		{ x = val; y = val; z = val; return *this; }
	Vector& operator=(const Vector& Vec2)
		{ x = Vec2.x; y = Vec2.y; z = Vec2.z; return *this; }
	
	// ----------------- regular operators --------------------------------------
	// -------------- vec times double ... scale --------------------------------
	Vector operator*( const double scal ) const
		{ Vector Res = *this; Res *= scal; return Res; }
	
	// -------------- vec plus vec ... addition ---------------------------------
	Vector operator+(const Vector& Vec2) const
		{ Vector Res = *this; Res += Vec2; return Res; }
	
	// -------------- vec minus vec ... subtraction -----------------------------
	Vector operator-(const Vector& Vec2) const
		{ Vector Res = *this; Res -= Vec2; return Res; }
	
	// -------------- unary minus ... negate vector -----------------------------
	Vector operator-() const
		{ Vector Res( -x, -y, -z ); return Res; }
	
	// -------------- cross product ... vec times vec ... result vec ------------
	Vector operator*( const Vector& Vec2 ) const
		{
			Vector Res;
			Res.x = y * Vec2.z - z * Vec2.y;
			Res.y = z * Vec2.x - x * Vec2.z;
			Res.z = x * Vec2.y - y * Vec2.x;
			return Res;
		}
	
	// -------------- vector comparison -----------------------------------------
	int operator==(const Vector& Vec2) const
		{
			return ( fabs( x - Vec2.x ) < 1e-6 &&
				 fabs( y - Vec2.y ) < 1e-6 &&
				 fabs( z - Vec2.z ) < 1e-6 );
		}
	
	
	// -------------------- element functions -----------------------------------
	
	Vector& Negate()
		{ x = -x; y = -y; z = -z; return *this; }
	
	void TurnToMe()
		{ if( z < 0.0 ) Negate(); }
	
	void Set(double nx, double ny, double nz)
		{ x = nx; y = ny; z = nz; }
	
	void Scale(Vector &Sc)
		{ x *= Sc.x; y *= Sc.y; z *= Sc.z; }
	void InvScale(Vector &Sc)
		{ x /= Sc.x; y /= Sc.y; z /= Sc.z; }

	double Length() const
		{ return sqrt( x*x + y*y + z*z ); }
	
	double SqLen() const
		{ return x*x + y*y + z*z; }
	
	void   Unify();
	void   Print() const;
// 	int    IsVisible( Clip* ) const;

	friend double DotProduct(const Vector& V1, const Vector& V2);
	friend double SquareLength(const Vector& Vec);
	friend Vector Unit(const Vector& V);
};



inline Vector CrossProduct (const Vector &u, const Vector &v)
{
	return Vector ( u.y*v.z - u.z*v.y,
			u.z*v.x - u.x*v.z,
			u.x*v.y - u.y*v.x );

}
// ----------------------------------------------------------------------------





// Obsolete
int     vector_i_equal        ( Vector*, Vector* );

#define vector_equal( u,v )             vector_i_equal( &(u),&(v) )




#endif // VECTOR_H
