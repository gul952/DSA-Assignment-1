#include "Polynomial.h"
#include <sstream>
#include <cmath>

// Insert a term into the polynomial
void Polynomial::insertTerm(int coefficient, int exponent) {
    // Ignore negative exponents as per problem constraints
    if (exponent < 0) return; 
    
    // Skip zero coefficients - they don't contribute anything
    if (coefficient == 0) return;

    // Check if we already have a term with this exponent
    auto iterator = terms_.find(exponent);
    if (iterator != terms_.end()) {
        // Found existing term, add coefficients
        iterator->second += coefficient;
        // Remove term if coefficient becomes zero after addition
        if (iterator->second == 0) {
            terms_.erase(iterator);
        }
    } else {
        // New term, just add it
        terms_.emplace(exponent, coefficient);
    }
}

std::string Polynomial::toString() const {
    // Handle empty polynomial
    if (terms_.empty()) {
        return "0";
    }

    std::ostringstream output_stream;
    bool isFirst = true;
    
    // Iterate through terms (map keeps them sorted by exponent)
    for (const auto& term_pair : terms_) {
        int exp = term_pair.first;
        int coef = term_pair.second;
        
        // Skip zero coefficients (shouldn't happen but just in case)
        if (coef == 0) continue;

        // Handle sign for first term vs subsequent terms
        if (isFirst) {
            if (coef < 0) {
                output_stream << "-";
            }
        } else {
            if (coef < 0) {
                output_stream << " - ";
            } else {
                output_stream << " + ";
            }
        }

        int coefficient_abs = std::abs(coef);

        // Handle different cases for term formatting
        if (exp == 0) {
            // This is just a constant term
            output_stream << coefficient_abs;
        } else {
            // Non-constant term
            if (coefficient_abs != 1) {
                output_stream << coefficient_abs;  // Only show coefficient if it's not 1
            }
            output_stream << "x";
            if (exp != 1) {
                output_stream << "^" << exp;  // Only show exponent if it's not 1
            }
        }

        isFirst = false;
    }
    
    return output_stream.str();
}

// Add two polynomials together
Polynomial Polynomial::add(const Polynomial& other) const {
    Polynomial result_poly = *this; // Start with a copy of current polynomial

    // Add all terms from the other polynomial
    for (const auto& term : other.terms_) {
        int exp = term.first;
        int coef = term.second;
        result_poly.insertTerm(coef, exp);
    }
    
    return result_poly;
}

// Multiply two polynomials
Polynomial Polynomial::multiply(const Polynomial& other) const {
    Polynomial result_poly;
    
    // Multiply each term in this polynomial with each term in other polynomial
    for (const auto& term1 : terms_) {
        int exp1 = term1.first;
        int coef1 = term1.second;
        
        for (const auto& term2 : other.terms_) {
            int exp2 = term2.first;
            int coef2 = term2.second;
            
            // When multiplying terms: multiply coefficients, add exponents
            result_poly.insertTerm(coef1 * coef2, exp1 + exp2);
        }
    }
    
    return result_poly;
}

// Calculate derivative of polynomial
Polynomial Polynomial::derivative() const {
    Polynomial derivative_result;
    
    for (const auto& term : terms_) {
        int exp = term.first;
        int coef = term.second;
        
        // Derivative of constant term is 0, so skip it
        if (exp == 0) continue;
        
        // Power rule: d/dx(ax^n) = n*a*x^(n-1)
        derivative_result.insertTerm(coef * exp, exp - 1);
    }
    
    return derivative_result;
}