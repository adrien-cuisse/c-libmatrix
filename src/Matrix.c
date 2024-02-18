
#include "Matrix.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>




struct Matrix
{
	size_t width;
	size_t height;

	double ** cells;
};




/**
 * Let A, a m*n matrix, Cof(A)ij is the (i,j) cofactor, such as Cof(A)ij = Det(Cij),
 * with Cij the (i,j) minor matrix of A
 *
 * No check is performed for out of bounds index, or matrix validity (square, size >= 2)
 *
 * @param this - the matrix to compute cofactor in
 * @param rowIndex - the row of the cofactor
 * @param columIndex - the column of the cofactor
 *
 * @return - the [rowIndex][columnIndex] cofactor, or NO VALUE if allocation failed
 */
static double cofactor(Matrix const * this, size_t rowIndex, size_t columIndex);




static Matrix * create(size_t height, size_t width)
{
	size_t rowIndex;
	Matrix * this;

	if ((width == 0) || (height == 0))
		return NULL;

	this = malloc(sizeof(* this));
	if (this == NULL)
		return NULL;

	this->cells = calloc(height, sizeof(* this->cells));
	if (this->cells == NULL)
	{
		_Matrix->delete(& this);
		return NULL;
	}

	for (rowIndex = 0; rowIndex < height; rowIndex++)
	{
		this->cells[rowIndex] = calloc(width, sizeof(this->cells[rowIndex]));
		if (this->cells[rowIndex] == NULL)
		{
			_Matrix->delete(& this);
			return NULL;
		}
	}

	this->width = width;
	this->height = height;

	return this;
}


static void delete(Matrix ** this)
{
	size_t rowIndex;

	if (this == NULL)
		return;
	if (* this == NULL)
		return;

	for (rowIndex = 0; rowIndex < (* this)->height; rowIndex++)
	{
		free((* this)->cells[rowIndex]);
		(* this)->cells[rowIndex] = NULL;
	}

	free((* this)->cells);
	(* this)->cells = NULL;

	free(* this);
	* this = NULL;
}


static Matrix * identity(size_t size)
{
	Matrix * this = NULL;
	size_t coord;

	if (size == 0)
		return NULL;

	this = _Matrix->create(size, size);
	if (this == NULL)
		return NULL;

	for (coord = 0; coord < size; coord++)
		this->cells[coord][coord] = 1;

	return this;
}


static int isIdentity(Matrix const * const this)
{
	size_t rowIndex, columnIndex;
	double expectedValue;

	if (this == NULL)
		return 0;

	if (this->height != this->width)
		return 0;

	for (rowIndex = 0; rowIndex < this->height; rowIndex++)
	{
		for (columnIndex = 0; columnIndex < this->width; columnIndex++)
		{
			if (rowIndex == columnIndex)
				expectedValue = 1;
			else
				expectedValue = 0;

			if (this->cells[rowIndex][columnIndex] != expectedValue)
				return 0;
		}
	}

	return 1;
}


static Matrix * copy(Matrix const * const this)
{
	Matrix * copy;
	size_t rowIndex;

	if (this == NULL)
		return NULL;

	copy = _Matrix->create(this->height, this->width);
	if (copy == NULL)
		return NULL;

	for (rowIndex = 0; rowIndex < this->height; rowIndex++)
		memcpy(copy->cells[rowIndex], this->cells[rowIndex], this->width * sizeof(this->cells[rowIndex][0]));

	return copy;
}


static Matrix * fromRows(size_t height, size_t width, double const * const rows, ...)
{
	va_list variadic;
	size_t rowIndex;
	double const * row;

	Matrix * this = _Matrix->create(height, width);
	if (this == NULL)
		return NULL;

	row = rows;
	va_start(variadic, rows);
	for (rowIndex = 0; rowIndex < height; rowIndex++)
	{
		memcpy(this->cells[rowIndex], row, width * sizeof(this->cells[rowIndex][0]));
		row = va_arg(variadic, double *);
	}
	va_end(variadic);

	return this;
}


static Matrix * fromColumns(size_t height, size_t width, double const * const columns, ...)
{
	va_list variadic;
	size_t rowIndex, columnIndex;
	double const * column;

	Matrix * this = _Matrix->create(height, width);
	if (this == NULL)
		return NULL;

	column = columns;
	va_start(variadic, columns);
	for (columnIndex = 0; columnIndex < width; columnIndex++)
	{
		for (rowIndex = 0; rowIndex < height; rowIndex++)
		{
			this->cells[rowIndex][columnIndex] = column[rowIndex];
		}
		column = va_arg(variadic, double *);
	}
	va_end(variadic);

	return this;
}


