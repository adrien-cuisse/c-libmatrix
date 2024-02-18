
#include "../../src/Matrix.h"

#include <stdio.h>

#include <criterion/criterion.h>


// TODO: add doc for failed allocations

Test(Matrix, create_requires_positive_width)
{
	// given
	size_t width = 0;

	// when
	Matrix * const this = _Matrix->create(1, width);

	// then
	cr_assert_null(this, "Matrix with no width makes no sense");
}


Test(Matrix, create_requires_positive_height)
{
	// given
	size_t height = 0;

	// when
	Matrix * const this = _Matrix->create(height, 1);

	// then
	cr_assert_null(this, "Matrix with no height makes no sense");
}


Test(Matrix, constructor_destructor_memory_management)
{
	// when
	Matrix * this = _Matrix->create(1, 1);

	// then
	cr_assert_not_null(this);

	// when
	_Matrix->delete(& this);

	// then
	cr_assert_null(this);
}


Test(Matrix, create_returns_zero_filled_matrix)
{
	// when
	Matrix * this = _Matrix->create(2, 2);

	// then
	cr_expect_eq(0, _Matrix->getCell(this, 0, 0), "Value at (0,0) is non-zero: %lf", _Matrix->getCell(this, 0, 0));
	cr_expect_eq(0, _Matrix->getCell(this, 0, 1), "Value at (0,1) is non-zero: %lf", _Matrix->getCell(this, 0, 1));
	cr_expect_eq(0, _Matrix->getCell(this, 1, 0), "Value at (1,0) is non-zero: %lf", _Matrix->getCell(this, 1, 0));
	cr_expect_eq(0, _Matrix->getCell(this, 1, 1), "Value at (1,1) is non-zero: %lf", _Matrix->getCell(this, 1, 1));

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, fromRows_stores_given_values)
{
	// given
	size_t width = 3;
	size_t height = 2;
	double rows[][3] = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
	};

	// when
	Matrix * this = _Matrix->fromRows(height, width, rows[0], rows[1]);

	for (size_t rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (size_t columnIndex = 0; columnIndex < width; columnIndex++)
		{
			double actual = _Matrix->getCell(this, rowIndex, columnIndex);
			double expected = rows[rowIndex][columnIndex];
			cr_expect_eq(
				expected,
				actual,
				"Incorrect value at (%lu,%lu), got %lf instead of %lf",
				rowIndex,
				columnIndex,
				actual,
				expected);
		}
	}

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, fromColumns_stores_given_values)
{
	// given
	size_t width = 2;
	size_t height = 3;
	double columns[][3] = {
		{ 1, 2, 3 },
		{ 4, 5, 6 }
	};

	// when
	Matrix * this = _Matrix->fromColumns(height, width, columns[0], columns[1]);

	// then
	for (size_t rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (size_t columnIndex = 0; columnIndex < width; columnIndex++)
		{
			double actual = _Matrix->getCell(this, rowIndex, columnIndex);
			double expected = columns[columnIndex][rowIndex];
			cr_expect_eq(
				expected,
				actual,
				"Incorrect value at (%lu,%lu), got %lf instead of %lf",
				rowIndex,
				columnIndex,
				actual,
				expected);
		}
	}

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, sum_requires_equal_widths)
{
	// given
	Matrix * left = _Matrix->fromRows(2, 3, (double[]) { 0, 0, 0 }, (double[]) { 0, 0, 0 });
	Matrix * right = _Matrix->fromRows(2, 2, (double[]) { 0, 0 }, (double[]) { 0, 0 });

	// when
	Matrix * sum = _Matrix->sum(left, right);

	// then
	cr_expect_null(sum, "Adding matrices should not be possible from different widths");

	// teardown
	_Matrix->delete(& left);
	_Matrix->delete(& right);
}


