#include "Polytope.h"

Polytope::Polytope() {
    m_dim = m_symbol.GetDim() + 1;
}

Polytope::Polytope(S_Symbol symbol):
    m_symbol(symbol)
{
    m_dim = m_symbol.GetDim() + 1;
}

Polytope::Polytope(std::string str):
    m_symbol(str)
{
    m_dim = m_symbol.GetDim() + 1;
}

Polytope::~Polytope() {

}

void Polytope::create(std::string str) {
    m_symbol.setPQR(str);
    m_dim = m_symbol.GetDim() + 1;
}
