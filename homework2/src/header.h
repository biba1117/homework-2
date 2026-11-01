#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H
#include <iostream>
#include <cmath>
using namespace std;

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

    void newTerm(float coef, int exp) {
        if (coef == 0) return;
        if (terms == capacity) {
            Term* temp = new Term[capacity * 2];
            for (int i = 0; i < terms; i++)
                temp[i] = termArray[i];
            delete[] termArray;
            termArray = temp;
            capacity *= 2;
        }
        termArray[terms].coef = coef;
        termArray[terms].exp = exp;
        terms++;
    }

    // 找相同指數，沒有就新增，最後依指數排列。
    void addTerm(float coef, int exp) {
        if (coef == 0) return;
        for (int i = 0; i < terms; i++) {
            if (termArray[i].exp == exp) {
                termArray[i].coef += coef;
                if (termArray[i].coef == 0) {
                    for (int j = i; j < terms - 1; j++)
                        termArray[j] = termArray[j + 1];
                    terms--;
                }
                return;
            }
        }
        newTerm(coef, exp);
        for (int i = terms - 1; i > 0; i--) {
            if (termArray[i].exp > termArray[i - 1].exp) {
                Term temp = termArray[i];
                termArray[i] = termArray[i - 1];
                termArray[i - 1] = temp;
            } else break;
        }
    }

public:
    Polynomial() {
        capacity = 2;
        terms = 0;
        termArray = new Term[capacity];
    }
    ~Polynomial() { delete[] termArray; }

    // 複製資料，讓每個物件使用自己的陣列。
    Polynomial(const Polynomial& p) {
        capacity = p.capacity;
        terms = p.terms;
        termArray = new Term[capacity];
        for (int i = 0; i < terms && i < capacity; i++)
            termArray[i] = p.termArray[i];
    }
    Polynomial& operator=(const Polynomial& p) {
        if (this == &p) return *this;
        Term* temp = new Term[p.capacity];
        for (int i = 0; i < p.terms && i < p.capacity; i++)
            temp[i] = p.termArray[i];
        delete[] termArray;
        termArray = temp;
        capacity = p.capacity;
        terms = p.terms;
        return *this;
    }

    Polynomial Add(const Polynomial& poly) const {
        Polynomial result(*this);
        for (int i = 0; i < poly.terms; i++)
            result.addTerm(poly.termArray[i].coef, poly.termArray[i].exp);
        return result;
    }
    Polynomial Mult(const Polynomial& poly) const {
        Polynomial result;
        for (int i = 0; i < terms; i++) {
            for (int j = 0; j < poly.terms; j++) {
                float coef = termArray[i].coef * poly.termArray[j].coef;
                int exp = termArray[i].exp + poly.termArray[j].exp;
                result.addTerm(coef, exp);
            }
        }
        return result;
    }
    float Eval(float x) const {
        float sum = 0;
        for (int i = 0; i < terms; i++)
            sum += termArray[i].coef * (float)pow(x, termArray[i].exp);
        return sum;
    }

    // 用成員函式讀取 Term 的私有資料。
    ostream& print(ostream& out) const {
        if (terms == 0) return out << 0;
        for (int i = 0; i < terms; i++) {
            float coef = termArray[i].coef;
            int exp = termArray[i].exp;
            if (i > 0) out << (coef < 0 ? " - " : " + ");
            else if (coef < 0) out << '-';
            float absCoef = (coef < 0) ? -coef : coef;
            if (absCoef != 1 || exp == 0) out << absCoef;
            if (exp > 0) {
                out << 'x';
                if (exp > 1) out << '^' << exp;
            }
        }
        return out;
    }
    friend istream& operator>>(istream& in, Polynomial& p) {
        int n;
        if (!(in >> n)) return in;
        if (n < 0) { in.setstate(ios::failbit); return in; }
        Polynomial temp;
        for (int i = 0; i < n; i++) {
            float coef;
            int exp;
            if (!(in >> coef >> exp)) return in;
            if (exp < 0) { in.setstate(ios::failbit); return in; }
            temp.addTerm(coef, exp);
        }
        p = temp;
        return in;
    }
    friend ostream& operator<<(ostream& out, const Polynomial& p) {
        return p.print(out);
    }
};
#endif