Test(Matrix, sum_requires_equal_heights)
{
	// given
	Matrix * left = _Matrix->fromRows(3, 2, (double[]) { 0, 0 }, (double[]) { 0, 0 }, (double[]) { 0, 0 });
	Matrix * right = _Matrix->fromRows(2, 2, (double[]) { 0, 0 }, (double[]) { 0, 0 });

	// when
	Matrix * sum = _Matrix->sum(left, right);

	// then
	cr_expect_null(sum, "Adding matrices should not be possible from different heights");

	// teardown
	_Matrix->delete(& left);
	_Matrix->delete(& right);
}


Test(Matrix, sum_returns_new_matrix)
{
	// given
	Matrix * left = _Matrix->fromRows(2, 2, (double[]) { 1, 2 }, (double[]) { 3, 4 });
	Matrix * right = _Matrix->fromRows(2, 2, (double[]) { 5, 6 }, (double[]) { 7, 8 });

	// when
	Matrix * sum = _Matrix->sum(left, right);

	// then
	cr_expect_neq(left, sum, "Left operand was returned instead of a copy");
	cr_expect_neq(right, sum, "Right operand was returned instead of a copy");

	// teardown
	_Matrix->delete(& left);
	_Matrix->delete(& right);
	_Matrix->delete(& sum);
}


Test(Matrix, sum)
{
	// given
	Matrix * left = _Matrix->fromRows(2, 2, (double[]) { 1, 2 }, (double[]) { 3, 4 });
	Matrix * right = _Matrix->fromRows(2, 2, (double[]) { 5, 6 }, (double[]) { 7, 8 });

	// when
	Matrix * sum = _Matrix->sum(left, right);

	// then
	cr_expect_eq(_Matrix->getCell(sum, 0, 0), 6, "Got %lf instead of 6 at (0,0)", _Matrix->getCell(sum, 0, 0));
	cr_expect_eq(_Matrix->getCell(sum, 0, 1), 8, "Got %lf instead of 8 at (0,1)", _Matrix->getCell(sum, 0, 1));
	cr_expect_eq(_Matrix->getCell(sum, 1, 0), 10, "Got %lf instead of 10 at (1,0)", _Matrix->getCell(sum, 1, 0));
	cr_expect_eq(_Matrix->getCell(sum, 1, 1), 12, "Got %lf instead of 12 at (1,1)", _Matrix->getCell(sum, 1, 1));

	// teardown
	_Matrix->delete(& left);
	_Matrix->delete(& right);
	_Matrix->delete(& sum);
}


Test(Matrix, copy_returns_new_instance)
{
	// given
	Matrix * this = _Matrix->fromRows(1, 1, (double[]) { 1 }, (double[]) { 1 });

	// when
	Matrix * copy = _Matrix->copy(this);

	// then
	cr_expect_neq(copy, this, "Copy should return newly allocated memory");

	// teardown
	_Matrix->delete(& this);
	_Matrix->delete(& copy);
}


Test(Matrix, copy)
{
	// given
	size_t height = 2;
	size_t width = 3;
	double rows[2][3] = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
	};
	Matrix * this = _Matrix->fromRows(height, width, rows[0], rows[1]);

	// when
	Matrix * copy = _Matrix->copy(this);

	// then
	for (size_t rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (size_t columnIndex = 0; columnIndex < width; columnIndex++)
		{
			double actual = _Matrix->getCell(copy, rowIndex, columnIndex);
			double expected = _Matrix->getCell(this, rowIndex, columnIndex);
			cr_expect_eq(
				actual, expected,
				"Incorrect value stored at (%lu,%lu), got %lf instead of %lf",
				 rowIndex, columnIndex, actual, expected);
			}
	}

	// teardown
	_Matrix->delete(& this);
	_Matrix->delete(& copy);
}


Test(Matrix, width_error_on_null_matrix)
{
	// given
	Matrix * this = NULL;

	// when
	size_t width = _Matrix->width(this);

	// then
	cr_assert_eq(width, NO_VALUE, "Error code should be returned for invalid matrix' width");
}


