/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2001 Johannes Gutenberg-Universitaet Mainz
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



#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <BigInteger.h>

BigInteger::BigInteger()
{
	mpz_init (mpz);
}

BigInteger::BigInteger (const BigInteger &b)
{
	mpz_init(mpz);
	mpz_set (mpz, b.mpz);
}

BigInteger::~BigInteger()
{
	mpz_clear (mpz);
}

void BigInteger::operator=(const BigInteger &b)
{
	mpz_set (mpz, b.mpz);
}

void BigInteger::operator *= (const BigInteger &b)
{
	mpz_mul (mpz, mpz, b.mpz);
}

void BigInteger::operator += (const BigInteger &b)
{
	mpz_add (mpz, mpz, b.mpz);
}

void BigInteger::operator -= (const BigInteger &b)
{
	mpz_sub (mpz, mpz, b.mpz);
}

void BigInteger::print (ostream &os) const
{
	int size = mpz_sizeinbase (mpz, 10)+4;
	char *str = new char[size];
	
	mpz_get_str (str, 10, mpz);
	os << str;
	

	delete []str;
}
