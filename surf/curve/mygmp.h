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



#ifndef MYGMP_H
#define MYGMP_H

#include <gmp.h>

#include<iostream>

extern void mpz_lcm (mpz_t lcm, mpz_t op1, mpz_t op2);
extern ostream & operator << (ostream &os, mpz_t z);
extern ostream & operator << (ostream &os, mpq_t q);

extern void mpz_pow (mpz_t result, mpz_t z, int n);

inline void mpq_add_self (mpq_t a, mpq_t b)
{
	mpq_add (a,a,b);
}

inline void mpq_sub_self (mpq_t a, mpq_t b)
{
	mpq_sub (a,a,b);
}

inline void mpz_add_self (mpz_t a, mpz_t b)
{
	mpz_add (a,a,b);
}

inline void mpz_sub_self (mpz_t a, mpz_t b)
{
	mpz_sub (a,a,b);
}

inline void mpz_mul_self (mpz_t a, mpz_t b)
{
	mpz_mul (a,a,b);
}



inline void mpz_neg_self (mpz_t a)
{
	mpz_neg(a,a);
}

extern mpq_t *mpq_array_new (unsigned int size);
extern void mpq_array_del (mpq_t *ptr, unsigned int size);

extern mpz_t *mpz_array_new (unsigned int size);
extern void mpz_array_del (mpz_t *ptr, unsigned int size);

#endif
