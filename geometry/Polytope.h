/**
 * This class is a representation of a regular polytope, stored as
 * a collection of adjacent vertices -- in other words, edges. Note
 * that despite the name, this only represents the edge set of regular
 * polytopes (those with schlafli symbols). Faces and irregular polytopes
 * are not intended to be represented using this class.
 */

#ifndef POLYTOPE_H
#define POLYTOPE_H
#include "S_Symbol.h"
#include "Polytope_helper.h"

typedef std::pair<VectorXd, VectorXd> vpair;

class Polytope {
private:
    S_Symbol m_symbol; // The schlafli symbol of the polytope
    unsigned int m_dim; // The dimension of the polytope (min 2); 1 greater than dim of the symbol
    std::vector<VectorXd> vertices;
    std::vector<vpair> edges;

public:
    Polytope();
    Polytope(S_Symbol symbol);
    Polytope(std::string str);

    ~Polytope();

    void create(std::string str);
};
#endif // POLYTOPE_H