static size_t width(Matrix const * const this)
{
	if (this == NULL)
		return NO_VALUE;
	return this->width;
}


static size_t height(Matrix const * const this)
{
	if (this == NULL)
		return NO_VALUE;
	return this->height;
}


static void print(Matrix const * const this)
{
	size_t rowIndex, columnIndex;
	int isFirstCell;

	for (rowIndex = 0; rowIndex < this->height; rowIndex++)
	{
		isFirstCell = 1;
		for (columnIndex = 0; columnIndex < this->width; columnIndex++)
		{
			if (! isFirstCell)
				printf("\t");
			printf("%.2f", this->cells[rowIndex][columnIndex]);
			isFirstCell = 0;
		}
		printf("\n");
	}
}


static double getCell(Matrix const * const this, size_t ordinate, size_t abscissa)
{
	if (this == NULL)
		return NO_VALUE;

	if ((abscissa >= this->width) || (ordinate >= this->height))
		return NO_VALUE;

	return this->cells[ordinate][abscissa];
}


static double trace(Matrix const * const this)
{
	size_t coord;
	double trace;

	if (this == NULL)
		return NO_VALUE;

	if (this->height != this->width)
		return MATRIX_IS_NOT_SQUARE;

	trace = 0;
	for (coord = 0; coord < this->height; coord++)
		trace += this->cells[coord][coord];

	return trace;
}


static double determinant(Matrix const * const this)
{
	size_t rowIndex, columnIndex;
	Matrix * cofactorsMatrix;
	double determinant = 0;

	if (this->width != this->height)
		return MATRIX_IS_NOT_SQUARE;

	if (this->height == 1)
		return this->cells[0][0];
	if (this->height == 2)
		return this->cells[0][0] * this->cells[1][1] - this->cells[0][1] * this->cells[1][0];

	/* TODO: we don't need the FULL cofactors matrix, only 1 row or column */
	/* TODO: rows permutation for faster processing, instead of Laplace expansion */

	cofactorsMatrix = _Matrix->cofactors(this);
	if (cofactorsMatrix == NULL)
		return NO_VALUE;

	for (rowIndex = 0; rowIndex < this->height; rowIndex++)
	{
		for (columnIndex = 0; columnIndex < cofactorsMatrix->width; columnIndex++)
			determinant += this->cells[rowIndex][columnIndex] * cofactorsMatrix->cells[0][columnIndex];
	}

	_Matrix->delete(& cofactorsMatrix);

	return determinant;
}


static Matrix * minor(Matrix const * const this, size_t rowIndex, size_t columnIndex)
{
	Matrix * minor;
	size_t sourceRowIndex, sourceColumnIndex;
	size_t destRowIndex, destColumnIndex;

	if (this->height < 2)
		return NULL;

	minor = _Matrix->create(this->height - 1, this->width - 1);
	if (minor == NULL)
		return NULL;

	for (sourceRowIndex = destRowIndex = 0; sourceRowIndex < this->height; sourceRowIndex++)
	{
		if (sourceRowIndex == rowIndex)
			continue;

		for (sourceColumnIndex = destColumnIndex = 0; sourceColumnIndex < this->width; sourceColumnIndex++)
		{
			if (sourceColumnIndex == columnIndex)
				continue;

			minor->cells[destRowIndex][destColumnIndex] = this->cells[sourceRowIndex][sourceColumnIndex];
			destColumnIndex++;
		}

		destRowIndex++;
	}

	return minor;
}


static Matrix * cofactors(Matrix const * const this)
{
	size_t rowIndex, columnIndex;
	Matrix * cofactors;

	if (this->height != this->width)
		return NULL;
	if (this->height < 2)
		return NULL;

	cofactors = _Matrix->create(this->height, this->width);
	if (cofactors == NULL)
		return NULL;

	for (rowIndex = 0; rowIndex < this->height; rowIndex++)
	{
		for (columnIndex = 0; columnIndex < this->width; columnIndex++)
			cofactors->cells[rowIndex][columnIndex] = cofactor(this, rowIndex, columnIndex);
	}

	return cofactors;
}


