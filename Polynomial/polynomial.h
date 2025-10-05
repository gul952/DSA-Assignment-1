#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <string>
#include <map>

class Polynomial {
public:
    Polynomial() = default;
    virtual ~Polynomial() = default;

    // Insert a term into the polynomial (combine like terms, ignore zero coeff)
    virtual void insertTerm(int coefficient, int exponent);

    // Return polynomial as a human-readable string
    virtual std::string toString() const;

    // Return a new polynomial that is the sum of this and other
    virtual Polynomial add(const Polynomial& other) const;

    // Return a new polynomial that is the product of this and other
    virtual Polynomial multiply(const Polynomial& other) const;

    // Return a new polynomial that is the derivative of this polynomial
    virtual Polynomial derivative() const;

private:
    // sparse representation: exponent -> coefficient. kept sorted descending for printing.
    std::map<int, int, std::greater<int>> terms_;
};

#endif // POLYNOMIAL_H