Test(Matrix, width)
{
	// given
	Matrix * this = _Matrix->create(1, 2);

	// when
	size_t width = _Matrix->width(this);

	// then
	cr_expect_eq(2, width, "Got %lu instead of 2", width);

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, height_error_on_null_matrix)
{
	// given
	Matrix * this = NULL;

	// when
	size_t height = _Matrix->height(this);

	// then
	cr_assert_eq(height, NO_VALUE, "Error code should be returned for invalid matrix' height");
}


Test(Matrix, height)
{
	// given
	Matrix * this = _Matrix->create(2, 1);

	// when
	size_t height = _Matrix->height(this);

	// then
	cr_expect_eq(2, height, "Got %lu instead of 2", height);

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, identity_requires_positive_size)
{
	// given
	size_t size = 0;

	// when
	Matrix * identity = _Matrix->identity(size);

	// then
	cr_assert_null(identity, "Identity matrix with no size makes no sense");
}


Test(Matrix, identity_returns_allocated_matrix)
{
	// given
	size_t size = 2;

	// when
	Matrix * identity = _Matrix->identity(size);

	// then
	cr_expect_not_null(identity, "Identity matrix was not allocated");

	// teardown
	_Matrix->delete(& identity);
}


Test(Matrix, identity_is_square)
{
	// given
	size_t size = 2;

	// when
	Matrix * identity = _Matrix->identity(size);

	// then
	size_t width = _Matrix->width(identity);
	size_t height = _Matrix->height(identity);

	cr_expect_eq(width, height, "Identity matrix should be squared");

	// teardown
	_Matrix->delete(& identity);
}


Test(Matrix, identity_has_ones_on_main_diagonal)
{
	// given
	size_t size = 3;

	// when
	Matrix * identity = _Matrix->identity(size);

	// then
	for (size_t coord = 0; coord < size; coord++)
	{
		double cell = _Matrix->getCell(identity, coord, coord);
		cr_expect_eq(1, cell, "Cell at (%zu,%zu) is not 1", coord, coord);
	}

	// teardown
	_Matrix->delete(& identity);
}


Test(Matrix, identity_has_zeros_on_not_main_diagonal)
{
	// given
	size_t size = 3;

	// when
	Matrix * identity = _Matrix->identity(size);

	// then
	for (size_t rowIndex = 0; rowIndex < size; rowIndex++)
	{
		for (size_t columnIndex = 0; columnIndex < size; columnIndex++)
		{
			if (rowIndex == columnIndex)
				continue;

			double cell = _Matrix->getCell(identity, rowIndex, columnIndex);
			cr_expect_eq(
				0, cell,
				"Cells not on the main diagonal should be 0, got %lf at (%zu,%zu)",
				cell, rowIndex, columnIndex);
		}
	}

	// teardown
	_Matrix->delete(& identity);
}


Test(Matrix, product_requires_left_width_equal_to_right_height)
{
	// given
	Matrix * left = _Matrix->create(2, 3);
	Matrix * right = _Matrix->create(2, 2);

	// when
	Matrix * product = _Matrix->product(left, right);

	// then
	cr_expect_null(product, "Matrix multiplication should fail on invalid operands size");

	// teardown
	_Matrix->delete(& left);
	_Matrix->delete(& right);
}


Test(Matrix, product_returns_new_matrix)
{
	// given
	Matrix * left = _Matrix->create(2, 2);
	Matrix * right = _Matrix->create(2, 3);

	// when
	Matrix * product = _Matrix->product(left, right);

	// then
	cr_expect_not_null(product, "Matrix multiplication should return newly allocated matrix");

	// teardown
	_Matrix->delete(& left);
	_Matrix->delete(& right);
	_Matrix->delete(& product);
}


