/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */

#ifndef MATRIX_H
#define MATRIX_H

//! A generic matrix class.
/*!
  Matrix template class. Provides basic matrix arithmetic, including
  - matrix multiplication,
  - calculation of the determinant (using LU decomposition),
  - matrix inversion,
  - calculation of minors (removing one row and one column)

  Elements can be of any type that supports =, +, -, * +=, -=, *=.
*/

#include <Vec3.h>

#include <GL/gl.h>
#include<vector>
  
template<class T>
class Matrix {
public:
	//! Copy constructor.
	Matrix(const Matrix<T>& src) : array(0) {
		setDim(src.rows, src.cols);
		elems = src.elems;
	}
	
	//! Constructor for square matrices.
	/*!
	  \param n number of both rows and columns
	  \return the allocated (n x n) matrix
	*/
	Matrix(size_t n) : array(0) { setDim(n); }

	//! Constructor for general matrices.
	/*!
	  \param n number of rows
	  \param m number of columns
	  \return the allocated (n x m) matrix
	*/
	Matrix(size_t n, size_t m) : array(0) { setDim(n, m); }

	//! Destructor.
	virtual ~Matrix() { }


	//! Set square dimensions.
	/*!
	  Resets the number of rows and columns.
	  \param n new number of rows & columns
	*/
	void setDim(size_t n) {
		rows = cols = n;
		elems.resize(n*n);
	}

	//! Set dimensions.
	/*!
	  Resets the number of rows and the number of columns.

	  \param n new number of rows
	  \param m new number of columns
	*/
	void setDim(size_t n, size_t m) {
		rows = n;
		cols = m;
		elems.resize(n*m);
	}

	//! Get number of rows.
	/*!
	  \return number of rows
	*/
	size_t getRowDim() const { return rows; }

	//! Get number of columns.
	/*!
	  \return number of columns
	*/
	size_t getColDim() const { return cols; }

	//! Check if matrix is square.
	/*!
	  \return true if matrix is square, false otherwise
	*/
	bool isSquare() const { return rows == cols; }


	//! Set matrix to identity matrix
	void setToIdentity();

	//! Set matrix to rotation matrix
	/*!
	  \param rotation angle
	  \param rotation axis
	*/
	void setToRotation(T angle, const Vec3<T>& axis);


	//! Direct element access by value.
	/*!
	  Caution: There's no range-checking.
	  
	  \param ij index of element in an one-dimensional C-style array
	  \return element at position ij by value
	*/
	T operator[](size_t ij) const { return elems[ij]; }

	//! Direct element access by reference.
	/*!
	  Caution: There's no range-checking.
	  
	  \param ij index of element in an one-dimensional C-style array
	  \return element at position ij by reference
	*/
	  
	T& operator[](size_t ij) { return elems[ij]; }

	//! Get C-like array w/matrix elements
	/*!
	  This can be useful to get matrices for use w/OpenGL etc.
	*/
	const T* getArray();

	//! Free array allocated w/getArray()
	void freeArray();

	//! Assign one matrix to another.
	/*!
	  \param src source matrix
	  \return this matrix by reference
	*/
	Matrix<T>& operator=(const Matrix<T>& src);

	//! Fill-in array of values:
	Matrix<T>& operator=(const T* src);

	Matrix<T> operator+(const Matrix<T>& rhs) const;
	Matrix<T>& operator+=(const Matrix<T>& rhs);
	Matrix<T> operator-(const Matrix<T>& rhs) const;
	Matrix<T>& operator-=(const Matrix<T>& rhs);
	Matrix<T> operator*(const Matrix<T>& rhs) const;
	Matrix<T>& operator*=(const Matrix<T>& rhs);
	Matrix<T> operator*(T lamda) const;
	template<class T2>
	friend Matrix<T2> operator*(T2 lambda, const Matrix<T2>& mat);
	Matrix<T>& operator*=(T lambda);

	//! Calculate determinant.
	T det() const;

	//! Calculate minor.
	/*!
	  Discard row r and column c.
	  
	  \param r row to remove
	  \param c column to remove
	  \return minor matrix by value
	 */
	Matrix<T> deleteRowCol(size_t r, size_t c) const;

private:
	size_t rows;
	size_t cols;
	
	std::vector<T> elems;
	
