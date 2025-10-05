#include <iostream>
#include "Polynomial.h"

int main() {
    Polynomial p1;
    p1.insertTerm(3, 4);
    p1.insertTerm(2, 2);
    p1.insertTerm(-1, 1);
    p1.insertTerm(5, 0);

    Polynomial p2;
    p2.insertTerm(1, 4);
    p2.insertTerm(1, 0);

    Polynomial sum = p1.add(p2);

    Polynomial p3;
    p3.insertTerm(2, 1);

    Polynomial prod = sum.multiply(p3);
    Polynomial deriv = p1.derivative();

    std::cout << "p1.toString(): " << p1.toString() << std::endl;
    std::cout << "sum.toString(): " << sum.toString() << std::endl;
    std::cout << "prod.toString(): " << prod.toString() << std::endl;
    std::cout << "deriv.toString(): " << deriv.toString() << std::endl;

    // Extra test: cancellation
    Polynomial z;
    z.insertTerm(1, 1);
    z.insertTerm(-1, 1);
    std::cout << "zero (should be 0): " << z.toString() << std::endl;

    return 0;
}
