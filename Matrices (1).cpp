#include "Matrices.h"
#include <cmath>

namespace Matrices
{
    // constructor for Matrix class
    ///Construct a matrix of the specified size.
    ///Initialize each element to 0.
    Matrix::Matrix(int _rows, int _cols) : rows(_rows), cols(_cols)
    {
        // resize the vector with specified amount
        a.resize(_rows);

        // loop through each row to initialize each element with 0
        for (int i = 0; i < _rows; i++)
        {
            // resize each row in vector a to have amount of cols wiht _cols
            a[i].resize(_cols);
            // assign each element value of 0
            for (int j = 0; j < _cols; j++)
            {
                a[i][j] = 0;
            }
        }
    }


    // Constructor for RotationMatrix class
    ///2D rotation matrix
    ///usage:  A = R * A rotates A theta radians counter-clockwise
    ///Call the parent constructor to create a 2x2 matrix
    ///Then assign each element as follows:
    /*
    cos(theta)  -sin(theta)
    sin(theta)   cos(theta)
    */
    ///theta represents the angle of rotation in radians, counter-clockwise
    RotationMatrix::RotationMatrix(double theta) : Matrix(2, 2)
    {
        // compute
        double cosTheta = cos(theta);
        double sinTheta = sin(theta);

        // assign the above computations to *this indexes
        (*this)(0, 0) = cosTheta; // cos(theta)
        (*this)(0, 1) = -sinTheta; // -sin(theta)
        (*this)(1, 0) = sinTheta; // sin(theta)
        (*this)(1, 1) = cosTheta; // cos(theta)
    }


    // constructor for ScalingMatrix class
    ///2D scaling matrix
    ///usage:  A = S * A expands or contracts A by the specified scaling factor
    ///Call the parent constructor to create a 2x2 matrix
    ///Then assign each element as follows:
    /*
    scale   0
    0       scale
    */
    ///scale represents the size multiplier
    ScalingMatrix::ScalingMatrix(double scale) : Matrix(2, 2)
    {
        // assign either scale parameter or 0 to *this indexes
        (*this)(0, 0) = scale;
        (*this)(0, 1) = 0;
        (*this)(1, 0) = 0;
        (*this)(1, 1) = scale;
    }


    // Constructor for TranslationMatrix class
    ///2D Translation matrix
    ///usage:  A = T + A will shift all coordinates of A by (xShift, yShift)
    ///Call the parent constructor to create a 2xn matrix
    ///Then assign each element as follows:
    /*
    xShift  xShift  xShift  ...
    yShift  yShift  yShift  ...
    */
    ///paramaters are xShift, yShift, and nCols
    ///nCols represents the number of columns in the matrix
    ///where each column contains one (x,y) coordinate pair
    TranslationMatrix::TranslationMatrix(double xShift, double yShift, int nCols) : Matrix(2, nCols)
    {
        // iterate over each column in the matrix
        for (int i = 0; i < nCols; i++)
        {
            // assign the xShift value to the first row (x-coordinate)
            (*this)(0, i) = xShift;
            // assign the yShift value to the second row (y-coordinate)
            (*this)(1, i) = yShift;
        }
    }

    // operator overloads for Matrix class (use the ones from Matrix project)
    ///Add each corresponding element.
    ///usage:  c = a + b;
    Matrix operator+(const Matrix& a, const Matrix& b)
    {
        if (a.getRows() != b.getRows() || a.getCols() != b.getCols())
        {
            throw runtime_error("Error: dimensions must agree");
        }

        Matrix result(a.getRows(), a.getCols());

        for (int i = 0; i < a.getRows(); i++)
        {
            for (int j = 0; j < a.getCols(); j++)
            {
                result(i, j) = a(i, j) + b(i, j);
            }
        }
        return result;
    }

    ///Matrix multiply.  See description.
    ///usage:  c = a * b;
    Matrix operator*(const Matrix& a, const Matrix& b)
    {
        // check if a cols matches with b rows
        if (a.getCols() != b.getRows())
        {
            throw runtime_error("Error: dimensions must agree");
        }
        // create a matrix c obj to hold c = a * b
        Matrix c(a.getRows(), b.getCols());

        // triple for loop
        for (int k = 0; k < b.getCols(); k++)          // k, the column index for b
        {
            for (int i = 0; i < a.getRows(); i++)      // i, the row for a
            {
                double running_sum = 0;
                for (int j = 0; j < a.getCols(); j++)  // j, determine col to use from a & row from b. 
                {
                    running_sum += a(i, j) * b(j, k);
                }
                c(i, k) = running_sum;
            }
        }
        return c;
    }





    ///Matrix comparison.  See description.
    ///usage:  a == b
    bool operator==(const Matrix& a, const Matrix& b)
    {
        if (a.getRows() != b.getRows() || a.getCols() != b.getCols())
        {
            return false;
        }
        for (int i = 0; i < a.getRows(); i++)
        {
            for (int j = 0; j < a.getCols(); j++)
            {
                if (abs(a(i, j) - b(i, j)) < 0.001)
                {
                    return false;
                }
            }
        }
        return true;
    }

    ///Matrix comparison.  See description.
    ///usage:  a != b
    bool operator!=(const Matrix& a, const Matrix& b)
    {
        return !(a == b);
    }

    ///Output matrix.
    ///Separate columns by ' ' and rows by '\n'
    ostream& operator<<(ostream& os, const Matrix& a)
    {
        // for rows in a
        for (int i = 0; i < a.getRows(); i++)
        { // for cols in a
            for (int j = 0; j < a.getCols(); j++)
            { // output i and j in a precise and fixed manner
                os << setw(10) << a(i, j);
                // separate cols with " "
                if (j < a.getCols() - 1)
                {
                    os << " ";
                }
            }
            // separate rows with "\n"
            os << "\n";
        }
        return os;
    }
}