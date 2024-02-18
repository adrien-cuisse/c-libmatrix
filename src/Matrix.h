
#ifndef MATRIX_HEADER
#define MATRIX_HEADER

#include <stddef.h>
#include <math.h>

#define NO_VALUE ((double) 0x7ff8000000000000) /* IEEE 754 NaN */
#define MATRIX_IS_NOT_SQUARE ((double) 0xDEADBEEF)




typedef struct Matrix Matrix;


typedef struct
{
	/**
	 * Creates a m*n matrix
	 * @param height - n, the number of rows
	 * @param width - m, the number of columns
	 *
	 * @return - the created matrix, or NULL if:
	 * 		any dimension is 0,
	 * 		any allocation failed
	 */
	Matrix * (* create)(size_t height, size_t width);

	/**
	 * Deletes the matrix, and sets it to NULL
	 *
	 * @param this - pointer to pointer to matrix to delete
	 */
	void (* delete)(Matrix ** this);

	/**
	 * Creates an identity matrix
	 *
	 * @param size - the dimensions of the matrix to create
	 *
	 * @return - the created identity matrix, or NULL if:
	 * 		size is 0,
	 * 		allocation failed
	 */
	Matrix * (* identity)(size_t size);

	/**
	 * Identity matrix is a n*n square matrix, with 1s on its main diagonal,
	 * and 0s everywhere else
	 *
	 * @param this - the matrix to check
	 *
	 * @return - 1 only if [this] is not NULL, is square, and has
	 * 		its main diagonal filled with 1s, returns 0 otherwise
	 */
	int (* isIdentity)(Matrix const * this);

	/**
	 * Copies the input matrix
	 *
	 * @param this - the matrix to copy
	 *
	 * @return - a copy of the matrix, or NULL if:
	 * 		[this] is NULL,
	 * 		allocation failed
	 */
	Matrix * (* copy)(Matrix const * this);

	/**
	 * Creates a matrix from rows, from top to bottom
	 * If not exactly [height] rows are given, or if any row doesn't contain
	 * exactly [width] values, the behavior is undefined
	 *
	 * @param height - the number of rows
	 * @param width - the number of values in each row
	 * @param rows - [width]-sized array of doubles
	 * @param ... - other [width]-sized arrays of doubles, there must be [height]-1 of them after [rows]
	 *
	 * @return - the newly created matrix, or NULL if allocation failed
	 */
	Matrix * (* fromRows)(size_t height, size_t width, double const * rows, ...);

	/**
	 * Creates a matrix from columns, from left to right
	 * If not exactly [width] columns are given, or if any column doesn't contain
	 * exactly [height] values, the behavior is undefined
	 *
	 * @param height - the number of values in each column
	 * @param width - the number of columns
	 * @param columns - [height]-sized array of doubles
	 * @param ... - other [height]-sized arrays of doubles, there must be [width]-1 of them after [columns]
	 *
	 * @return - the newly created matrix, or NULL if allocation failed
	 */
	Matrix * (* fromColumns)(size_t height, size_t width, double const * columns, ...);

	/**
	 * For a m*n matrix, returns n
	 *
	 * @param this - the matrix to get width for
	 *
	 * @return - the width of the matrix, or NO_VALUE if [this] is NULL
	 */
	size_t (* width)(Matrix const * this);

	/**
	 * For a m*n matrix, returns m
	 *
	 * @param this - the matrix to get height for
	 *
	 * @return - the height of the matrix, or NO_VALUE if [this] is NULL
	 */
	size_t (* height)(Matrix const * this);

	void (* print)(Matrix const * this);

	/**
	 * Returns Ai,j
	 *
	 * @param ordinate - the row, in range [0, height[
	 * @param abscissa - the column, in range [0, width[
	 *
	 * @return - the requested cell, or NO_VALUE if:
	 * 		[this] is NULL,
	 * 		out of bounds occurred
	 */
	double (* getCell)(Matrix const * this, size_t ordinate, size_t abscissa);

	/**
	 * Let A, a n*n square matrix, Tr(A) = ∑_i=1->n Ai,j
	 *
	 * @param this - the matrix to get trace for
	 *
	 * @return - the sum of the main diagonal, or NO_VALUE if [this] is NULL,
	 * 		or MATRIX_IS_NOT_SQUARE if [this] is not square
	 */
	double (* trace)(Matrix const * this);

	/**
	 * For a n*n square matrix A, Det(A) = ∑_i=1->n (-1)^(i+1) * Ai1 * Det(Ci))
	 *
	 * Negative determinant means the application flips the space orientation
	 * Absolute value is how output measures (length, surface, volume, etc.) are multiplied
	 * Zero value means the application squishes at least 1 dimension from the input space
	 *
	 * @param this - the matrix to compute determinant for
	 *
	 * @return - the determinant, or MATRIX_IS_NOT_SQUARE is [this] isn't square,
	 * 		or NO_VALUE if minors allocation failed
	 */
	double (* determinant)(Matrix const * this);

	/**
	 * Let A, a m*n matrix, Cij is its (i,j) minor, obtained by removing the i-th row and j-th column
	 *
	 * @param this - the matrix to minor
	 * @param rowIndex - the row to remove
	 * @param columnIndex - the column to remove
	 *
	 * @return - the matrix without the given row and column, or NULL if matrix size is lesser than 2,
	 * 		or if allocation failed
	 */
	Matrix * (* minor)(Matrix const * this, size_t rowIndex, size_t columnIndex);

	/**
	 * Let A, a m*m square matrix, Cof(A) is the cofactors matrix, where Cof(A)ij = (-1)^(i+j) * Det(Cij(A))
	 *
	 * @param this - the matrix to compute cofactors for
	 *
	 * @return - the matrix of cofactors, or NULL if matrix isn't square, or if its size is lesser than 2,
	 * 		or if allocation failed
	 */
	Matrix * (* cofactors)(Matrix const * this);

	/**
	 * Let A, a m*n matrix, ^t A is its transpose matrix, such as Aij = ^t Aji
	 *
	 * @param - the matrix to get transpose from
	 *
	 * @return - the matrix where rows are written in columns, or NULL if allocation failed
	 */
	Matrix * (* transpose)(Matrix const * this);

	/**
	 * For a square matrix, the adjugate matrix is the transpose of its cofactors matrix
	 * Adj(A) = ^t Cof(A)
	 *
	 * @param this - the matrix to get the adjugate for
	 *
	 * @return - the adjugate matrix, or NULL if:
	 * 		[this] is NULL,
	 * 		[this] isn't square,
	 * 		cofactors matrix is not defined,
	 * 		or if allocation failed
	 */
	Matrix * (* adjugate)(Matrix const * this);

	/**
	 * Let A and B, two m*n matrix, S is the m*n matrix such that Si,j = Ai,j + Bi,j
	 *
	 * @param left - the left operand
	 * @param right - the right operand
	 *
	 * @return - the sum of the operands, or NULL if:
	 * 		any operand is NULL,
	 * 		operands don't have the same size,
	 * 		allocation failed
	 */
	Matrix * (* sum)(Matrix const * left, Matrix const * right);

	/**
	 * Let A and B, m*n matrix and n*p respectively, P is the m*p matrix, such that
	 * Pi,j = ∑_k=1->n Ai,k * Bk,j
	 * Product do not commute
	 *
	 * @param left - the left operand
	 * @param right - the right operand
	 *
	 * @return - the product as a new matrix, or NULL if:
	 * 		[left] or [right] is NULL,
	 * 		[left] width != [right] height
	 * 		allocation failed
	 */
	Matrix * (* product)(Matrix const * left, Matrix const * right);

	/**
	 * Multiplies every cell in [this] by [scalar]
	 *
	 * @param this - the matrix to multiply
	 * @param scalar - the factor with which cells in [this] must be multiplied
	 *
	 * @return - a new matrix, where every cell in [this] has been multiplied by [scalar],
	 * 		or NULL if [this] is NULL, or if allocation failed
	 */
	Matrix * (* scalarProduct)(Matrix const * this, double scalar);

	/**
	 * Checks whether or not [this] can be inverted
	 * @see _Matrix->inverse
	 *
	 * @param this - the matrix to check
	 *
	 * @return - 1 if [this] is not NULL, is square and has non-zero determinant,
	 * 		0 otherwise
	 */
	int (* isInvertible)(Matrix const * this);

	/**
	 * Given A, a n*n square matrix, A^(-1) is its inverse matrix, such as A*A^(-1) = A^(-1)*A = Id(n)
	 * Inverse is only defined for square matrix, with non-zero determinant
	 *
	 * @param this - the matrix to invert
	 *
	 * @return - the inverse matrix, or NULL if :
	 * 		[this] is NULL,
	 * 		[this] isn't square,
	 * 		[this] as a determinant equal to 0,
	 * 		some allocation failed
	 */
	Matrix * (* inverse)(Matrix const * this);

} MatrixMethods;




extern MatrixMethods const * const _Matrix;




#endif /* MATRIX_HEADER */