static Matrix * transpose(Matrix const * const this)
{
	Matrix * transpose;
	size_t rowIndex, columnIndex;

	transpose = _Matrix->create(this->width, this->height);
	if (transpose == NULL)
		return NULL;

	for (rowIndex = 0; rowIndex < this->height; rowIndex++)
	{
		for (columnIndex = 0; columnIndex < this->width; columnIndex++)
			transpose->cells[columnIndex][rowIndex] = this->cells[rowIndex][columnIndex];
	}

	return transpose;
}


static Matrix * adjugate(Matrix const * const this)
{
	Matrix * cofactorsMatrix;
	Matrix * adjugate;

	if (this == NULL)
		return NULL;

	cofactorsMatrix = _Matrix->cofactors(this);
	if (cofactorsMatrix == NULL)
		return NULL;

	adjugate = _Matrix->transpose(cofactorsMatrix);

	_Matrix->delete(& cofactorsMatrix);

	return adjugate;
}


static Matrix * sum(Matrix const * const left, Matrix const * const right)
{
	size_t rowIndex, columnIndex;
	Matrix * sum;

	if ((left == NULL) || (right == NULL))
		return NULL;
	if (left->width != right->width)
		return NULL;
	if (left->height != right->height)
		return NULL;

	sum = _Matrix->copy(left);
	if (sum == NULL)
		return NULL;

	for (rowIndex = 0; rowIndex < left->height; rowIndex++)
	{
		for (columnIndex = 0; columnIndex < left->width; columnIndex++)
			sum->cells[rowIndex][columnIndex] += right->cells[rowIndex][columnIndex];
	}

	return sum;
}


static Matrix * product(Matrix const * const left, Matrix const * const right)
{
	Matrix * product;
	/*
	 * A ∈ M(a,b), B ∈ M(b,c) => AB ∈ M(a,c)
	 * Pi,j = ∑_i=1->n Ai,k * Bk,j
	 */
	size_t i, j, k;

	if ((left == NULL) || (right == NULL))
		return NULL;

	if (left->width != right->height)
		return NULL;

	product = _Matrix->create(left->height, right->width);
	if (product == NULL)
		return NULL;

	for (i = 0; i < left->height; i++)
	{
		for (j = 0; j < right->width; j++)
		{
			for (k = 0; k < left->width; k++)
				product->cells[i][j] += left->cells[i][k] * right->cells[k][j];
		}
	}

	return product;
}


static Matrix * scalarProduct(Matrix const * const this, double scalar)
{
	size_t rowIndex, columnIndex;
	Matrix * product;

	if (this == NULL)
		return NULL;

	product = _Matrix->create(this->height, this->width);
	if (product == NULL)
		return NULL;

	for (rowIndex = 0; rowIndex < this->height; rowIndex++)
	{
		for (columnIndex = 0; columnIndex < this->width; columnIndex++)
			product->cells[rowIndex][columnIndex] = scalar * this->cells[rowIndex][columnIndex];
	}

	return product;
}


static int isInvertible(Matrix const * const this)
{
	if (this == NULL)
		return 0;
	if (this->height != this->width)
		return 0;

	return _Matrix->determinant(this) != 0;
}


static Matrix * inverse(Matrix const * const this)
{
	double determinant;
	Matrix * adjugate;
	Matrix * inverse;

	if (this == NULL)
		return NULL;

	if (this->height != this->width)
		return NULL;

	determinant = _Matrix->determinant(this);
	if ((determinant == 0) || (determinant == NO_VALUE))
		return NULL;

	adjugate = _Matrix->adjugate(this);
	if (adjugate == NULL)
		return NULL;

	inverse = _Matrix->scalarProduct(adjugate, 1.0 / determinant);

	_Matrix->delete(& adjugate);

	return inverse;
}




static double cofactor(Matrix const * const this, size_t rowIndex, size_t columnIndex)
{
	Matrix * minor;
	double cofactor;

	minor = _Matrix->minor(this, rowIndex, columnIndex);

	if ((rowIndex + columnIndex) & 1)
		cofactor = -1 * _Matrix->determinant(minor);
	else
		cofactor = _Matrix->determinant(minor);

	_Matrix->delete(& minor);

	return cofactor;
}




static MatrixMethods methods =
{
	create,
	delete,
	identity,
	isIdentity,
	copy,
	fromRows,
	fromColumns,
	width,
	height,
	print,
	getCell,
	trace,
	determinant,
	minor,
	cofactors,
	transpose,
	adjugate,
	sum,
	product,
	scalarProduct,
	isInvertible,
	inverse
};
MatrixMethods const * const _Matrix = & methods;