Test(Matrix, product)
{
	// given
	Matrix * left = _Matrix->fromRows(
		3, 2,
		(double[]) { 2, 3 },
		(double[]) { 5, 7 },
		(double[]) { 11, 13 });
	Matrix * right = _Matrix->fromRows(
		2, 4,
		(double[]) { 17, 19, 23, 29 },
		(double[]) { 31, 37, 41, 43 });

	// when
	Matrix * product = _Matrix->product(left, right);

	// then
	double expected[][4] = {
		{ 127, 149, 169, 187 },
		{ 302, 354, 402, 446 },
		{ 590, 690, 786, 878 }};
	for (size_t ordinate = 0; ordinate < _Matrix->height(product); ordinate++)
	{
		for (size_t abscissa = 0; abscissa < _Matrix->width(product); abscissa++)
		{
			double actualValue = _Matrix->getCell(product, ordinate, abscissa);
			double expectedValue = expected[ordinate][abscissa];
			cr_expect_eq(
				actualValue,expectedValue,
				"Incorrect cell at (%zu,%zu), got %lf instead of %lf",
				ordinate, abscissa, actualValue, expectedValue);
		}
	}

	// teardown
	_Matrix->delete(& left);
	_Matrix->delete(& right);
	_Matrix->delete(& product);
}


Test(Matrix, trace_requires_square_matrix)
{
	// given
	Matrix * this = _Matrix->create(2, 3);

	// when
	double trace = _Matrix->trace(this);

	// then
	cr_expect_eq(MATRIX_IS_NOT_SQUARE, trace, "Non-square matrix can't have trace");

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, trace)
{
	// given
	double rows[][3] = {
		{    2, -100, -100 },
		{ -100,    3, -100 },
		{ -100, -100,    5 }
	};
	Matrix * this = _Matrix->fromRows(3, 3, rows[0], rows[1], rows[2]);

	// when
	double trace = _Matrix->trace(this);

	// then
	cr_expect_eq(10, trace, "Got %lf instead of 10", trace);

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, determinant_of_non_square_matrix_is_not_defined)
{
	// given
	Matrix * this = _Matrix->fromRows(1, 2, (double[]) { 0, 0 });

	// when
	double determinant = _Matrix->determinant(this);

	// then
	cr_expect_eq(MATRIX_IS_NOT_SQUARE, determinant, "Determinant only exists for square matrix");

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, determinant_for_matrix_size_1_is_its_only_value)
{
	// given
	Matrix * this = _Matrix->fromRows(1, 1, (double[]) { 2 });

	// when
	double determinant = _Matrix->determinant(this);

	// then
	cr_expect_eq(2, determinant, "Got %lf instead of 2", determinant);

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, determinant_for_matrix_size_2_is_diagonal_products_difference)
{
	// given
	double rows[][2] = {
		{ 2, 3, },
		{ 5, 7, },
	};
	Matrix * this = _Matrix->fromRows(2, 2, rows[0], rows[1]);

	// when
	double determinant = _Matrix->determinant(this);

	// then
	cr_expect_eq(
		2 * 7 - 3 * 5, determinant,
		"Got %lf instead of %lf", determinant, 2.0 * 7 - 3 * 5);

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, determinant)
{
	// given
	Matrix * this = _Matrix->fromRows(
		3, 3,
		(double[]) {  2,  3,  5 },
		(double[]) {  7, 11, 13 },
		(double[]) { 17, 19, 23 });

	// when
	double determinant = _Matrix->determinant(this);

	// then
	cr_expect_eq(determinant, -78, "Got %lf instead of -78", determinant);

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, transpose_creates_new_matrix)
{
	// given
	Matrix * this = _Matrix->create(1, 2);

	// when
	Matrix * transpose = _Matrix->transpose(this);

	// then
	cr_expect_neq(transpose, this, "Transpose should create a new matrix");

	// teardown
	_Matrix->delete(& transpose);
	_Matrix->delete(& this);
}


