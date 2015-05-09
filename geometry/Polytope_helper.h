#ifndef POLYTOPE_HELPER_H
#define POLYTOPE_HELPER_H
#include <Eigen/Dense> // Linear algebra library

using namespace Eigen;

/** Rotates the vector v around the row space of the matrix A by an angle theta
 * A should be an n-2 x n matrix, and v an n x 1 vector. Theta should be in radians
 * Returns an n x 1 vector representing the image of v after rotation
 */
VectorXd RotateAroundRowSpace(MatrixXd A, VectorXd v, double theta);

/** Returns a matrix whose rows form an orthogonal basis for the left null space of A
 */
MatrixXd GetLeftNullSpaceOrtho(MatrixXd A);


#endif // POLYTOPE_HELPER_H
