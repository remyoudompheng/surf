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



#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#include <iostream.h>

#include "mygmp.h"

class BigInteger
{
public:
	BigInteger();
	BigInteger(const BigInteger &b);
	~BigInteger();
	
	void operator = (double d)
		{
			mpz_set_d (mpz, d);
		}
	void operator = (const BigInteger &b);
	void operator *= (const BigInteger &b);
	void operator += (const BigInteger &b);
	void operator -= (const BigInteger &b);

	void print (ostream &os) const;
	void setLong (long int val)
		{
			mpz_set_si (mpz, val);
		}
	double asDouble()
		{
			return mpz_get_d (mpz);
		}
	friend inline bool isNull (const BigInteger &b);
	friend inline void negate (BigInteger &b);

private:
	mpz_t mpz;
};

inline ostream & operator << (ostream &os, const BigInteger &b)
{
	b.print(os);
	return os;
}

inline bool isNull (const BigInteger &b)
{
	return mpz_sgn(b.mpz)==0;
}

inline void negate (BigInteger &b)
{
	mpz_neg(b.mpz, b.mpz);
}
#endif