	T* array;
	size_t arraySize;
};

typedef Matrix<GLfloat> GLmatrix;

// implementation begins here:
// ------------------------------------------------------------------------------

#ifndef HAVE_LIBSTLPORT
#  include<algo.h>
#endif

#ifndef MAX
#  define MAX(a, b) ( ((a) > (b)) ? (a) : (b) )
#endif

template<class T>
void Matrix<T>::setToIdentity()
{
	for(size_t i = 0; i != rows; i++) {
		for(size_t j = 0; j != cols; j++) {
			if(i == j) {
				elems[i*cols + j] = 1;
			} else {
				elems[i*cols + j] = 0;
			}
		}
	}
}

template<class T>
void Matrix<T>::setToRotation(T angle, const Vec3<T>& axis)
{
	T s = std::sin(angle);
	T c = std::cos(angle);
	T t = 1 - c;
	Vec3<T> unit = axis.unit();
	T x = unit.x;
	T y = unit.y;
	T z = unit.z;

	elems[0 + 0] = t*x*x + c;
	elems[0 + 1] = t*x*y + s*z;
	elems[0 + 2] = t*x*z - s*y;
	elems[0 + 3] = 0;
	elems[4 + 0] = t*x*y - s*z;
	elems[4 + 1] = t*y*y + c;
	elems[4 + 2] = t*y*z + s*x;
	elems[4 + 3] = 0;
	elems[8 + 0] = t*x*z + s*y;
	elems[8 + 1] = t*y*z - s*x;
	elems[8 + 2] = t*z*z + c;
	elems[8 + 3] = 0;
	elems[12+ 0] = 0;
	elems[12+ 1] = 0;
	elems[12+ 2] = 0;
	elems[12+ 3] = 1;
}

template<class T>
const T* Matrix<T>::getArray()
{
	size_t newSize = rows*cols;
	
	if(array && arraySize != newSize) {
		freeArray();
	}

	array = new T[newSize];
	arraySize = newSize;
	for(size_t i = 0; i != newSize; ++i) {
		array[i] = elems[i];
	}
	
	return array;
}

template<class T>
void Matrix<T>::freeArray()
{
	if(array) {
		delete [] array;
		array = 0;
	}
}

template<class T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& src)
{
	if(rows != src.rows || cols != src.cols) {
		setDim(src.rows, src.cols);
	}
	
	elems = src.elems;
	return *this;
}

template<class T>
Matrix<T>& Matrix<T>::operator=(const T* src)
{
	for(size_t i = 0; i != rows*cols; i++) {
		elems[i] = src[i];
	}
	return *this;
}

template<class T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& rhs) const
{
	Matrix<T> result(rows, cols);
	result.elems = elems;
	size_t numElems = rows*cols;
	for(size_t i = 0; i != numElems; ++i) {
		result.elems[i] += rhs.elems[i];
	}
	return result;
}

template<class T>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& rhs)
{
	size_t numElems = rows*cols;
	for(size_t i = 0; i != numElems; ++i) {
		elems[i] += rhs.elems[i];
	}
	return *this;
}

template<class T>
Matrix<T> Matrix<T>::operator-(const Matrix<T>& rhs) const
{
	Matrix<T> result(rows, cols);
	result.elems = elems;
	size_t numElems = rows*cols;
	for(size_t i = 0; i != numElems; ++i) {
		result.elems[i] -= rhs.elems[i];
	}
	return result;
}

template<class T>
Matrix<T>& Matrix<T>::operator-=(const Matrix<T>& rhs)
{
	size_t numElems = rows*cols;
	for(size_t i = 0; i != numElems; ++i) {
		elems[i] -= rhs.elems[i];
	}
	return *this;
}

template<class T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& rhs) const
{
	Matrix<T> result(rows, rhs.cols);
	for(size_t i = 0; i != rows; ++i) {
		for(size_t k = 0; k != rhs.cols; ++k) {
			T& elem = result.elems[i*rhs.cols + k];
			elem = 0;
			for(size_t j = 0; j != cols; ++j) {
				elem += elems[i*cols + j]
					* rhs.elems[j*rhs.cols + k];
			}
		}
	}
	return result;
}

template<class T>
Matrix<T>& Matrix<T>::operator*=(const Matrix<T>& rhs)
{
	*this = *this*rhs;
	return *this;
}

