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

#include <simple.h>
#include <mymemory.h>
#include <IO.h>

#include<cmath>
#include<cstdlib>
#include<sstream>

namespace {

// here we store binomail coefficients:
struct binom_struct
{
	int created;
	int** data;
};
binom_struct b_c = { -1, 0 };

int** new_int_p(int n)
{
	if(n > 0) {
		int** i = static_cast<int**>(malloc(n*sizeof(int*)));
		if(i == 0) {
			IO::print_error("Out of memory!");
		}
		return i;
	} else {
		IO::print_error("n == 0 in new_int_p()!");
	}
	return 0; // suppress warnings
}

int** renew_int_p(int** i, int n)
{
	if(i == 0) {
		return new_int_p(n);
	} else if(n > 0) {
		i = static_cast<int**>(realloc(i, n*sizeof(int*)));
		if(i == 0) {
			IO::print_error("Out of memory!");
		}
		return  i;
	} else {
		IO::print_error("n == 0 in renew_int_p()!");
	}
	return 0; // suppress warnings
}

void binom_struct_create_row(int n)
{
	if(n == 0) {
		b_c.data[0][0] = 1;
	} else {
		int k = n - 1;
		b_c.data[n][0] = 1;
	
		for(int i = 1, j = 0; i < n; i++, j++) {
			b_c.data[n][i] = b_c.data[k][i] + b_c.data[k][j];
		}
		b_c.data[n][n] = 1;
	}
}

void binom_struct_add(int n)
{
	b_c.data = renew_int_p(b_c.data, n + 1);
	
	for(int i = b_c.created + 1; i <= n; i++) {
		b_c.data[i] = new int[i + 1];
		binom_struct_create_row(i);
	}
	
	b_c.created = n;
}
} // end of anonymous namespace


/* binomeal coefficient:

     / n \   / n-1 \   / n-1 \
     |   | = |     | + |     |
     \ k /   \ k-1 /   \  k  /
*/

int binom_coeff(int n, int k)
{
	if(n >= k && k >= 0) {
		if(n > b_c.created) {
			binom_struct_add(n);
		}
		return b_c.data[n][k];
	} else if(k > n && n >= 0) {
		return  0;
	} else {
		std::ostringstream os;
		os << "Can't compute binomial coefficient of"
		   << n << " and " << k << "!";
		IO::print_error(os.str());
	}
	return 0; // suppress warnings
}

/* multinomial coefficient: see
     Donald E. Knuth: The Art of Computer Programming, Volume 1 /
     Fundamental Algorithms, Addison-Wesley 1973, 2nd edition, p. 64
   for the recursion formula */

int multinom_coeff(int n, int* index, int k)
{
	int result = 1;
	int up = index[0];
	int down = 0;

	for(int i = 0, j = 1; j < k; i++, j++) {
		up += index[j];
		down += index[i];
    
		result *= binom_coeff(up, down);
	}
	return  result;
}