Test(Matrix, transpose_has_inverse_dimensions)
{
	// given
	Matrix * this = _Matrix->create(1, 2);

	// when
	Matrix * transpose = _Matrix->transpose(this);

	// then
	cr_expect_eq(2, _Matrix->height(transpose));
	cr_expect_eq(1, _Matrix->width(transpose));

	// teardown
	_Matrix->delete(& this);
	_Matrix->delete(& transpose);
}


Test(Matrix, transpose)
{
	// given
	Matrix * this = _Matrix->fromRows(
		2, 3,
		(double[]) { 2, 3, 5 },
		(double[]) { 5, 7, 11 } );

	// when
	Matrix * transpose = _Matrix->transpose(this);

	// then
	for (size_t rowIndex = 0; rowIndex < 2; rowIndex++)
	{
		for (size_t columnIndex = 0; columnIndex < 3; columnIndex++)
		{
			double expected = _Matrix->getCell(this, rowIndex, columnIndex);
			double actual = _Matrix->getCell(transpose, columnIndex, rowIndex);
			cr_expect_eq(
				expected, actual,
				"At (%lu,%lu), got %lf instead of %lf",
				columnIndex, rowIndex, actual, expected);
		}
	}

	// teardown
	_Matrix->delete(& this);
	_Matrix->delete(& transpose);
}


Test(Matrix, adjugate_matrix_is_only_defined_for_square_matrix)
{
	// given
	Matrix * this = _Matrix->create(1, 2);

	// when
	Matrix * adjugate = _Matrix->adjugate(this);

	// then
	cr_expect_null(adjugate, "Adjugate matrix is only defined for square matrix");

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, adjugate)
{
	// given
	Matrix * this = _Matrix->fromRows(
		3, 3,
		(double[]) {  2,  3,  5 },
		(double[]) {  7, 11, 13 },
		(double[]) { 17, 19, 23 });

	// when
	Matrix * adjugate = _Matrix->adjugate(this);

	// then
	double expected[][3] = {
		{   6,  26, -16 },
		{  60, -39,   9 },
		{ -54,  13,   1 },
	};
	for (size_t rowIndex = 0; rowIndex < 3; rowIndex++)
	{
		for (size_t columnIndex = 0; columnIndex < 3; columnIndex++)
		{
			double actual = _Matrix->getCell(adjugate, rowIndex, columnIndex);
			cr_expect_eq(
				expected[rowIndex][columnIndex],
				actual,
				"At (%lu,%lu), got %lf instead of %lf",
				rowIndex, columnIndex, actual, expected[rowIndex][columnIndex]);
		}
	}

	// teardown
	_Matrix->delete(& adjugate);
	_Matrix->delete(& this);
}


Test(Matrix, cofactors_matrix_is_only_defined_for_square_matrix)
{
	// given
	Matrix * this = _Matrix->create(2, 3);

	// when
	Matrix * cofactors = _Matrix->cofactors(this);

	// then
	cr_expect_null(cofactors, "Cofactors matrix is only defined for square matrix");

	_Matrix->delete(& this);
}


Test(Matrix, cofactors_matrix_is_only_defined_for_matrix_size_greater_than_1)
{
	// given
	Matrix * this = _Matrix->create(1, 1);

	// when
	Matrix * cofactors = _Matrix->cofactors(this);

	// then
	cr_expect_null(cofactors, "Cofactors matrix is only defined for square matrix size greater than 1");

	_Matrix->delete(& this);
}


Test(Matrix, scalarProduct)
{
	// given
	double rows[][2] = {
		{ 1, 2 },
		{ 4, 5 },
	};
	Matrix * this = _Matrix->fromRows(2, 2, rows[0], rows[1]);

	// when
	double scalar = 3;
	Matrix * product = _Matrix->scalarProduct(this, scalar);

	// then
	for (size_t rowIndex = 0; rowIndex < 2; rowIndex++)
	{
		for (size_t columnIndex = 0; columnIndex < 2; columnIndex++)
		{
			cr_expect_eq(
				scalar * rows[rowIndex][columnIndex],
				_Matrix->getCell(product, rowIndex, columnIndex),
				"At (%lu,%lu), got %lf instead of %lf",
				rowIndex, columnIndex,
				_Matrix->getCell(product, rowIndex, columnIndex), scalar * rows[rowIndex][columnIndex]);
		}
	}
}


