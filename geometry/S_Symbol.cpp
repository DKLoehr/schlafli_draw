#include "S_Symbol.h"
#include <sstream>

S_Symbol::S_Symbol()
{
    m_symbols = NULL;
    setPQR("{}");
}


S_Symbol::S_Symbol(std::string str) {
    setPQR(str);
}

S_Symbol::~S_Symbol()
{
    delete m_symbols;
}

S_Symbol::S_Symbol(const S_Symbol& other)
{
    m_symbols = new s_pair[other.m_dim];
    for(int iii = 0; iii < other.m_dim; iii++)
        m_symbols[iii] = other.m_symbols[iii];
    m_dim = other.m_dim;
}

S_Symbol& S_Symbol::operator=(const S_Symbol& rhs)
{
    if (this == &rhs) return *this; // handle self assignment

    delete m_symbols;
    m_symbols = new s_pair[rhs.m_dim];
    for(int iii = 0; iii < rhs.m_dim; iii++)
        m_symbols[iii] = rhs.m_symbols[iii];
    m_dim = rhs.m_dim;
    return *this;
}

s_pair* S_Symbol::GetSymbols() {
    return m_symbols;
}

unsigned int S_Symbol::GetDim() {
    return m_dim;
}

// Note: This assumes that the input string is a well-formed schlafli symbol
void S_Symbol::setPQR(std::string str) {
    delete m_symbols; // Going to be replacing all of this, so clean up memory
    m_original = str;
    m_dim = 1; // Min. dim of 1: we treat the 0-dim case as a degenerate 1-dim case
    for(int iii = 0; iii < str.length(); iii++) { // Calculate dimension
        if(str[iii] == ',') // Each comma increments by 1
            m_dim++;
    }

    m_symbols = new s_pair[m_dim];
    m_symbols[0] = s_pair{2, 1}; // Default for first pair if we have an empty symbol

    if(str.find('{') != std::string::npos) {  // If '{' is present (which it should be)
        str = str.substr(str.find('{') + 1, str.length() - 1); // Edit the string to remove it
    }
    if(str.find('}') != std::string::npos) {  // If '}' is present (which it also should be)
        str = str.substr(0, str.find('}')); // Edit the string to remove it
    }

    if(str.length() == 0) // Empty string, default values will work fine
        return;

    std::stringstream stream(str);
    unsigned int p = -1, q = -1;
    char c = 0;
    for(int iii = 0; !stream.eof(); iii++) {
        stream >> p; // Read the first integer (up to the /, comma or end of file)
        m_symbols[iii].first = p; // Guaranteed to be there if string is well-formed
        char c;
        stream >> c;
        if(c == '/') { // There is a second part, so set it
            stream >> q;
            m_symbols[iii].second = q;
            stream >> c; // Remove the comma or set eof bit
        } else { // Otherwise, no second part, so default to 1 and continue
            m_symbols[iii].second = 1;
        }
    }
}
