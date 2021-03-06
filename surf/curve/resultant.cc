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

#include <Bezout.h>
#include <BigInteger.h>

#include <resultant.h>

static Bezout<BigInteger>::Coeff3Poly convertToBigIntPoly(const Polyxyz& p)
{
	Bezout<BigInteger>::Coeff3Poly result;

	for(int i = 0; i < p.GetNumber(); i++) {
		MonomXYZ mon = p.Monom(i);
		BigInteger rat;
		rat = mon.Coeff()*1e15;
		if(!rat.isNull()) {
		    result.addMonom(monom(rat, mon.Exponent(0), mon.Exponent(1), mon.Exponent(2)));
		}
	}
	return result;
}

namespace {
void addMonomFromBigInt(CMonom<BigInteger, 2>* monom, void* ptr)
{
	Polyxy* poly = static_cast<Polyxy*>(ptr);
	MonomXY monxy;
	monxy.SetCoeff(monom->getCoeff().asDouble());
	monxy.Exponent(0, monom->getExponent(0));
	monxy.Exponent(1, monom->getExponent(1));
	*poly += monxy;
}
}

Polyxy resultant(const Polyxyz& p1, const Polyxyz& p2)
{
	Bezout<BigInteger>::Coeff3Poly ip1 = convertToBigIntPoly(p1) ;
	Bezout<BigInteger>::Coeff3Poly ip2 = convertToBigIntPoly(p2) ;

	Bezout<BigInteger>::Coeff2Poly ires = Bezout<BigInteger>::resultant(ip1, ip2);

	Polyxy retval2;
	ires.withMonomsPerform(addMonomFromBigInt, &retval2);
	retval2.Norm();
	return retval2;
}