template<class T>
Matrix<T> Matrix<T>::operator*(T lamda) const
{
	Matrix<T> result(rows, cols);
	size_t numElems = rows*cols;
	for(size_t i = 0; i != numElems; ++i) {
		result.elems[i] = elems[i]*lamda;
	}
	return result;
}

template<class T>
Matrix<T> operator*(T lambda, const Matrix<T>& mat)
{
	Matrix<T> result(mat.rows, mat.cols);
	size_t numElems = mat.rows*mat.cols;
	for(size_t i = 0; i != numElems; ++i) {
		result.elems[i] = lambda*mat.elems[i];
	}
	return result;
}

template<class T>
Matrix<T>& Matrix<T>::operator*=(T lambda)
{
	size_t numElems = rows*cols;
	for(size_t i = 0; i != numElems; ++i) {
		elems[i] *= lambda;
	}
	return *this;
}

template<class T>
T Matrix<T>::det() const
{
	switch(rows) {
	case 1:
		return elems[0];
		break;
	case 2:
		return elems[0]*elems[3] - elems[1]*elems[2];
		break;
	default: {
		Matrix<T> a = *this; // we work w/a copy of the matrix

		T* vv = new T[rows]; // vv stores the implicit scaling of each row
	
		T d = 1; // no row interchanges yet
	
		for(size_t i = 1; i <= rows; ++i) { // loop over rows to get the implicit scaling information
			T big = 0.0;
			for(size_t j = 1; j <= cols; ++j) {
				big = MAX(std::fabs(a.at(i, j)), big);
			}

			assert(big != 0); // no nonzero largest element
		
			vv[i - 1] = T(1)/big; // save the scaling
		}
	
		for(size_t j = 1; j <= cols; ++j) { // this is the loop over columns of Crout's method
			for(size_t i = 1; i < j; ++i) {
				T sum = a.at(i, j);
				for(size_t k = 1; k < i; ++k) {
					sum -= a.at(i, k)*a.at(k, j);
				}
				a.at(i, j) = sum;
			}
		
			size_t imax = 0;
			T big = 0; // init for the search for largest pivot element

			for(size_t i = j; i <= rows; ++i) {
				T sum = a.at(i, j);
				for(size_t k = 1; k < j; ++k) {
					sum -= a.at(i, k)*a.at(k, j);
				}
				a.at(i, j) = sum;
				
				T dum = vv[i - 1]*std::fabs(sum);
				if(dum >= big) { // is the figure of merit for the pivot better than the best so far?
					big = dum;
					imax = i;
				}
			}
		
			if(j != imax) { // do we need to interchange rows?
				for(size_t k = 1; k <= rows; ++k) { // yes, do so...
					std::swap(a.at(imax, k), a.at(j, k));
				}
				d = -d; // change parity of d
				vv[imax - 1] = vv[j - 1]; // also interchange the scale factor
			}
			if(a.at(j, j) == T(0)) {
				a.at(j, j) = 0.000001;
				// If the pivot element is zero the matrix is singular
				// (at least to the precision of the algorithm).
				// For some applications on singular matrices, it is
				// desirable to substitue 0.000001 for zero.
			}
			if(j != rows) { // now, finally, divide by the pivot element.
				T dum = T(1)/a.at(j, j);
				for(size_t i = j + 1; i <= rows; ++i) {
					a.at(i, j) *= dum;
				}
			}
		} // go back to the next column in reduction.

		// Determinant is product of diagonal elements (d holds the parity):
		for(size_t i = 1; i <= rows; ++i) {
			d *= a.at(i, i);
		}
		return d;
	}
	}
}

template<class T>
Matrix<T> Matrix<T>::deleteRowCol(size_t r, size_t c) const
{
	Matrix<T> result(rows - 1, cols - 1);
	int r_cntr = -1;
	for(size_t i = 0; i != rows; ++i) {
		int c_cntr = -1;
		if(i == r) {
			break; // skip row
		}
		++r_cntr;
		for(size_t j = 0; j != cols; ++j) {
			if(j == c) {
				break; // skip column
			}
			++c_cntr;
			result.elems[r_cntr*(cols - 1) + c_cntr]
				= elems[i*cols + j];
		}
	}
	return result;
}

#endif // !MATRIX_H
