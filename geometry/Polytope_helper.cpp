#include "Polytope_helper.h"
#include <Eigen/QR>
#include <math.h>
#include <iostream>
/** Rotates the vector v around the row space of the matrix A by an angle theta
 * A should be an n-2 x n matrix, and v an n x 1 vector. Theta should be in radians
 * Returns an n x 1 vector representing the image of v after rotation
 */
VectorXd RotateAroundRowSpace(MatrixXd A, VectorXd v, double theta) {
    for(int iii = 0; iii < A.rows(); iii++) { // Normalize each row
        A.row(iii) *= 1/sqrt(A.row(iii)*A.row(iii).transpose()); // Divide by magnitude
    }
    MatrixXd L = GetLeftNullSpaceOrtho(A);
    std::cout << "L: \n" << L << "\n";
    std::cout << "A: \n" << A << "\n";
    MatrixXd B(A.rows()+L.rows(), A.cols()); // Matrix to change basis
    B << L, A; // Concatenate A and L vertically, with L on the top
    std::cout << "B: \n" << B << "\n";
    MatrixXd Binv = B.inverse(); // Will change basis back
    std::cout << "Binv: \n" << Binv << "\n";

    MatrixXd R(B.rows(), B.cols()); // Compute the rotation matrix
    R.setIdentity();
    R(0,0) = cos(theta);
    R(0,1) = -sin(theta);
    R(1,0) = sin(theta);
    R(1,1) = cos(theta);

    std::cout << "R: \n" << R << "\n";
    MatrixXd M = Binv * R * B; // Overall rotation: Change basis, rotate easily, change back
    return M * v;
}

/** Returns a matrix whose rows form an orthogonal basis for the left null space of A
 */
MatrixXd GetLeftNullSpaceOrtho(MatrixXd A) {
    JacobiSVD<MatrixXd> svd(A, Eigen::ComputeFullV);

    unsigned int r = 2; // rank(A); should be 2 for our purposes
    MatrixXd V(svd.matrixV().transpose());
    //std::cout << "V: \n" << V << "\n";
    MatrixXd N = V.block(V.rows() - r, 0, r, V.cols()); // Extract last r rows of V
    //std::cout << "N: \n" << N << "\n";
    return N;
}
