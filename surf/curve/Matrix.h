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


#ifndef MATRIX_H
#define MATRIX_H

#include <assert.h>
#include "Thread.h"
#include <pthread.h>

// #define DEBUG
#include "debug.h"

template<class Type> 
class Matrix
{
private:
	Matrix(const Matrix &);
	void operator=(const Matrix &);

public:
	Matrix(int newSize) : size(newSize), elems(new Type[newSize*newSize])	
		{}
	~Matrix()
		{ delete []elems;	};

	
	Type &      getElement (int row, int column)       
		{ return elems[getIndex(row,column)];	};

	const Type& getElement (int row, int column) const 
		{ return elems[getIndex(row, column)];	};

	Type det() const;
	int getSize() const	{return size;	};
	Type threadedDeterminant() const;
	
protected:
	int size;
	Type *elems;


	inline int getIndex (int row, int col) const
		{
			assert(row >= 0 && col >= 0 && row<size && col<size);
			return row*size+col;
		};

	void deleteRowAndColumn (int row,  int col, Matrix<Type>& m) const;

	void getBestRow (int &row, int &zeros) const;
	void getBestColumn (int &col, int &zeros) const;


	static void *computeDeterminant(void *ptr);
};

template<class Type>
void * Matrix<Type>::computeDeterminant(void *ptr)
{
	Matrix<Type> *matrix = (Matrix<Type> *) ptr;
	Type *retval = new Type (matrix->det());
	delete matrix;
	return retval;
}


// well, this function does not work. the problem is that the polynomials in
// the matrix are not copied and the RefCounter class is not thread safe.
// protecting RefCounter's release and retain with mutexes leads to poore 
// performance. This will be fixed. 
// - Ralf

template<class Type>
Type Matrix<Type>::threadedDeterminant () const
{
	if (size==1)
		return elems[0];
	else if (size==2)
		return elems[0]*elems[3]-elems[1]*elems[2];

	Type result;
	setNull(result);
	if (Thread::shouldStop()) {
		return result;
	}




	int bestRow;
	int bestRowZeros;
	
	int bestCol=-1;
	int bestColZeros=-1;
	
	getBestRow(bestRow, bestRowZeros);
	getBestColumn(bestCol, bestColZeros);
	
	TRACE(bestRow);
	TRACE(bestRowZeros);
	
	TRACE(bestCol);
	TRACE(bestColZeros);

	struct 
	{
		pthread_t thread;
		bool nonzero;
	} threads [size];
	memset (threads, 0, size*sizeof(threads[0]));

	void *subdeterminant;
	if (bestRowZeros > bestColZeros) {
		int col;
		for (col=0; col<getSize(); col++) {
			if (isNull(getElement(bestRow, col)))
				continue;
			Matrix *m = new Matrix (size-1);
			deleteRowAndColumn(bestRow, col, *m);
			int err = pthread_create(&threads[col].thread, 0, computeDeterminant, m);
			assert(err==0);
			cerr << "started" << endl;
			threads[col].nonzero=true;
		}

		cerr << "okay...started some threads" << endl;

		for (col=0; col<getSize(); col++) {

			if (threads[col].nonzero) {
				int err = pthread_join (threads[col].thread, &subdeterminant);
				assert(err==0);

				Type *subdet = (Type *) subdeterminant;
				if ((bestRow+col) % 2 == 0) {
					result += *subdet*getElement(bestRow, col);
				} else {
					result -= *subdet*getElement(bestRow, col);
				}
				delete subdet;

				cerr << "joined..." << endl;
			}
		}

	} else {
		int row;
		for (row=0; row<getSize(); row++) {
			if (isNull(getElement(row, bestCol)))
				continue;

			Matrix *m = new Matrix (size-1);
			deleteRowAndColumn(row, bestCol, *m);

			int err = pthread_create(&threads[row].thread, 0, computeDeterminant, m);
			assert(err==0);
			cerr << "started" << endl;
			threads[row].nonzero=true;
		}

		for (row=0; row<getSize(); row++) {
			if (threads[row].nonzero) {
				int err = pthread_join (threads[row].thread, &subdeterminant);
				assert(err==0);
				Type *subdet = (Type *) subdeterminant;
				if ((bestCol+row) % 2 == 0) {
					result += *subdet*getElement(row, bestCol);
				} else {
					result -= *subdet*getElement(row, bestCol);
				}
				delete subdet;

				cerr << "joined..." << endl;
			}
		}
		
	}

	return result;
	
}


