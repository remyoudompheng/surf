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



#ifndef Bezout_h
#define Bezout_h

#include <iostream.h>

#include "TreePolynom.h"
#include "Monom.h"
#include "Matrix.h"

template<class Coeff>
class Bezout
{
public:
	typedef CMonom<Coeff,2> Coeff2Monom;
	typedef CMonom<Coeff,3> Coeff3Monom;
	typedef TreePoly<Coeff2Monom> Coeff2Poly;
	typedef TreePoly<Coeff3Monom> Coeff3Poly;

	struct FillArrayStruct 
	{
		Coeff2Poly *bezoutArray;
		int bezoutDegree;
	};

	static void fillArray (Coeff3Monom *mon, void *ptr)
		{
			FillArrayStruct *fillArrayStruct = (FillArrayStruct *) ptr;
			assert(mon);
			Coeff2Monom mon2;
			mon2.getCoeff() = mon->getCoeff();
			mon2.getExponent(0) = mon->getExponent(0);
			mon2.getExponent(1) = mon->getExponent(1);
			fillArrayStruct->bezoutArray[fillArrayStruct->bezoutDegree-mon->getExponent(2)].addMonom(mon2);
		}

	// compute the bezout matrix
	static Matrix<Coeff2Poly> * computeBezoutMatrix (Coeff3Poly f, Coeff3Poly g)
		{
			int n = f.degreeInVariable(2);
			int m = g.degreeInVariable(2);
			if (n<m)
				return computeBezoutMatrix (g,f);
			assert (n>=m);
			
			Coeff2Poly *a = new Coeff2Poly [n+1];
			Coeff2Poly *b = new Coeff2Poly [m+1];

			FillArrayStruct fillArrayStruct;

			fillArrayStruct.bezoutArray = a;
			fillArrayStruct.bezoutDegree = n;
			f.withMonomsPerform(fillArray, &fillArrayStruct);
	
			fillArrayStruct.bezoutArray = b;
			fillArrayStruct.bezoutDegree = m;
			g.withMonomsPerform(fillArray, &fillArrayStruct);

			
			Matrix<Coeff2Poly> *matrix = new Matrix<Coeff2Poly> (n);

			int i;
			for (i=1; i<m+1; i++) {
				int j1;
				int j2;
				for (j1=i; j1<m+1; j1++) {
					for (j2=0; j2<i; j2++) {
						matrix->getElement(m-i, j1+j2-i) += b[j1]*a[j2];
					}
				} 
				
				for (j1=i; j1<n+1; j1++) {
					for (j2=0; j2<i; j2++) {
						matrix->getElement(m-i, j1+j2-i) -= a[j1]*b[j2];
					}
				}
			}
	
			for(i=m+1; i<n+1; i++) {
				int j;
				for (j=1; j<=m+1; j++) {
					matrix->getElement(i-1, i-m-1+j-1) = b[j-1];
				}
			}

			delete [] a;
			delete [] b;
			
			return matrix;
		}

	// compute the resultant
	static Coeff2Poly resultant (Coeff3Poly f, Coeff3Poly g)
		{
			int n = f.degreeInVariable(2);
			int m = g.degreeInVariable(2);

			Matrix<Coeff2Poly> * matrix = n>=m ? computeBezoutMatrix (f,g) : computeBezoutMatrix (f,g);	
	
#if 0
			// threadedDeterminant does not work. See Matrix.h.
			Coeff2Poly det = matrix->threadedDeterminant();
#else
			Coeff2Poly det = matrix->det();
#endif
			TRACE(det);
			delete matrix;
			
			return det;
		}

};
#endif