Test(Matrix, isInvertible_false_if_not_square)
{
	// given
	Matrix * this = _Matrix->create(1, 2);

	// when
	int invertible = _Matrix->isInvertible(this);

	// then
	cr_expect_not(invertible, "Matrix can only be inverted if it's square");

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, isInvertible_if_determinant_is_not_zero)
{
	// given
	double rows[][2] = {
		{ 0, 0 },
		{ 0, 0 },
	};
	Matrix * this = _Matrix->fromRows(2, 2, rows[0], rows[1]);

	// when
	int invertible = _Matrix->isInvertible(this);

	// then
	cr_expect_not(invertible, "Matrix can only be inverted if it's square");

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, inverse_NULL_if_not_invertible)
{
	// given
	double rows[2][2] = {
		{ 0, 0 },
		{ 0, 0 },
	};
	Matrix * this = _Matrix->fromRows(2, 2, rows[0], rows[1]);

	// when
	Matrix * inverse = _Matrix->inverse(this);

	// then
	cr_expect_null(inverse, "Matrix can only be inverted if it's square");

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, inverse_multiplied_by_matrix_gives_identity)
{
	// given
	double rows[2][2] = {
		{ 2, 3 },
		{ 5, 7 },
	};
	Matrix * this = _Matrix->fromRows(2, 2, rows[0], rows[1]);

	// when
	Matrix * inverse = _Matrix->inverse(this);

	// then
	Matrix * product = _Matrix->product(this, inverse);
	cr_expect(
		_Matrix->isIdentity(product),
		"Matrix multiplied by its inverse should be equal to the identity");

	// teardown
	_Matrix->delete(& this);
	_Matrix->delete(& product);
}


Test(Matrix, inverse)
{
	// given
	double rows[2][2] = {
			{ 2, 3 },
			{ 5, 7 },
	};
	Matrix * this = _Matrix->fromRows(2, 2, rows[0], rows[1]);

	// when
	Matrix * inverse = _Matrix->inverse(this);

	// then
	Matrix * product = _Matrix->product(inverse, this);
	cr_expect(
		_Matrix->isIdentity(product),
		"Matrix and its inverse do not commute");

	// teardown
	_Matrix->delete(& this);
	_Matrix->delete(& product);
}

Test(Matrix, isIdentity_false_is_not_square)
{
	// given
	Matrix * this = _Matrix->create(2, 3);

	// when
	int isIdentity = _Matrix->isIdentity(this);

	// then
	cr_expect_not(isIdentity, "Identity matrix is square");

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, isIdentity_false_if_non_zero_on_not_main_diagonal)
{
	// given
	Matrix * this = _Matrix->fromRows(
		2, 3,
		(double[]) { 1, 0, 0 },
		(double[]) { 0, 1, 0 },
		(double[]) { 1, 0, 1 });

	// when
	int isIdentity = _Matrix->isIdentity(this);

	// then
	cr_expect_not(isIdentity, "Identity matrix has only values in main diagonal");

	// teardown
	_Matrix->delete(& this);
}


Test(Matrix, isIdentity_false_if_main_diagonal_has_any_not_1)
{
	// given
	Matrix * this = _Matrix->fromRows(
			2, 3,
			(double[]) { 1, 0, 0 },
			(double[]) { 0, 2, 0 },
			(double[]) { 0, 0, 1 });

	// when
	int isIdentity = _Matrix->isIdentity(this);

	// then
	cr_expect_not(isIdentity, "Identity matrix has only 1s in main diagonal");

	// teardown
	_Matrix->delete(& this);
}
