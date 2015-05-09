/**
 * This class is a representation of schlafli symbols as an array of
 * pairs of integers. Each pair represents one term p/q in the symbol:
 * the first element of the pair is p, the second is q (1 if omitted)
 */


#ifndef S_SYMBOL_H
#define S_SYMBOL_H
#include <string>
#include <utility> // pair

typedef std::pair<unsigned int, unsigned int> s_pair; // Represents the p/q pairs inside the schlafli symbol

class S_Symbol
{
private:
    s_pair* m_symbols;  // A collection of the terms in the symbol
    unsigned int m_dim; // The size of m_symbols, the number of terms in the schlafli symbol
    std::string m_original; // The string we used to fill m_symbols and m_dim

public:
    S_Symbol(); // Default constructor: Uses the empty schlafli symbol as the input string
    S_Symbol(std::string str); // Constructs the symbol from a string representing the symbol
    ~S_Symbol(); // Destructor

    S_Symbol(const S_Symbol& other); // Copy constructor
    S_Symbol& operator=(const S_Symbol& other); // Assignment operator

    s_pair* GetSymbols();
    unsigned int GetDim();

    void setPQR(std::string str); // Fills m_symbols and updates m_dim from a string representing a schlafli symbol
};

#endif // S_SYMBOL_H
