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



#include "mygmp.h"

void mpz_pow (mpz_t result, mpz_t z, int n)
{
	mpz_set_ui(result, 1);
	while (n>0) {
		mpz_mul(result, result, z);
		n--;
	}
}

ostream & operator << (ostream &os, mpz_t z)
{
	int size = mpz_sizeinbase (z, 10)+4;
	char *str = new char[size];
	
	mpz_get_str (str, 10, z);
	os << str;
	

	delete []str;
	return os;
}

ostream & operator << (ostream &os, mpq_t q)
{
	os << mpq_numref (q) << "/" << mpq_denref(q);
	return os;
}


void mpz_lcm (mpz_t lcm, mpz_t op1, mpz_t op2)
{
	mpz_t gcd;

	mpz_init (gcd);

	mpz_gcd (gcd, op1, op2);
	
	mpz_mul (lcm, op1, op2);
	mpz_divexact (lcm, lcm, gcd);


	mpz_clear (gcd);
}


mpq_t *mpq_array_new (unsigned int size)
{
	mpq_t *ptr = new mpq_t[size];
	unsigned int i;
	for (i=0; i<size; i++) 
		mpq_init(ptr[i]);
	return ptr;
}

void mpq_array_del (mpq_t *ptr, unsigned int size)
{
	unsigned int i;
	for (i=0; i<size; i++)
		mpq_clear(ptr[i]);
	delete [] ptr;
}

mpz_t *mpz_array_new (unsigned int size)
{
	mpz_t *ptr = new mpz_t[size];
	unsigned int i;
	for (i=0; i<size; i++) 
		mpz_init(ptr[i]);
	return ptr;
}

void mpz_array_del (mpz_t *ptr, unsigned int size)
{
	unsigned int i;
	for (i=0; i<size; i++)
		mpz_clear(ptr[i]);
	delete [] ptr;
}
