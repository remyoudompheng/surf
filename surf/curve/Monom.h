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



#ifndef CMONOM_H
#define CMONOM_H

#include<cstring>
#include<iostream>

/**
 * template class for monoms with coefficient and an arbitrary number of variables
 */
template<class Coeff, int num> 
class CMonom
{
public:
	CMonom() {
		std::memset(exponent, 0, sizeof(int[num]));
	}
	CMonom(const CMonom& m) : coeff(m.coeff) {
		std::memcpy(exponent, m.exponent, sizeof(int[num]));
	}
	~CMonom() {}
	
	CMonom& operator=(const CMonom& m) {
		std::memcpy(exponent, m.exponent, sizeof(int[num]));
		coeff = m.coeff; 
		return *this;
	}
	
	CMonom& operator+=(const CMonom& m) {
		coeff += m.coeff;
		return *this;
	}
	CMonom& operator-=(const CMonom& m) {
		coeff -= m.coeff;
		return *this;
	}
	CMonom& operator*=(const CMonom& m) {
		for(int i = 0; i < num; i++) {
			exponent[i] += m.exponent[i];
		}
		coeff *= m.coeff;
		return *this;
	}
	
	int lexcmp(const CMonom& m) const {
		int i;
		for(i = 0; i < num && exponent[i] == m.exponent[i]; i++) {
			// empty
		}
		return i == num ? 0 : exponent[i] - m.exponent[i];
	}

	void print(std::ostream& os) const {
		static const char vars[] = "xyzabcdefg";
		os << "(" << coeff << ")";
		for(int i = 0; i < num; i++) {
			os << "*" << vars[i] << "^" << exponent[i];
		}
	}
	
	Coeff& getCoeff() {
		return coeff;
	}
	const Coeff& getCoeff() const {
		return coeff;
	}
	
	int& getExponent(int var) {
		assert(var >= 0 && var < num);
		return exponent[var];
	}

	static int size() {
		return num;
	}

	bool isNull() const {
		return coeff.isNull();
	}

	CMonom& negate() {
		coeff.negate();
		return *this;
	}

protected:
	int exponent[num];
	Coeff coeff;
};

template<class Coeff, int num> 
std::ostream& operator<<(std::ostream& os, const CMonom<Coeff, num>& m)
{
	m.print(os);
 	return os;
}

template<class Coeff> 
CMonom<Coeff, 2> monom(const Coeff& coeff, int e0, int e1)
{
	CMonom<Coeff, 2> mon;
	mon.getCoeff() = coeff;
	mon.getExponent(0) = e0;
	mon.getExponent(1) = e1;
	return mon;
}

template<class Coeff> 
CMonom<Coeff, 3> monom(const Coeff& coeff, int e0, int e1, int e2)
{
	CMonom<Coeff, 3> mon;
	mon.getCoeff() = coeff;

	mon.getExponent(0) = e0;
	mon.getExponent(1) = e1;
	mon.getExponent(2) = e2;
	return mon;
}

#endif // !CMONOM_H
