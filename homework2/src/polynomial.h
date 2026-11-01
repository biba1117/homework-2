#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>



class Polynomial;
class Term {
    friend class Polynomial;
private:
    float coef;
    int exp;
};

class Polynomial {
private:
    Term* termArray;
    int capacity;
    int terms;

    // Append a nonzero term; normalize after input and arithmetic.
    void newTerm(float coef, int exp) {
        if (!std::isfinite(coef)) throw std::string("Non-finite coefficient");
        if (coef == 0) return;
        if (terms == capacity) {
            if (capacity > static_cast<int>(~0u >> 1) / 2)
                throw std::string("Too many terms");
            Term* temp = new Term[capacity * 2];
            std::copy(termArray, termArray + terms, temp);
            delete[] termArray;
            termArray = temp;
            capacity *= 2;
        }
        termArray[terms].coef = coef;
        termArray[terms++].exp = exp;
    }

    // Normalize arbitrary input: sort, combine equal exponents, remove zeros.
    void normalize() {
        std::stable_sort(termArray, termArray + terms,
            [](const Term& a, const Term& b) { return a.exp > b.exp; });
        int out = 0;
        for (int i = 0; i < terms;) {
            int exponent = termArray[i].exp;
            float sum = 0;
            do { sum += termArray[i++].coef; }
            while (i < terms && termArray[i].exp == exponent);
            if (!std::isfinite(sum)) throw std::string("Coefficient overflow");
            if (sum != 0) {
                termArray[out].coef = sum;
                termArray[out++].exp = exponent;
            }
        }
        terms = out;
    }
public:
    Polynomial() : termArray(new Term[2]), capacity(2), terms(0) {}
    ~Polynomial() { delete[] termArray; }
    // Rule of Three: each polynomial owns its own dynamic array.
    Polynomial(const Polynomial& other)
        : termArray(new Term[other.capacity]), capacity(other.capacity), terms(other.terms) {
        std::copy(other.termArray, other.termArray + terms, termArray);
    }
    Polynomial& operator=(Polynomial other) {
        std::swap(termArray, other.termArray);
        std::swap(capacity, other.capacity);
        std::swap(terms, other.terms);
        return *this;
    }
    // const-reference parameters follow the supplementary lecture version.
    Polynomial Add(const Polynomial& poly) const {
        Polynomial result(*this);
        for (int i = 0; i < poly.terms; ++i) {
            bool found = false;
            for (int j = 0; j < result.terms; ++j) {
                if (result.termArray[j].exp == poly.termArray[i].exp) {
                    result.termArray[j].coef += poly.termArray[i].coef;
                    found = true;
                    break;
                }
            }
            if (!found) result.newTerm(poly.termArray[i].coef, poly.termArray[i].exp);
        }
        result.normalize();
        return result;
    }
    Polynomial Mult(const Polynomial& poly) const {
        Polynomial result;
        for (int i = 0; i < terms; ++i) {
            for (int j = 0; j < poly.terms; ++j) {
                if (termArray[i].exp > static_cast<int>(~0u >> 1) - poly.termArray[j].exp)
                    throw std::string("Exponent overflow");
                float newCoef = termArray[i].coef * poly.termArray[j].coef;
                int newExp = termArray[i].exp + poly.termArray[j].exp;
                if (!std::isfinite(newCoef)) throw std::string("Coefficient overflow");
                bool found = false;
                for (int k = 0; k < result.terms; ++k) {
                    if (result.termArray[k].exp == newExp) {
                        result.termArray[k].coef += newCoef;
                        found = true;
                        break;
                    }
                }
                if (!found) result.newTerm(newCoef, newExp);
            }
        }
        result.normalize();
        return result;
    }
    float Eval(float x) const {
        if (!std::isfinite(x)) throw std::string("x must be finite");
        float sum = 0;
        for (int i = 0; i < terms; ++i) {
            sum += termArray[i].coef * static_cast<float>(std::pow(x, termArray[i].exp));
            if (!std::isfinite(sum)) throw std::string("Evaluation overflow");
        }
        return sum;
    }
    // Input: count followed by (coefficient, exponent) pairs.
    // On failure, leave the original polynomial unchanged.
    friend std::istream& operator>>(std::istream& in, Polynomial& p) {
        int count;
        if (!(in >> count)) return in;
        if (count < 0) { in.setstate(std::ios::failbit); return in; }
        Polynomial temp;
        for (int i = 0; i < count; ++i) {
            float coef; int exp;
            if (!(in >> coef >> exp)) return in;
            if (!std::isfinite(coef) || exp < 0) {
                in.setstate(std::ios::failbit); return in;
            }
            temp.newTerm(coef, exp);
        }
        temp.normalize();
        p = temp;
        return in;
    }
    std::ostream& write(std::ostream& out) const {
        if (terms == 0) return out << 0;
        for (int i = 0; i < terms; ++i) {
            float c = termArray[i].coef;
            int e = termArray[i].exp;
            if (i) out << (c < 0 ? " - " : " + ");
            else if (c < 0) out << '-';
            float magnitude = std::fabs(c);
            if (e == 0 || magnitude != 1) out << magnitude;
            if (e > 0) { out << 'x'; if (e != 1) out << '^' << e; }
        }
        return out;
    }
    friend std::ostream& operator<<(std::ostream& out, const Polynomial& p) {
        return p.write(out);
    }
};
#endif




