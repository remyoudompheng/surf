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


#ifndef MATRIX_H
#define MATRIX_H

#include <assert.h>

/**
 * template square matrix class
 *
 * Features:
 * - range checking
 * - computing determinant
 * - get (n-1)x(n-1)-sub-matrix by deleting one row and one column
 * - get "best" row and column (the one containing most 0's)
 */
template<class Type> 
class Matrix
{
private:
	Matrix(const Matrix&);
	void operator=(const Matrix&);

public:
        /**
         * @param size: size of new matrix (always square matrices)
         */
	Matrix(int newSize) : size(newSize), elems(new Type[newSize*newSize]) {}
	~Matrix() { delete [] elems; }

	/**
         * Get matrix element w/range check (non-const method)
         */
	Type& getElement(int row, int column) {
		return elems[getIndex(row,column)];
	}

	/**
         * Get matrix element w/range check (const method)
         */
	const Type& getElement(int row, int column) const {
		return elems[getIndex(row, column)];
	}

	/**
         * computer determinant
         */
	Type det() const;
	
	/**
         * @return size of matrix
         */
	int getSize() const { return size; }
	
protected:
	int size;
	Type* elems;

	/**
         * compute array index from row/column-pair (w/range check)
         */
	int getIndex(int row, int col) const {
		assert(row >= 0 && col >= 0 && row < size && col < size);
		return row*size + col;
	}

	/**
         * @return (n-1)x(n-1)-sub-matrix with one row and col deleted
         */
	void deleteRowAndColumn(int row, int col, Matrix<Type>& m) const;

	/**
         * @return index of row with most 0's (and # of 0's)
         */
	void getBestRow(int& row, int& zeros) const;

	/**
         * @return index of column with most 0's (and # of 0's)
         */
	void getBestColumn (int& col, int& zeros) const;
};

template<class Type>
void Matrix<Type>::getBestRow(int& row, int& zeros) const
{
	int minNonZero = getSize();
	int bestRow = 0;
	
	for(int r = 0; r < getSize(); r++) {
		int nonZero=0;
		
		for(int c = 0; c < getSize() && nonZero < minNonZero; c++) {
			if(!isNull(getElement(r, c))) {
				nonZero++;
			}
		}
		
		if(nonZero < minNonZero) {
			minNonZero = nonZero;
			bestRow = r;
		}
	}
	
	row = bestRow;
	zeros = getSize() - minNonZero;
}

template<class Type>
void Matrix<Type>::getBestColumn(int& col, int& zeros) const
{
	int minNonZero = getSize();
	int bestCol = 0;
	
	for(int c = 0; c < getSize(); c++) {
		int nonZero=0;

		for(int r = 0; r < getSize() && nonZero < minNonZero; r++) {
			if(!isNull(getElement(r,c))) {
				nonZero++;
			}
		}
		
		if (nonZero < minNonZero) {
			minNonZero = nonZero;
			bestCol = c;
		}
	}
	
	col = bestCol;
	zeros = getSize() - minNonZero;
}

template<class Type> 
void Matrix<Type>::deleteRowAndColumn(int row, int col, Matrix<Type>& m) const
{
	BEGIN("deleteRowAndColumn");
	assert(m.getSize() == getSize() - 1);

	for(int r = 0; r < row; r++) {
		for(int c = 0; c < col; c++) {
			m.getElement(r, c) = getElement(r, c);
		}
	}
	
	for(int r = row + 1; r < getSize(); r++) {
		for(int c = 0; c < col; c++) {
			m.getElement(r - 1, c) = getElement(r, c);
		}
	}

	for(int r = row + 1; r < getSize(); r++) {
		for(int c = col + 1; c < getSize(); c++) {
			m.getElement(r - 1, c - 1) = getElement(r, c);
		}
	}
	
	for(int r = 0; r < row; r++) {
		for(int c = col + 1; c < getSize(); c++) {
			m.getElement(r, c - 1) = getElement(r, c); 
		}
	}
}

template<class Type> 
Type Matrix<Type>::det() const
{
	if(size == 1) {
		return elems[0];
	} else if(size == 2) {
		return elems[0]*elems[3] - elems[1]*elems[2];
	}

	Type result;
	setNull(result);

	Matrix m(size - 1);

	int bestRow;
	int bestRowZeros;
	
	int bestCol=-1;
	int bestColZeros=-1;
	
	getBestRow(bestRow, bestRowZeros);
	getBestColumn(bestCol, bestColZeros);
	
	if(bestRowZeros > bestColZeros) {
		for(int col = 0; col < getSize(); col++) {
			if(isNull(getElement(bestRow, col))) {
				continue;
			}
			deleteRowAndColumn(bestRow, col, m);
			if((bestRow + col) % 2 == 0) {
				result += m.det()*getElement(bestRow, col);
			} else {
				result -= m.det()*getElement(bestRow, col);
			}
		}
	} else {
		for(int row = 0; row < getSize(); row++) {
			if(isNull(getElement(row, bestCol))) {
				continue;
			}
			deleteRowAndColumn(row, bestCol, m);
			if((row + bestCol) % 2 == 0) {
				result += m.det()*getElement(row, bestCol);
			} else {
				result -= m.det()*getElement(row, bestCol);
			}
			    
		}
		
	}

	return result;
}

#endif // !MATRIX_H
