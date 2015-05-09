#include "Polytope_helper.h"
#include <Eigen/QR>
#include <math.h>

/** Rotates the vector v around the row space of the matrix A by an angle theta
 * A should be an n-2 x n matrix, and v an n x 1 vector. Theta should be in radians
 * Returns an n x 1 vector representing the image of v after rotation
 */
VectorXd RotateAroundRowSpace(MatrixXd A, VectorXd v, double theta) {
    MatrixXd L = GetLeftNullSpaceOrtho(A);
    MatrixXd B(A.rows()+L.rows(), A.cols()); // Matrix to change basis
    B << L, A; // Concatenate A and L vertically, with L on the top
    MatrixXd Binv = B.inverse(); // Will change basis back

    MatrixXd R; // Compute the rotation matrix
    R.setIdentity();
    R(0,0) = cos(theta);
    R(0,1) = -sin(theta);
    R(1,0) = sin(theta);
    R(1,1) = cos(theta);
    MatrixXd M = Binv * R * B; // Overall rotation: Change basis, rotate easily, change back
    return M * v;
}

/** Returns a matrix whose rows form an orthogonal basis for the left null space of A
 */
MatrixXd GetLeftNullSpaceOrtho(MatrixXd A) {
    MatrixXd Q, R, L;
    Eigen::HouseholderQR<MatrixXd> qr(A);
    Q = qr.householderQ(); // Perform  a QR decomposition of A, storing results in Q and R
    R = qr.matrixQR().triangularView<Upper>();
    unsigned int r = 2; // Rank of A; should be 2 for anything we use this for
    L = Q.block(0, Q.cols() - r - 1, Q.rows(), r); // Remove first r cols of Q: the remaining ones are an orthogonal basis for N_L(A)
    Q.transposeInPlace(); // Transpose Q, so the cols are rows to satisfy the return requirement
    return Q;
}
