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

#ifndef MATRIX33_H
#define MATRIX33_H

// very simple 3x3 matrix class with
//   operator= and operator* (matrix multiplication)

class Matrix33 {
public:
	Matrix33 () {
		array = new double[9];
	}
	Matrix33 (double* arr) {
		array = new double[9];
		setArray (arr);
	}
	Matrix33 (const Matrix33 &src) {
		array = new double[9];
		setArray (src.array);
	}

	virtual ~Matrix33 () {
		delete [] array;
	}

	Matrix33& setArray (double* arr) {
		for (int i = 0; i != 9; ++i) {
			array[i] = arr[i];
		}
		return *this;
	}

	Matrix33& operator= (const Matrix33& r) {
		for (int i = 0; i != 9; ++i) {
			array[i] = r.array[i];
		}
		return *this;
	}
	
	Matrix33 operator* (const Matrix33& r) const {
		Matrix33 res;
		for (int i = 0; i != 3; ++i) {
			for (int j = 0; j != 3; ++j) {
				res.array[i*3 + j] = array[i*3]*r.array[j]
					           + array[i*3+1]*r.array[3+j]
					           + array[i*3+2]*r.array[6+j];
			}
		}
		return res;
	}

	double& operator[](const int& i) const {
		return array[i];
	}

private:
	double* array;
};

#endif // !__MATRIX33_H__
