
#ifndef __FLOAT_MATRIX_H__
#define __FLOAT_MATRIX_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <vector>
#include "stdint.h"

namespace caret {

   class ConstFloatMatrixRowRef
   {//needed to do [][] on a const FloatMatrix
      const std::vector<float>& m_row;
      ConstFloatMatrixRowRef();//disallow default construction, this contains a reference
   public:
      ConstFloatMatrixRowRef(ConstFloatMatrixRowRef& right);//copy constructor
      ConstFloatMatrixRowRef(const std::vector<float>& therow);
      const float& operator[](const int64_t& index);//access element
      friend class FloatMatrixRowRef;//so it can check if it points to the same row
   };

   class FloatMatrixRowRef
   {//needed to ensure some joker doesn't call mymatrix[1].resize();, while still allowing mymatrix[1][2] = 5; and mymatrix[1] = mymatrix[2];
      std::vector<float>& m_row;
      FloatMatrixRowRef();//disallow default construction, this contains a reference
   public:
      FloatMatrixRowRef(FloatMatrixRowRef& right);//copy constructor
      FloatMatrixRowRef(std::vector<float>& therow);
      FloatMatrixRowRef& operator=(const FloatMatrixRowRef& right);//NOTE: copy row contents!
      FloatMatrixRowRef& operator=(const ConstFloatMatrixRowRef& right);//NOTE: copy row contents!
      FloatMatrixRowRef& operator=(const float& right);//NOTE: set all row values!
      float& operator[](const int64_t& index);//access element
   };

   ///class for using single precision matrices (insulates other code from the MatrixFunctions templated header)
   ///errors will result in a matrix of size 0x0, or an assertion failure if the underlying vector<vector> isn't rectangular
   class FloatMatrix
   {
      std::vector<std::vector<float> > m_matrix;
      bool checkDimensions() const;//put this inside asserts at the end of functions
   public:
      FloatMatrix() { };//to make the compiler happy
      FloatMatrixRowRef operator[](const int64_t& index);//allow direct indexing to rows
      ConstFloatMatrixRowRef operator[](const int64_t& index) const;//allow direct indexing to rows while const
      FloatMatrix& operator+=(const FloatMatrix& right);//add to
      FloatMatrix& operator-=(const FloatMatrix& right);//subtract from
      FloatMatrix& operator*=(const FloatMatrix& right);//multiply by
      FloatMatrix& operator+=(const float& right);//add scalar to
      FloatMatrix& operator-=(const float& right);//subtract scalar from
      FloatMatrix& operator*=(const float& right);//multiply by scalar
      FloatMatrix& operator/=(const float& right);//divide by scalar
      FloatMatrix operator+(const FloatMatrix& right) const;//add
      FloatMatrix operator-(const FloatMatrix& right) const;//subtract
      FloatMatrix operator-() const;//negate
      FloatMatrix operator*(const FloatMatrix& right) const;//multiply
      bool operator==(const FloatMatrix& right) const;//compare
      bool operator!=(const FloatMatrix& right) const;//anti-compare
      ///construct from a simple vector<vector<float> >
      FloatMatrix(const std::vector<std::vector<float> >& matrixIn);
      ///return the inverse
      FloatMatrix inverse() const;
      ///return the reduced row echelon form
      FloatMatrix reducedRowEchelon() const;
      ///return the transpose
      FloatMatrix transpose() const;
      ///resize the matrix - keeps contents within bounds unless destructive is true (destructive is faster)
      void resize(const int64_t rows, const int64_t cols, const bool destructive = false);
      ///return a matrix of zeros
      static FloatMatrix zeros(const int64_t rows, const int64_t cols);
      ///return square identity matrix
      static FloatMatrix identity(const int64_t rows);
      ///get the range of values from first until one before afterLast, as a new matrix
      FloatMatrix getRange(const int64_t firstRow, const int64_t afterLastRow, const int64_t firstCol, const int64_t afterLastCol) const;
      ///return a matrix formed by concatenating right to the right of this
      FloatMatrix concatHoriz(const FloatMatrix& right) const;
      ///returns a matrix formed by concatenating bottom to the bottom of this
      FloatMatrix concatVert(const FloatMatrix& bottom) const;
      ///get the dimensions
      void getDimensions(int64_t& rows, int64_t& cols) const;
      ///get the matrix as a vector<vector>
      const std::vector<std::vector<float> >& getMatrix() const;
   };

}

#endif //__FLOAT_MATRIX_H__