template<class Type>
void Matrix<Type>::getBestRow (int &row, int &zeros) const
{
	int minNonZero=getSize();
	int bestRow=0;
	
	int r;
	for (r=0; r<getSize(); r++) {
		int nonZero=0;
		int c;

		for (c=0; c<getSize() && nonZero<minNonZero; c++) {
			if (!isNull(getElement(r,c))) {
				nonZero++;
			}
		}
		
		if (nonZero < minNonZero) {
			minNonZero = nonZero;
			bestRow = r;
		}
	}
	
	row = bestRow;
	zeros = getSize()-minNonZero;
}

template<class Type>
void Matrix<Type>::getBestColumn (int &col, int &zeros) const
{
	int minNonZero=getSize();
	int bestCol=0;
	
	int c;
	for (c=0; c<getSize(); c++) {
		int nonZero=0;
		int r;

		for (r=0; r<getSize() && nonZero<minNonZero; r++) {
			if (!isNull(getElement(r,c))) {
				nonZero++;
			}
		}
		
		if (nonZero < minNonZero) {
			minNonZero = nonZero;
			bestCol = c;
		}
	}
	
	col = bestCol;
	zeros = getSize()-minNonZero;
}

template<class Type> 
ostream & operator << (ostream &os, const Matrix<Type> &m)
{
	int size=m.getSize();
	int i;
	int j;
	for (i=0; i<size; i++) {
		os << endl << "Zeile " << i << ":";
		for (j=0; j<size; j++)
			os << m.getElement(i,j) << "    ";
	}
	return os;
}

template<class Type> 
void Matrix<Type>::deleteRowAndColumn (int row,  int col, Matrix<Type> &m) const
{
	BEGIN("deleteRowAndColumn");
	int r;
	int c;
	assert(m.getSize()==getSize()-1);


	for (r=0; r<row; r++) {
		for (c=0; c<col; c++) {
			m.getElement(r,c) = getElement(r,c);
		}
	}
	

	for (r=row+1; r<getSize(); r++) {
		for (c=0; c<col; c++) {
			m.getElement(r-1, c) = getElement(r,c);
		}
	}

	for (r=row+1; r<getSize(); r++) {
		for (c=col+1; c<getSize(); c++) {
			m.getElement(r-1, c-1) = getElement(r,c);
		}
	}
	
	for (r=0; r<row; r++) {
		for (c=col+1; c<getSize(); c++) {
			m.getElement(r, c-1) = getElement(r,c); 
		}
	}
}

template<class Type> 
Type Matrix<Type>::det() const
{
	if (size==1)
		return elems[0];
	else if (size==2)
		return elems[0]*elems[3]-elems[1]*elems[2];
	// return threadedDeterminant();
	Type result;
	setNull(result);
	if (Thread::shouldStop()) {
		return result;
	}
	Matrix m (size-1);
#if 1
	int bestRow;
	int bestRowZeros;
	
	int bestCol=-1;
	int bestColZeros=-1;
	
	getBestRow(bestRow, bestRowZeros);
	getBestColumn(bestCol, bestColZeros);
	
	TRACE(bestRow);
	TRACE(bestRowZeros);
	
	TRACE(bestCol);
	TRACE(bestColZeros);

	if (bestRowZeros > bestColZeros) {
		DMESS("enwickle nach Zeile");
		int col;
		for (col=0; col<getSize(); col++) {
			if (isNull(getElement(bestRow, col)))
				continue;
			deleteRowAndColumn(bestRow, col, m);
			if ((bestRow+col) % 2 == 0)
				result += m.det()*getElement(bestRow, col);
			else
				result -= m.det()*getElement(bestRow, col);
		}
	} else {
		DMESS("entwickle nach Spalte");
		int row;
		for (row=0; row<getSize(); row++) {
			if (isNull(getElement(row, bestCol)))
				continue;
			deleteRowAndColumn(row, bestCol, m);
			if ((row+bestCol) % 2 == 0) 
				result += m.det()*getElement(row, bestCol);
			else
				result -= m.det()*getElement(row, bestCol);
			    
		}
		
	}

	return result;
#else

	int i;
	for (i=0; i<size; i++) {
		// Zeile i loeschen
		int j,k;
		for (j=0; j<i; j++) 
			for (k=0; k<size-1; k++) 
				m.getElement(j,k) = getElement(j,k+1);
		
		for (j=i+1; j<size; j++)
			for (k=0; k<size-1; k++) 
				m.getElement(j-1,k) = getElement(j,k+1);

		if (i % 2 == 0)
			result+=m.det()*getElement(i,0);
		else
			result-=m.det()*getElement(i,0);
	}
	return result;
#endif
}

#endif
